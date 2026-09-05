// Trains AteNika's net: (768 -> 512)x2 -> 1, SCReLU, quantised for in-engine
// integer inference. Adapted from bullet's examples/simple.rs.

use bullet_lib::{
    game::inputs::Chess768,
    nn::optimiser::AdamW,
    trainer::{
        save::SavedFormat,
        schedule::{TrainingSchedule, TrainingSteps, lr, wdl},
        settings::LocalSettings,
    },
    value::{ValueTrainerBuilder, loader},
};

// These four and the SCReLU in build() must match src/nnue/nnue.hpp.
const HIDDEN_SIZE: usize = 512; // engine HIDDEN
const SCALE: i32 = 400; // engine SCALE, network output -> centipawns
const QA: i16 = 255; // engine QA, quantises feature weights and biases
const QB: i16 = 64; // engine QB, quantises output weights

// Training choices, free to tune.
const BATCH_SIZE: usize = 16_384;
const SUPERBATCHES: usize = 40; // passes over the data; the LR drops at 18

// bulletformat stores one position per 32 bytes.
const BYTES_PER_POSITION: u64 = 32;

fn main() {
    let data = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("usage: cargo run --release -- <dataset.bin>");
        std::process::exit(1);
    });

    let bytes = std::fs::metadata(&data)
        .unwrap_or_else(|e| panic!("{data}: {e}"))
        .len();
    assert!(
        bytes.is_multiple_of(BYTES_PER_POSITION),
        "{data} is not bulletformat: {bytes} bytes is not a multiple of {BYTES_PER_POSITION}"
    );

    let positions = (bytes / BYTES_PER_POSITION) as usize;
    let batches_per_superbatch = positions / BATCH_SIZE;
    println!("{data}: {positions} positions, {batches_per_superbatch} batches per superbatch");

    let mut trainer = ValueTrainerBuilder::default()
        .dual_perspective()
        .optimiser(AdamW)
        .inputs(Chess768)
        .save_format(&[
            SavedFormat::id("l0w").round().quantise::<i16>(QA),
            SavedFormat::id("l0b").round().quantise::<i16>(QA),
            SavedFormat::id("l1w").round().quantise::<i16>(QB),
            SavedFormat::id("l1b").round().quantise::<i16>(QA * QB),
        ])
        // target = wdl * game_result + (1 - wdl) * sigmoid(score / SCALE)
        .loss_fn(|output, target| output.sigmoid().squared_error(target))
        .build(|builder, stm_inputs, ntm_inputs| {
            let l0 = builder.new_affine("l0", 768, HIDDEN_SIZE);
            let l1 = builder.new_affine("l1", 2 * HIDDEN_SIZE, 1);

            let stm_hidden = l0.forward(stm_inputs).screlu();
            let ntm_hidden = l0.forward(ntm_inputs).screlu();
            l1.forward(stm_hidden.concat(ntm_hidden))
        });

    let schedule = TrainingSchedule {
        net_id: "atenika-v1".to_string(),
        eval_scale: SCALE as f32,
        steps: TrainingSteps {
            batch_size: BATCH_SIZE,
            batches_per_superbatch,
            start_superbatch: 1,
            end_superbatch: SUPERBATCHES,
        },
        // 0.75 leans on the search score over the game result: the labels come
        // from low-depth self-play, so the outcome is the noisier of the two.
        wdl_scheduler: wdl::ConstantWDL { value: 0.75 },
        lr_scheduler: lr::StepLR {
            start: 0.001,
            gamma: 0.1,
            step: 18,
        },
        save_rate: 10, // checkpoint every 10 superbatches
    };

    let settings = LocalSettings {
        threads: 6, // data loading only, the GPU does the arithmetic
        test_set: None,
        output_directory: "checkpoints",
        batch_queue_size: 64,
    };

    let data_loader = loader::DirectSequentialDataLoader::new(&[data.as_str()]);

    trainer.run(&schedule, &settings, &data_loader);
}
