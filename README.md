<div align="center">
  <img width="240" height="240" alt="AteNika" src="assets/logo-determination.png" />
  <h1>AteNika</h1>
  <p><i>A UCI chess engine written in C++23</i></p>

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![CI](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml/badge.svg)](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml)
[![Release](https://github.com/LesterEvSe/AteNika/actions/workflows/release.yml/badge.svg)](https://github.com/LesterEvSe/AteNika/actions/workflows/release.yml)
[![GitHub Release](https://img.shields.io/github/v/release/LesterEvSe/AteNika?logo=github&color=097BBC)](https://github.com/LesterEvSe/AteNika/releases/latest)
[![Elo](https://img.shields.io/badge/Elo-2512-brightgreen)](matches/LADDER.md)
</div>

AteNika plays chess over [UCI][uci] with no graphical interface of its own, and
has no dependencies beyond GoogleTest for the test build.

Briefly about the name. **AteNika** are the names of two ancient Greek goddesses
**Athena**, who was the patroness of wisdom, strategy, justice and war defense,
and **Nika** (or **Nike**), which is literally translated from Greek "Victory",
she was the ancient Greek goddess of victory and triumph.

[uci]: https://www.wbec-ridderkerk.nl/html/UCIProtocol.html

## Strength

Measured by gauntlet against externally rated opponents, 10+0.1, rated with
`ordo`. Full history in [matches/LADDER.md](matches/LADDER.md).

| version | opponent | games | rating |
| ------- | -------- | ----- | ------ |
| 0.5.0 | stash-2473 | 1000 | **2511.8 +/- 19.7** |
| 0.5.0 | stash-2298 | 1000 | 2457.8 +/- 21.4 |
| 0.4.0 | stash-2298 | 2000 | 2295.1 +/- 14.1 |
| 0.3.0 | stash-1690 | 1000 | 1816.7 +/- 21.6 |

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

**Evaluation**: material and piece-square tables, passed and isolated pawns,
rooks and queens on open and semi-open files, bishop pair, and insufficient
material detection.

## Build

Requires a C++23 compiler (GCC 14+, Clang 18+, MSVC 19.39+) and CMake 3.25+.
Linux and Windows are built and tested on every commit.

```bash
cmake --preset release
cmake --build --preset release
./build/release/AteNika
```

## Usage

AteNika uses [UCI][uci], so
any UCI-compatible GUI can run it. The only option is `Hash`, the transposition
table size in MB.

A few non-UCI commands are available for development:

| command | what it does |
| ------- | ------------ |
| `d` | print the current position |
| `eval` | static evaluation of the current position |
| `bench` | fixed position set at fixed depth, reported as one node count |
| `perft [divide] <n>` | node counts, `divide` matching Stockfish's `go perft` format |
| `state` | internal state of the current position |
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

## Credits

**Code and ideas**

- [Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) — heavily, in the early stages
- [Stockfish](https://github.com/official-stockfish/Stockfish)
- [Stash-bot](https://github.com/mhouppin/stash-bot) — the reference ladder for testing

**Reference**

- [Chess Programming Wiki](https://www.chessprogramming.org/Main_Page)
- [UCI protocol][uci]
- [FICS games database](https://www.ficsgames.org/)

## License

GPL-3.0-or-later. See [LICENSE](LICENSE).
