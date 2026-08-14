# Ladder

Every accepted binary, in order. The binaries themselves live in `engines/` and
are not tracked; this file is the record that says which SHA produced which one
and what its bench fingerprint was, so any rung can be rebuilt.

Appended to by `archive.sh`. Ratings are filled in by hand after a gauntlet.

| version | sha | bench | archived | rating |
| --------| --- | ----- | -------- | ------ |
| atenika-0.2.0    | 3736a3a  | 8831568    | 2026-08-11 | 1555 +/- 15 |
| atenika-dev      | 2868302  | 5010987    | 2026-08-12 | 16.31 +/- 14.64, stopped earlier after 1258 games |
| atenika-0.3.0    | a8d61cd  | 13459037   | 2026-08-14 | 254.73 +/- 34.82 |
