#!/usr/bin/env python3
"""Where the engine's thinking time actually goes. Two questions, two modes.

    ./timeman.py search <engine> ["go args"] [positions]   one position at a time
    ./timeman.py games  <pgn>                              a finished match

search: drives "go" over positions drawn from the opening book.

    ./timeman.py search ../build/release/AteNika "movetime 2000"
    ./timeman.py search ../build/release/AteNika "wtime 8000 btime 8000 winc 80 binc 80"

Read "unfinished" only in the wtime form. Under movetime the engine cannot bank
what it does not spend -- stopping early just idles until the movetime expires
and then plays the same move -- so an iteration that gets killed there costs
nothing, and a high number is correct behaviour rather than waste. Under a real
clock the unspent time carries to later moves, and then it is a real trade.

    ./timeman.py games results/sprt-20260826-221711.pgn

Positions are drawn with a fixed seed so a before/after comparison sees the same
ones. Override with SEED=... ; override the book with BOOK=... .
"""

import os
import random
import re
import statistics
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
BOOK = Path(os.environ.get("BOOK", HERE / "books" / "UHO_4060_v4.epd"))
SEED = int(os.environ.get("SEED", "42"))

# Fallback when the book is absent: a quiet opening, a sharp middlegame and an
# endgame, so the three very different time profiles are all represented.
BUILTIN = [
    "r1bqkb1r/pp3ppp/2n1pn2/2pp4/3P1B2/2P1PN2/PP1N1PPP/R2QKB1R w KQkq - 0 7",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
]

BESTMOVE_TIMEOUT_S = 600


def load_positions(count):
    if not BOOK.is_file():
        print(f"no book at {BOOK}, using the built-in positions", file=sys.stderr)
        return BUILTIN

    lines = [ln.split() for ln in BOOK.read_text().splitlines() if ln.strip()]
    fens = [" ".join(parts[:4] + ["0", "1"]) for parts in lines if len(parts) >= 4]
    if count >= len(fens):
        return fens
    return random.Random(SEED).sample(fens, count)


class Engine:
    """One long-lived process. Keeping stdin open is the whole point: the engine
    aborts its search when stdin closes, so a one-shot pipe measures nothing."""

    def __init__(self, path):
        self.proc = subprocess.Popen(
            [str(path)],
            stdin=subprocess.PIPE,
            stdout=subprocess.PIPE,
            stderr=subprocess.DEVNULL,
            text=True,
            bufsize=1,
        )

    def send(self, *commands):
        for command in commands:
            self.proc.stdin.write(command + "\n")
        self.proc.stdin.flush()

    def go(self, fen, go_args):
        """Returns (depth, last_iteration_ms, wall_ms) for one search."""
        self.send("ucinewgame", f"position fen {fen}", f"go {go_args}")

        depth = last_iter_ms = None
        started = time.monotonic()
        deadline = started + BESTMOVE_TIMEOUT_S

        while True:
            line = self.proc.stdout.readline()
            if not line:
                raise RuntimeError("engine closed its output")
            fields = line.split()
            if line.startswith("info depth"):
                for i, field in enumerate(fields[:-1]):
                    if field == "depth":
                        depth = int(fields[i + 1])
                    elif field == "time":
                        last_iter_ms = int(fields[i + 1])
            elif line.startswith("bestmove"):
                return depth, last_iter_ms, (time.monotonic() - started) * 1000
            if time.monotonic() > deadline:
                raise RuntimeError("timed out waiting for bestmove")

    def close(self):
        try:
            self.send("quit")
            self.proc.wait(timeout=10)
        except Exception:
            self.proc.kill()


def run_search(engine_path, go_args, count):
    if not os.access(engine_path, os.X_OK):
        sys.exit(f"not executable: {engine_path}")

    engine = Engine(engine_path)
    rows = []
    try:
        print(f"{'pos':<4} {'depth':<6} {'last-iter':<10} {'spent':<8} unfinished")
        for n, fen in enumerate(load_positions(count), start=1):
            depth, last_iter_ms, wall_ms = engine.go(fen, go_args)
            if depth is None:
                print(f"{n:<4} {'-':<6} {'-':<10} {wall_ms:<8.0f} -")
                continue
            unfinished = 100 * (wall_ms - last_iter_ms) / wall_ms if wall_ms > 0 else 0.0
            rows.append((depth, wall_ms, unfinished))
            print(f"{n:<4} {depth:<6} {last_iter_ms:<10} {wall_ms:<8.0f} {unfinished:.0f}%")
    finally:
        engine.close()

    if not rows:
        sys.exit("\nno completed searches")

    print(
        f"\nmean over {len(rows)} positions: "
        f"depth {statistics.mean(r[0] for r in rows):.1f}, "
        f"spent {statistics.mean(r[1] for r in rows):.0f} ms, "
        f"unfinished {statistics.mean(r[2] for r in rows):.0f}%"
    )


def run_games(pgn_path):
    path = Path(pgn_path)
    if not path.is_file():
        sys.exit(f"no such pgn: {path}")

    # fastchess writes "{+2.05/14 0.382s}" after every move, and a TimeControl
    # tag of the form "base+inc" in each game's header.
    text = path.read_text(errors="replace")
    games = text.split("[Event ")[1:]
    if not games:
        sys.exit("no games found")

    shares, move_counts = [], []
    for game in games:
        tc = re.search(r'\[TimeControl "([0-9.]+)\+([0-9.]+)"\]', game)
        if not tc:
            continue
        base, inc = float(tc.group(1)), float(tc.group(2))
        times = [float(t) for t in re.findall(r"\{[^}]*?([0-9.]+)s", game)]
        if len(times) < 20:  # adjudicated in the book, nothing to measure
            continue
        for side in (times[0::2], times[1::2]):
            shares.append(100 * sum(side) / (base + inc * len(side)))
            move_counts.append(len(side))

    if not shares:
        sys.exit("no games with usable clock comments")

    shares.sort()
    print(f"game-sides: {len(shares)}   mean moves/side: {statistics.mean(move_counts):.0f}")
    print(
        f"clock consumed: mean {statistics.mean(shares):.1f}%   "
        f"median {statistics.median(shares):.1f}%"
    )
    print(f"                p10 {shares[len(shares) // 10]:.1f}%   "
          f"p90 {shares[9 * len(shares) // 10]:.1f}%")
    print("\nTime still on the clock when a game ends never became depth.")


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)

    mode = argv[1]
    if mode == "search":
        if len(argv) < 3:
            sys.exit('usage: timeman.py search <engine> ["go args"] [positions]')
        go_args = argv[3] if len(argv) > 3 else "movetime 2000"
        count = int(argv[4]) if len(argv) > 4 else 15
        run_search(argv[2], go_args, count)
    elif mode == "games":
        if len(argv) < 3:
            sys.exit("usage: timeman.py games <pgn>")
        run_games(argv[2])
    else:
        sys.exit(__doc__)


if __name__ == "__main__":
    main(sys.argv)
