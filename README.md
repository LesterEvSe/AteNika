## Overview

A chess engine, **without graphical interface**, written in C++23.  
Briefly about the name. **AteNika** are the names of two ancient Greek goddesses **Athena**, who was the patroness of wisdom, strategy, justice and war defense, and **Nika**, which is literally translated from Greek "Victory", she was the ancient Greek goddess of victory and triumph.

## Build and Run

Requirements: a C++23 compiler (GCC 13+, Clang 16+, MSVC 19.35+) and CMake 3.25 or newer.

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

Release is the default when no build type is given.

| Option | Default | Meaning |
| --- | --- | --- |
| `ATENIKA_BUILD_TESTS` | `OFF` | Build the GoogleTest suite. Uses a system GoogleTest if one is installed, otherwise downloads it. |
| `ATENIKA_LTO` | `ON` | Link-time optimization in optimized builds. Worth roughly 1.7x on this codebase — leave it on. |
| `ATENIKA_NATIVE` | `OFF` | Build for the host CPU instead of the portable baseline. |
| `ATENIKA_ARCH` | `x86-64-v3` | Microarchitecture level for portable builds. Lower to `x86-64-v2` for older hardware. |
| `ATENIKA_WARNINGS` | `ON` | Compiler warning set. |

### Tests

``` Bash
cmake --preset tests
cmake --build --preset tests
ctest --preset tests        # everything, including perft
ctest --preset fast         # everything except perft
```

Or run the binary directly for finer control:

``` Bash
./build/tests/AteNikaTest --gtest_filter='PerftFixture.*'
```

### Windows

Open the folder in Visual Studio — it reads `CMakePresets.json` directly and offers the
presets above in the configuration dropdown. Any IDE with CMake and C++23 support works
the same way.

## Inspired by resources

### GitHub

[Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) a lot  
[Natrix](https://github.com/gth-other/Natrix)  
[Stockfish](https://github.com/official-stockfish/Stockfish)  

### Other

[Chessprogramming Wiki](https://www.chessprogramming.org/Main_Page)  
[Chess Database Information](https://chess.stackexchange.com/questions/18046/what-are-the-biggest-free-chess-game-databases-is-it-legal-to-download-games-fr)  
[FICS Database](https://www.ficsgames.org/)  
[UCI protocol](https://www.wbec-ridderkerk.nl/html/UCIProtocol.html)  
