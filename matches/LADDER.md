# Ladder

Every accepted binary, in order. The binaries themselves live in `engines/` and
are not tracked; this file is the record that says which SHA produced which one
and what its bench fingerprint was, so any rung can be rebuilt.

Appended to by `archive.sh`. Ratings are filled in by hand after a gauntlet.

## Gauntlets

| version | opponent | tc | games | rating |
| ------- | -------- | -- | ----- | ------ |
| atenika-0.2.0 | stash-1690 | 10+0.1 | 2000 | 1555 +/- 15     |
| atenika-0.3.0 | stash-1690 | 10+0.1 | 1000 | 1816.7 +/- 21.6 |
| atenika-0.3.0 | stash-1886 | 10+0.1 | 1000 | 1880.2 +/- 19.8 |

## SPRT

| version | sha | bench | archived | SPRT |
| --------| --- | ----- | -------- | -------------- |
| atenika-0.2.0     | 3736a3a  | 8831568    | 2026-08-11 | — |
| atenika-dev-0.3.0 | 2868302  | 5010987    | 2026-08-12 | +16.31 +/- 14.64 (1258 games, stopped early) |
| atenika-0.3.0    | a8d61cd  | 13459037   | 2026-08-14 | vs atenika-0.2.0 +254.73 +/- 34.82 (376 games) |
