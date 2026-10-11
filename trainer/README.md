# trainer

Trains AteNika's network: `(768 -> HIDDEN)x2 -> 1`, SCReLU, quantised for integer
inference in the engine. HIDDEN is 1024 by default and must match the engine's
`ATENIKA_HIDDEN`. Built on [bullet](https://github.com/jw1912/bullet).

## Requirements

- A supported GPU. There is no CPU backend.
- The CUDA toolkit, and a dataset in bulletformat (32 bytes per position).

Currently works with `g++-15`, so it need to be installed.

## Run

```bash
cargo run --release -- <path-to-dataset.bin>

# Every training setting can be overridden from the environment:
HIDDEN=512 WDL=0.0 SUPERBATCHES=40 BATCHES_PER_SUPERBATCH=1000 SAVE_RATE=5 NET_ID=e2-n5000 \
  cargo run --release -- <path-to-dataset.bin>

# Lint with
cargo fmt
cargo clippy --workspace --all-targets -- -D warnings
```

Checkpoints appear every SAVE_RATE (default 10) superbatches in `checkpoints/<NET_ID>-N/`.
The engine reads `quantised.bin`; `raw.bin` is the unquantised f32 weights and
`optimiser_state/` exists to resume training.

## Using a net in the engine

```bash
stat -c%s checkpoints/<NET_ID>-N/quantised.bin   # must equal sizeof(Network)
cp checkpoints/<NET_ID>-N/quantised.bin ../nets/atenika.nnue
cd .. && cmake --preset release && cmake --build --preset release
```

A size mismatch fails the build on `static_assert` in `src/nnue/nnue.cpp`.

