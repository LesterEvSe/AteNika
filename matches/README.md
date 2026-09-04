# matches

Engine-vs-engine testing. Distinct from `tests/`, which is the gtest unit suite.

## Layout

| directory | holds | so that |
|---|---|---|
| `tools/` | fastchess, ordo, Stockfish | Stockfish is a reference for `perft divide`, not an opponent |
| `engines/` | reference engine **binaries** | `ls engines/` *is* the opponent roster, so it must contain only executables |
| `builds/` | archived AteNika binaries | ladder rungs stay separate from the working build |
| `src/` | engine source checkouts | build trees stay out of the roster |
| `books/` | opening books | |
| `results/` | PGNs, logs, ordo output | |

## Scripts

| script | question it answers |
|---|---|
| `sprt.sh` | Is this change better than the previous build? |
| `gauntlet.sh` | What is this build's rating on the CCRL scale? |
| `archive.sh` | Freeze the current commit as a ladder rung. |
| `engine-check.sh` | Do the binaries in `engines/` speak UCI? |
| `moq.py`, `timeman.py` | Parse `debug on` output: move-ordering quality and time-management behaviour. |

Each script's own header carries its usage and flags.

```bash
bash sprt.sh <new> <old> [gainer|major|nonreg]   # accept or reject a change
bash gauntlet.sh <build> <opponent-with-elo-suffix> [games]
bash archive.sh <version>                        # append a rung to LADDER.md
```

`sprt.sh` saves state every 20 games, so Ctrl-C loses nothing — resume with
`bash sprt.sh --resume results/sprt-<stamp>.json`.

## Setup

```bash
mkdir -p matches/{tools,engines,builds,src,books,results}
```

### fastchess

<https://github.com/Disservin/fastchess> — check out a release tag. A mid-project
update from `main` can change adjudication defaults and shift every result.

```bash
git clone https://github.com/Disservin/fastchess
cd fastchess
git checkout <latest release tag>
make -j 6
cp fastchess ../tools/
```

Verify with the built-in compliance checker before trusting any match. It takes
the engine path directly, not `-engine cmd=`:

```bash
./tools/fastchess --compliance ../build/release/AteNika
./tools/fastchess -check-mate-pvs ../build/release/AteNika
```

The second one validates that a PV reported alongside a mate score really is a
mate — directly relevant now that the triangular PV table replaced the old
transposition-table walk.

### ordo

<https://github.com/michiguel/Ordo> converts a gauntlet PGN into ratings by
fitting the whole results graph at once, then shifts the solution so the anchor
lands on its published value.

```bash
git clone https://github.com/michiguel/Ordo
cd Ordo
make
cp ordo ../tools/
```

### Opening book

<https://github.com/official-stockfish/books>

`UHO_4060_v4.epd.zip` is the default the scripts expect. UHO = Unbalanced Human
Openings: positions with a built-in edge (here roughly +0.40 to +0.60), which
cuts the draw rate and so reaches an SPRT verdict in fewer games.

`8moves_v3.pgn.zip` from the same repo is a balanced alternative. Pass it with
`BOOK=./books/8moves_v3.pgn` and change `format=epd` to `format=pgn`.

> **NOTE: A book is not optional.**

## Opponents

Binaries go in `engines/` named **`<engine>-<elo>`**. `gauntlet.sh` reads the
rating off the trailing number, so anything without one does not belong there.

### Stash ladder

One engine family across many versions, used to track progress during
development.

<https://github.com/mhouppin/stash-bot> has no releases, so build from tags.

| tag | Elo | | tag | Elo |
| --- | --- | --- | --- | --- |
| `v9` | 1275 | | `v15` | 2140* |
| `v10` | 1620* | | `v17.0` | 2298 |
| `v11` | 1690 | | `v19.0` | 2473 |
| `v12` | 1886 | | `v20.0` | 2512 |
| `v13` | 1972 | | `v21.0` | 2714 |
| `v14` | 2060 | | `v25.0` | 2932 |

`*` = estimated, not CCRL-ranked.

### Cross-family pool

Different authors and architectures, used for rating measurement.

| engine | Elo | source |
| --- | --- | --- |
| [Reckless 0.3.0](https://github.com/codedeliveryservice/Reckless/tree/v0.3.0) | 2616 | built |
| [Monolith 1.0.2](https://github.com/cimarronOST/Monolith/tree/v1.02) | 2806 | built |
| [stash-bot 27.0](https://github.com/mhouppin/stash-bot/tree/v27.0) | 3049 | built |
| [akimbo 0.5.0](https://github.com/jw1912/akimbo/tree/v0.5.0) | 3051 | built |
| [FabChess 1.16](https://github.com/fabianvdW/FabChess/tree/v1.16) | 3051 | built |
| [Gyatso 1.3.0](https://github.com/GyatsoYT/GyatsoChess/tree/v1.3.0) | 3052 | official release |
| [Igel 2.5.0](https://github.com/vshcherbyna/igel/tree/2.5.0) | 3250 | built |


### Building the opponents

Target **`x86-64-v3`**, the same baseline as AteNika's `release` preset, so
neither side gets a hardware advantage.

**Igel**

```bash
sed -i 's/-march=native/-march=x86-64-v3/g; s/ -Werror//g' CMakeLists.txt
cmake -B build -S . -D_BTYPE=1 -DCMAKE_C_FLAGS="-O3 -march=x86-64-v3 -flto -DNDEBUG"
cmake --build build -j$(nproc)
```

**Monolith**

```bash
sed -i 's/-s -O2/-s -O2 -march=x86-64-v3/' makefile
make COMP=gcc ARCH=x64-popcnt
```

**stash-bot**

```bash
bash unix_build.sh        # or bash utils/unix_build.sh
```

**akimbo, Reckless, FabChess** (Rust)

```bash
RUSTFLAGS="-C target-cpu=x86-64-v3" cargo build --release
```

**Gyatso**, no build, use the official `linux-avx2` release binary.

### Verifying a binary

```bash
bash engine-check.sh          # do they all speak UCI
```
