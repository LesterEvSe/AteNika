<div align="center">
  <img width="350" height="350" alt="AteNika" src="assets/logo-determination.png" />
  <h1>AteNika</h1>
  <p><i>A UCI chess engine written in C++23</i></p>

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![CI](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml/badge.svg)](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml)
[![Release](https://github.com/LesterEvSe/AteNika/actions/workflows/release.yml/badge.svg)](https://github.com/LesterEvSe/AteNika/actions/workflows/release.yml)
[![GitHub Release](https://img.shields.io/github/v/release/LesterEvSe/AteNika?logo=github&color=097BBC)](https://github.com/LesterEvSe/AteNika/releases/latest)
[![Elo](https://img.shields.io/badge/Elo-3139-brightgreen)](matches/LADDER.md)
</div>

AteNika plays chess over [UCI][uci] with no graphical interface of its own, and
has no dependencies beyond GoogleTest for the test build.

Briefly about the name. **AteNika** are the names of two ancient Greek goddesses
**Athena**, who was the patroness of wisdom, strategy, justice and war defense,
and **Nika** (or **Nike**), which is literally translated from Greek "Victory",
she was the ancient Greek goddess of victory and triumph.

[uci]: https://www.wbec-ridderkerk.nl/html/UCIProtocol.html

## Strength

More detailed in [CCI AteNika Rate](https://github.com/computer-chess-index/cci/blob/main/engines/AteNika.md).
Full history in [matches/LADDER.md](matches/LADDER.md).

| Version | Published | STC <sub>8.0+0.08s  | LTC <sub>60.0+0.60s | VLTC <sub>2m24s+1.12s |
| --- | --- | --- | --- | --- |
| 0.7.0 | 2026-09-20 | 2728 | 3090 | 3139 |
| 0.6.0 | 2026-09-13 | 2674 | 3005 | 3078 |
| 0.5.0 | 2026-09-02 | 2040 | 2317 | 2350 |

## Features

**Board and move generation**: bitboards with magic sliding attacks,
make/unmake against a history stack, incremental Zobrist hashing, and pin-aware
legal move generation.

**Search**: fail-soft negamax with a triangular PV table, aspiration windows,
and a transposition table with depth-preferred replacement and a generation
counter. Pruning and reductions: late move reductions with history-based relief,
reverse futility, futility, internal iterative reduction, and adaptive null-move
R. Move ordering uses the TT move, MVV-LVA, killers and butterfly history with
gravity aging. Quiescence generates captures only, plus check evasions, filtered
by SEE and delta pruning. Time management runs separate soft and hard limits with
best-move stability and score-drop detection.

**Evaluation**: NNUE a `(768 -> 1024)x2 -> 1` network with SCReLU activation. Evaluation runs through an AVX2 path with a scalar fallback. The network is trained with [bullet](https://github.com/jw1912/bullet), see [trainer/](trainer/README.md).

## Build

Prebuilt binaries for Linux and Windows (x86-64 with AVX2) and macOS (arm64) are attached to every [release](https://github.com/LesterEvSe/AteNika/releases/latest).

Requires a C++23 compiler (GCC 14+, Clang 18+, MSVC 19.39+) and CMake 3.25+.
Linux, Windows and macOS are built and tested on every PR, plus a run under
AddressSanitizer and UndefinedBehaviorSanitizer.

```bash
cmake --preset release
cmake --build --preset release
./build/release/AteNika
```

The `release` preset targets x86-64-v3 (AVX2). To embed a different network,
point `ATENIKA_NET` at it and set `ATENIKA_HIDDEN` to its hidden layer width:

```bash
cmake --preset release -DATENIKA_NET=/path/to/net.nnue -DATENIKA_HIDDEN=512
```

## Usage

UCI options:

| option | default | what it does |
| ------ | ------- | ------------ |
| `Hash` | 16 | transposition table size in MB, 1 to 1024 |
| `Ponder` | false | think on the opponent's time |
| `EvalFile` | `<embedded>` | load a network of the same architecture from a file; on failure an `info string` is printed and the current net stays |

Non-UCI commands for development:

| command | what it does |
| ------- | ------------ |
| `d` | print the current position |
| `state` | verbose board dump plus the current search limits |
| `eval` | static evaluation of the current position |
| `bench [n]` | fixed position set at fixed depth `n`, reported as one node count |
| `perft [divide] <n>` | node counts, `divide` matching Stockfish's `go perft` format |
| `datagen <nodes> <positions> <seed> <out>` | self-play training data, `fen \| score \| result` per line |
| `relabel <nodes> <in> <out>` | re-search every position of a datagen file, keeping its result |
| `e2e4 e7e5 ...` | play moves in coordinate notation on the current position |
| `help` | list every command |

`bench` is the behaviour fingerprint: any change meant to preserve behaviour must
leave its node count bit-identical.

## Development

### Tests

```bash
cmake --preset tests && cmake --build --preset tests --parallel
ctest --preset tests     # everything
ctest --preset fast      # excludes perft
```

`test.sh` is the shortcut for single tests and takes a GoogleTest filter:

```bash
bash test.sh 'Board*'    # one fixture
bash test.sh -l          # list tests
```

The `verify` preset is the same suite with the Zobrist key and the NNUE
accumulator recomputed after every `make()`:
```bash
cmake --preset verify && cmake --build --preset verify --parallel
ctest --preset verify
printf 'bench\nquit\n' | ./build/verify/AteNika    # the same, over more positions
```

### Format and lint

The equivalent of `cargo fmt` + `cargo clippy`. Needs `clang-format` and
`clang-tidy`:

```bash
bash check.sh            # reformat in place, then lint
bash check.sh -n         # report only, what CI runs
```

CI pins clang-format 18.1.8, and a different patch level reformats the tree
differently. Match it locally:

```bash
pipx install clang-format==18.1.8
```

### Engine-vs-engine testing

Every change that alters what the engine plays is accepted or rejected by SPRT,
never by intuition. The tooling lives in [matches/](matches/README.md); binaries,
opponents and opening books are not tracked and must be set up locally.

### Training a network

Data comes from `datagen` (or `relabel` on an existing set), the trainer in
[trainer/](trainer/README.md) turns it into `quantised.bin`, and the result is
copied to `nets/atenika.nnue`, where the build embeds it.

## Credits

**Code and ideas**

- [Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) — heavily, in the early stages
- [Stockfish](https://github.com/official-stockfish/Stockfish)
- [Stash-bot](https://github.com/mhouppin/stash-bot) — the reference ladder for testing
- [bullet](https://github.com/jw1912/bullet) — the NNUE trainer

**Reference**

- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
- [Renegade chess engine training data](https://www.kaggle.com/datasets/pkrisz/renegade-chess-engine-training-data)
- [The Computer Chess Index (CCI)](https://github.com/computer-chess-index/cci)

## License

GPL-3.0-or-later. See [LICENSE](LICENSE).
