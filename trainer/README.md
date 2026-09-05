# trainer

Trains AteNika's network: `(768 -> 512)x2 -> 1`, SCReLU, quantised for integer
inference in the engine. Built on [bullet](https://github.com/jw1912/bullet).

## Requirements

- A supported GPU. There is no CPU backend.
- The CUDA toolkit, and a dataset in bulletformat (32 bytes per position).

Currently works with `g++-15`, so it need to be installed.

## Run

```bash
cargo run --release -- <path-to-dataset.bin>

# Lint with
cargo fmt
cargo clippy --workspace --all-targets -- -D warnings
```

Checkpoints appear every 10 superbatches at
`checkpoints/atenika-v1-N/atenika-v1-N.bin`.

## Using a net in the engine

```bash
stat -c%s checkpoints/atenika-v1-40/atenika-v1-40.bin   # must equal sizeof(Network)
cp checkpoints/atenika-v1-40/atenika-v1-40.bin ../nets/placeholder.nnue
cd .. && cmake --preset release && cmake --build --preset release
```

A size mismatch fails the build on `static_assert` in `src/nnue/nnue.cpp`.

