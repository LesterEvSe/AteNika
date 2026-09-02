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
| atenika-0.4.0 | stash-2298 | 10+0.1 | 2000 | 2295.1 +/- 14.1 |
| atenika-0.5.0 | stash-2298 | 10+0.1 | 1000 | 2457.8 +/- 21.4 |
| atenika-0.5.0 | stash-2473 | 10+0.1 | 1000 | 2511.8 +/- 19.7 |

## Rejected features

Kept here because a feature that measures flat is worth remembering — otherwise
it gets re-implemented, and each attempt costs a day of SPRT throughput.

| feature | roadmap | measured | verdict |
| ------- | ------- | -------- | ------- |
| Late move pruning (min_depth 2, quiet-only count, non-PV) | 3, *+20–30* | -12.87 +/- 26.07 (540 games, LLR -0.87) | dropped `af3c614` |
| Continuation history, no colour dimension | 10, *+25–40* | -76.13 +/- 48.10 (102 games, LOS 0.06%) | bug: every cell shared by both reply directions |
| Continuation history, colour-indexed | 10, *+25–40* | -44.41 +/- 49.85 (118 games, LOS 3.77%) | dropped; table too sparse to learn at these depths |
| History persisting across moves (9c) | part of 9 | -31.35 +/- 40.03 (200 games) | reverted; also broke `bench`, `PvTest`, `MateTest` |
| Butterfly + gravity, per-search clear (9a+9b) | 9, *+10–20* | -0.00 +/- 40.58 (180 games, PairsRatio 1.04) | kept as a correctness fix, not a gainer |

**The pattern.** Every one of these touches quiet-move ordering in the main
search

## SPRT

| version | sha | bench | archived | SPRT |
| --------| --- | ----- | -------- | -------------- |
| atenika-0.2.0     | 3736a3a  | 8831568    | 2026-08-11 | — |
| atenika-dev-0.3.0 | 2868302  | 5010987    | 2026-08-12 | +16.31 +/- 14.64 (1258 games, stopped early) |
| atenika-0.3.0    | a8d61cd  | 13459037   | 2026-08-14 | vs atenika-0.2.0 +254.73 +/- 34.82 (376 games) H1 \[0; 5] accepted |
| atenika-0.3.1   | 490b85d  | 2719121    | 2026-08-17 | vs previous +92.57 +/- 31.17 H1 \[0; 10] accepted |
| atenika-0.3.1-alt | 9dc3f93  | 1435641    | 2026-08-17 | vs previous +11.33 +/- 24.75 (460 games, stopped early), decline feature |
| atenika-0.3.2   | bc72f9d  | 19304650   | 2026-08-18 | vs 0.3.1 +43 +/- 14.80 H1 \[0; 5] accepted |
| atenika-0.3.3   | 14a9a08  | 5212673    | 2026-08-19 | +118.70 +/- 34.98 H1 \[0; 10] accepted |
| atenika-0.3.4    | af3c614  | 3892577    | 2026-08-21 | dropped LMP (min_depth 2, quiet-only count, non-PV) -12.87 +/- 26.07 (540 games, LLR -0.87), stopped early; no measurable gain, not shipped |
| atenika-0.3.5    | 92cdf2e  | 3783197    | 2026-08-26 | vs 0.3.3 +30.50 +/- 12.35 H1 \[0; 5] accepted |
| atenika-0.3.6    | 05b9ed8  | 2770619    | 2026-08-26 | +56.86 +/- 22.58 H1 \[0; 10] accepted |
| atenika-0.3.7    | fe08dad  | 2770619    | 2026-08-28 | +66.55 +/- 25.49 H1 \[0; 10] accepted |
| atenika-0.3.8    | 9a418fb  | 2724987    | 2026-08-30 | butterfly history + gravity + history-based LMR relief; +4.32 +/- 30.28 (322 games, LLR 0.01). Not resolved, shipped as a correctness fix. Drop history *heuristic*, costs 15.4% more nodes. |
| atenika-0.4.1    | 1fbe83f  | 16659372   | 2026-08-30 | vs 124e296 +36.73 +/- 18.70 H1 \[0; 10] accepted |
| atenika-0.4.2    | 871b439  | 16990797   | 2026-08-30 | +26.71 +/- 14.93 H1 \[0; 10] accepted |
| atenika-0.4.3    | 90fbd55  | 16191153   | 2026-09-01 | +14.88 +/- 16.03 (1098 games, stopped early). Change accepted |
