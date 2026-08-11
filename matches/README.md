# matches

Engine-vs-engine testing. Distinct from `tests/`, which is the gtest unit suite.

| directory | holds | so that |
|---|---|---|
| `tools/` | fastchess, ordo |  |
| `engines/` | reference engine **binaries** | `ls engines/` *is* the opponent roster, must contain only executable files |
| `builds/` | archived AteNika binaries | it is different from global build to distinguish  |
| `src/` | engine source checkouts | build trees stay out of the roster |
| `books/` | opening books | |
| `results/` | PGNs, logs, ordo output | |

| script | question it answers |
|---|---|
| `sprt.sh` | Is this change better than the previous build? |
| `gauntlet.sh` | What is this build's rating on the CCRL scale? |
| `archive.sh` | Freeze the current commit as a ladder rung. |
| `engine-check.sh` | Does engine works correctly? |

Each script's own header carries its usage and flags.

## Setup

``` bash
mkdir -p matches/{tools,engines,builds,src,books,results}
```

### fastchess

<https://github.com/Disservin/fastchess> check out a release tag, since mid-project update (in `main`) can change adjudication defaults and shift every result.

``` bash
git clone https://github.com/Disservin/fastchess
cd fastchess

git checkout <latest release tag>
make -j 6

cp fastchess ../tools/
```

Verify with the built-in compliance checker before trusting any match. It takes
the engine path directly, not `-engine cmd=`:

``` bash
./tools/fastchess --compliance ../build/release/AteNika
./tools/fastchess -check-mate-pvs ../build/release/AteNika
```

The second one validates that a PV reported alongside a mate score really is a
mate — directly relevant now that the triangular PV table replaced the old
transposition-table walk.

### ordo

<https://github.com/michiguel/Ordo> used to converts a gauntlet PGN into ratings by
fitting the whole results graph at once, then shifts the solution so the
anchor lands on its published value.

``` bash
git clone https://github.com/michiguel/Ordo
cd Ordo

make

cp ordo ../tools/
```

### Opening book

<https://github.com/official-stockfish/books>

`UHO_4060_v4.epd.zip` is the default the scripts expect. UHO = Unbalanced Human
Openings: positions with a built-in edge (here roughly +0.40 to +0.60), which
cuts the draw rate and so reaches an SPRT verdict in fewer games. The `4060`
family is gentler than `+085`, which suits engines at this level — a
strongly unbalanced start just hands the weaker side a loss and tells you
nothing.

`8moves_v3.pgn.zip` from the same repo is a balanced alternative; pass it with
`BOOK=./books/8moves_v3.pgn` to `gauntlet.sh` and `sprt.sh` and change `format=epd` to `format=pgn`.

**A book is not optional.** Without one every game starts from the same
position, so a thousand games are a handful of near-identical games repeated.
The effective sample size collapses while the ± Elo formula keeps assuming
independence, and SPRT then accepts or rejects on noise. Same reason `bench`
uses fifteen positions rather than `startpos` fifteen times.

### Reference engines

<https://github.com/mhouppin/stash-bot> has no releases, so build from tags.
Clone into `src/`, put only the binaries in `engines/`, named
**`<engine>-<elo>`**. `gauntlet.sh` reads the rating off the trailing number, so
it cannot be paired with the wrong binary, and `ls engines/` shows the ladder
with its ratings.

Published CCRL Blitz ratings (`*` = estimated, not CCRL-ranked):

|  tag    |  Elo  |
| ------- | ----- |
| `v9`    | 1275  |
| `v10`   | 1620* |
| `v11`   | 1690  |
| `v12`   | 1886  |
| `v13`   | 1972  |
| `v14`   | 2060  |
| `v15`   | 2140* |
| `v16.0` | 2220* |
| `v17.0` | 2298  |

After that, you can run `bash gauntlet.sh` to test the Elo of the current engine, or `bash sprt.sh` to check the difference between two engines.

> Note: Keep one opponent from the old pool whenever we move the ladder up, or ordo
cannot chain the two pools onto one scale and the progression becomes
uncomparable.
