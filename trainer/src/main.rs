// Trains AteNika's net: (768 -> HIDDEN)x2 -> 1, SCReLU, quantised for in-engine
// integer inference. Adapted from bullet's examples/simple.rs.
//
// Every setting below the quantisation constants can be overridden from the
// environment, e.g. HIDDEN=512 WDL=0.0 NET_ID=b5k cargo run --release -- data.bin

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

// These three and the SCReLU in build() must match src/nnue/nnue.hpp, and
// HIDDEN must match the engine's ATENIKA_HIDDEN.
const SCALE: i32 = 400; // engine SCALE, network output -> centipawns
const QA: i16 = 255; // engine QA, quantises feature weights and biases
const QB: i16 = 64; // engine QB, quantises output weights

const BATCH_SIZE: usize = 16_384;

// bulletformat stores one position per 32 bytes.
const BYTES_PER_POSITION: u64 = 32;

const CHECKPOINTS: &str = "checkpoints";

fn setting<T: std::str::FromStr>(name: &str, default: T) -> T {
    match std::env::var(name) {
        Ok(value) => value
            .parse()
            .unwrap_or_else(|_| panic!("{name} has an unusable value {value:?}")),
        Err(_) => default,
    }
}

fn main() {
    let data = std::env::args().nth(1).unwrap_or_else(|| {
        eprintln!("usage: cargo run --release -- <dataset.bin>");
        std::process::exit(1);
    });

    let hidden: usize = setting("HIDDEN", 1024);

    // 6104 * 16384 ~ 100M positions per superbatch.
    let batches_per_superbatch: usize = setting("BATCHES_PER_SUPERBATCH", 6104);

    // LR step is 45% from it.
    let superbatches: usize = setting("SUPERBATCHES", 120);

    // target = wdl * game_result + (1 - wdl) * sigmoid(score / SCALE)
    let wdl: f32 = setting("WDL", 0.3);
    let save_rate: usize = setting("SAVE_RATE", 10);
    let net_id: String = setting("NET_ID", "atenika-v1".to_string());

    // RESUME=<superbatch> picks up from that checkpoint.
    let resume: Option<usize> = std::env::var("RESUME").ok().map(|v| {
        v.parse()
            .unwrap_or_else(|_| panic!("RESUME wants a superbatch number, got {v:?}"))
    });

    let bytes = std::fs::metadata(&data)
        .unwrap_or_else(|e| panic!("{data}: {e}"))
        .len();
    assert!(
        bytes.is_multiple_of(BYTES_PER_POSITION),
        "{data} is not bulletformat: {bytes} bytes is not a multiple of {BYTES_PER_POSITION}"
    );

    // How often the data gets re-read.
    let positions = bytes / BYTES_PER_POSITION;
    let fed = (superbatches * batches_per_superbatch * BATCH_SIZE) as f64;
    println!(
        "{data}: {positions} positions, read {:.1} times across {superbatches} superbatches",
        fed / positions as f64
    );
    println!(
        "{net_id}: hidden {hidden}, wdl {wdl}, {batches_per_superbatch} batches per superbatch"
    );

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
        .loss_fn(|output, target| output.sigmoid().squared_error(target))
        .build(|builder, stm_inputs, ntm_inputs| {
            let l0 = builder.new_affine("l0", 768, hidden);
            let l1 = builder.new_affine("l1", 2 * hidden, 1);

            let stm_hidden = l0.forward(stm_inputs).screlu();
            let ntm_hidden = l0.forward(ntm_inputs).screlu();
            l1.forward(stm_hidden.concat(ntm_hidden))
        });

    let schedule = TrainingSchedule {
        net_id: net_id.clone(),
        eval_scale: SCALE as f32,
        steps: TrainingSteps {
            batch_size: BATCH_SIZE,
            batches_per_superbatch,
            start_superbatch: resume.map_or(1, |n| n + 1),
            end_superbatch: superbatches,
        },

        wdl_scheduler: wdl::ConstantWDL { value: wdl },
        lr_scheduler: lr::StepLR {
            start: 0.001,
            gamma: 0.1,
            step: (superbatches * 45 / 100).max(1),
        },
        save_rate,
    };

    let settings = LocalSettings {
        threads: 6, // data loading only, the GPU does the arithmetic
        test_set: None,
        output_directory: CHECKPOINTS,
        batch_queue_size: 64,
    };

    let data_loader = loader::DirectSequentialDataLoader::new(&[data.as_str()]);

    if let Some(n) = resume {
        let from = format!("{CHECKPOINTS}/{net_id}-{n}");
        println!("resuming from {from}, continuing at superbatch {}", n + 1);
        trainer.load_from_checkpoint(&from);
    }

    trainer.run(&schedule, &settings, &data_loader);
}
