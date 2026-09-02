"""Move-ordering quality: the share of fail-highs that resolve on the first
searched move, in the main search and in quiescence.

    ./moq.py <engine> ["go args"] [positions]

    ./moq.py ../build/release/AteNika
    ./moq.py ../build/release/AteNika "depth 14" 20
    ./moq.py ../build/release/AteNika "movetime 3000"
"""

import os
import random
import re
import subprocess
import sys
import time
from pathlib import Path

HERE = Path(__file__).resolve().parent
BOOK = Path(os.environ.get("BOOK", HERE / "books" / "UHO_4060_v4.epd"))
SEED = int(os.environ.get("SEED", "42"))

# Fallback when the book is absent.
BUILTIN = [
    "r4rk1/1pp1qppp/p1np1n2/2b1p1B1/2B1P1b1/P1NP1N2/1PP1QPPP/R4RK1 w - - 0 1",
    "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1",
    "2rr3k/pp3pp1/1nnqbN1p/3pN3/2pP4/2P3Q1/PPB4P/R4RK1 w - - 0 1",
    "3r1rk1/p3qp1p/2bb2p1/2p5/3P4/1P6/PBQN1PPP/2R2RK1 w - - 0 1",
    "r1bq1rk1/pp2ppbp/2np1np1/8/3NP3/2N1BP2/PPPQ2PP/R3KB1R w KQ - 0 1",
    "8/2p5/3p4/KP5r/1R3p1k/8/4P1P1/8 w - - 0 1",
]

MOQ_RE = re.compile(
    r"moq main (\d+)% \(fh (\d+) fhf (\d+)\) qs (\d+)% \(fh (\d+) fhf (\d+)\)"
)
BESTMOVE_TIMEOUT_S = 600

NEEDS_VALUE = {"depth", "movetime", "nodes", "mate",
               "wtime", "btime", "winc", "binc", "movestogo"}


def check_go_args(go_args):
    tokens = go_args.split()

    if not tokens:
        sys.exit('empty go args; try "depth 13"')

    if tokens[-1] in NEEDS_VALUE:
        sys.exit(f'go args end on "{tokens[-1]}" with no value -- quote them:\n'
                 f'    moq.py <engine> "{go_args} N"')

    if "infinite" in tokens:
        sys.exit("go infinite never returns bestmove; give a depth, movetime or nodes limit")

    if not any(token in NEEDS_VALUE for token in tokens):
        sys.exit(f'no search limit in "{go_args}"; the engine would search forever')


def share(part, total):
    return 100.0 * part / total if total else 0.0


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
        self.send("uci", "debug on")

    def send(self, *commands):
        for command in commands:
            self.proc.stdin.write(command + "\n")
        self.proc.stdin.flush()

    def go(self, fen, go_args):
        """Returns [(depth, main_fh, main_fhf, qs_fh, qs_fhf)], one per iteration."""
        self.send("ucinewgame", f"position fen {fen}", f"go {go_args}")

        rows, depth = [], None
        deadline = time.monotonic() + BESTMOVE_TIMEOUT_S

        while True:
            line = self.proc.stdout.readline()
            if not line:
                raise RuntimeError("engine closed its output")

            if line.startswith("info depth"):
                fields = line.split()
                for i, field in enumerate(fields[:-1]):
                    if field == "depth":
                        depth = int(fields[i + 1])
                        break
            elif line.startswith("info string moq"):
                found = MOQ_RE.search(line)
                if found and depth is not None:
                    rows.append((depth, int(found.group(2)), int(found.group(3)),
                                 int(found.group(5)), int(found.group(6))))
            elif line.startswith("bestmove"):
                return rows

            if time.monotonic() > deadline:
                raise RuntimeError("timed out waiting for bestmove")

    def close(self):
        try:
            self.send("quit")
            self.proc.wait(timeout=10)
        except Exception:
            self.proc.kill()


def per_iteration(rows):
    """Cumulative counters differenced into what each iteration alone did."""
    out, prev = [], (0, 0, 0, 0)
    for depth, m_fh, m_fhf, q_fh, q_fhf in rows:
        out.append((depth, m_fh - prev[0], m_fhf - prev[1],
                    q_fh - prev[2], q_fhf - prev[3]))
        prev = (m_fh, m_fhf, q_fh, q_fhf)
    return out


def main(argv):
    if len(argv) < 2:
        sys.exit(__doc__)

    engine_path = argv[1]
    go_args = argv[2] if len(argv) > 2 else "depth 13"
    count = int(argv[3]) if len(argv) > 3 else 15

    check_go_args(go_args)
    if not os.access(engine_path, os.X_OK):
        sys.exit(f"not executable: {engine_path}")

    engine = Engine(engine_path)
    totals = [0, 0, 0, 0]
    by_depth = {}

    try:
        print(f"{'pos':<4} {'depth':<6} {'main':<6} {'qs':<6} main fh/fhf")
        for n, fen in enumerate(load_positions(count), start=1):
            rows = engine.go(fen, go_args)
            if not rows:
                print(f"{n:<4} {'-':<6} {'-':<6} {'-':<6} -")
                continue

            depth, m_fh, m_fhf, q_fh, q_fhf = rows[-1]
            for i, value in enumerate((m_fh, m_fhf, q_fh, q_fhf)):
                totals[i] += value

            for d, dm_fh, dm_fhf, dq_fh, dq_fhf in per_iteration(rows):
                bucket = by_depth.setdefault(d, [0, 0, 0, 0])
                for i, value in enumerate((dm_fh, dm_fhf, dq_fh, dq_fhf)):
                    bucket[i] += value

            print(f"{n:<4} {depth:<6} {share(m_fhf, m_fh):<6.0f} "
                  f"{share(q_fhf, q_fh):<6.0f} {m_fh}/{m_fhf}")
    finally:
        engine.close()

    if not totals[0]:
        sys.exit("\nno fail-highs recorded")

    print(f"\n{'depth':<6} {'main':<6} {'qs':<6} main fh (this iteration only)")
    for d in sorted(by_depth):
        m_fh, m_fhf, q_fh, q_fhf = by_depth[d]
        if not m_fh:
            continue
        print(f"{d:<6} {share(m_fhf, m_fh):<6.0f} {share(q_fhf, q_fh):<6.0f} {m_fh}")

    print(f"\noverall  main {share(totals[1], totals[0]):.1f}% "
          f"(fh {totals[0]} fhf {totals[1]})")
    print(f"         qs   {share(totals[3], totals[2]):.1f}% "
          f"(fh {totals[2]} fhf {totals[3]})")


if __name__ == "__main__":
    main(sys.argv)
