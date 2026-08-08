[![CI](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml/badge.svg)](https://github.com/LesterEvSe/AteNika/actions/workflows/ci.yml)

## Overview

A chess engine, **without graphical interface**, written in C++23.  
Briefly about the name. **AteNika** are the names of two ancient Greek goddesses **Athena**, who was the patroness of wisdom, strategy, justice and war defense, and **Nika**, which is literally translated from Greek "Victory", she was the ancient Greek goddess of victory and triumph.

## Build and Run

Requirements: a C++23 compiler (GCC 14+, Clang 18+, MSVC 19.39+) and CMake 3.25 or newer.
Linux and Windows are built and tested on every commit.

### Using presets

The project ships a `CMakePresets.json`, so a build is two commands from the repository root:

``` Bash
cmake --preset release
cmake --build --preset release
./build/release/AteNika
```

### Without presets

``` Bash
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j
./build/AteNika
```

## Development

### Format and lint

The equivalent of `cargo fmt` + `cargo clippy`. Needs `clang-format` and `clang-tidy`:

``` Bash
bash check.sh          # reformat in place, then lint
```

CI pins clang-format 18.1.8. Match it locally.

``` Bash
pipx install clang-format==18.1.8
```

### Sanitizers

The suite under AddressSanitizer and UndefinedBehaviorSanitizer. They are catches out-of-bounds
table indexing and undefined behaviour, which a bitboard engine hits silently rather than
crashing. Roughly 7x slower than the `tests` preset.

``` Bash
cmake --preset sanitize
cmake --build --preset sanitize
ctest --preset sanitize
```

## Inspired by resources

### GitHub

[Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) a lot  
[Stockfish](https://github.com/official-stockfish/Stockfish)  

### Other

[Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page)  
[Chess Database Information](https://chess.stackexchange.com/questions/18046/what-are-the-biggest-free-chess-game-databases-is-it-legal-to-download-games-fr)  
[FICS Database](https://www.ficsgames.org/)  
[UCI protocol](https://www.wbec-ridderkerk.nl/html/UCIProtocol.html)  
