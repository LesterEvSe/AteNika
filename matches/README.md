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
