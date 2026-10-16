# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Yevhenii Sekhin
#
# Joins ratings (rate.sh CSV) with validation losses (eval_loss.py CSV) and draws
# the figures of the report. Names follow NET_ID-superbatch, with the budget in
# NET_ID as "-n<nodes>": e2-n5000-40 is experiment 2, 5000 nodes, superbatch 40.
#
#   python3 plots.py --ratings ../results/rate-X.csv --loss ref-loss.csv [--own own-loss.csv]
#                    [--exp e2] [--final ../results/picked.txt] [--out figs]
#
# --final names the checkpoint that stands for each budget (pick.sh writes it);
# without it the last superbatch does.
#
# Prints the fitted optimum and Spearman correlations; writes PNGs when
# matplotlib is installed.

import argparse
import csv
import re
from pathlib import Path

import numpy as np

# ordo's ERROR column is a 95% half-width (its -F default).
Z95 = 1.96
BOOTSTRAP = 4000
NAME = re.compile(r"^(?P<exp>[^-]+)-n(?P<nodes>\d+)-(?P<superbatch>\d+)$")


def read_ratings(path):
    ratings = {}
    with open(path) as file:
        for row in csv.DictReader(file):
            error = row["ERROR"]
            ratings[row["PLAYER"]] = (float(row["RATING"]), float(error) if error != "-" else 0.0)
    return ratings


def read_loss(path):
    losses = {}
    with open(path) as file:
        for row in csv.DictReader(file):
            losses[Path(row["net"]).parent.name] = float(row["mse_score"])
    return losses


def ranks(values):
    order = np.argsort(values, kind="stable")
    result = np.empty(len(values))
    result[order] = np.arange(len(values))
    for value in np.unique(values):
        tied = values == value
        result[tied] = result[tied].mean()
    return result


def spearman(x, y):
    return float(np.corrcoef(ranks(x), ranks(y))[0, 1])


def spearman_interval(x, y, rng):
    samples = []
    for _ in range(BOOTSTRAP):
        pick = rng.integers(0, len(x), len(x))
        if np.ptp(x[pick]) > 0 and np.ptp(y[pick]) > 0:
            samples.append(spearman(x[pick], y[pick]))
    return np.percentile(samples, [2.5, 97.5])


def optimum(log_nodes, elo, error, rng):
    """Vertex of a weighted parabola through Elo(log2 nodes), with a bootstrap
    over the rating errors. None when the fit opens upwards."""

    def vertex(values):
        a, b, _ = np.polyfit(log_nodes, values, 2, w=Z95 / np.maximum(error, 1.0))
        return -b / (2 * a) if a < 0 else None

    best = vertex(elo)
    draws = [vertex(rng.normal(elo, error / Z95)) for _ in range(BOOTSTRAP)]
    draws = [v for v in draws if v is not None and log_nodes.min() <= v <= log_nodes.max()]
    interval = np.percentile(draws, [2.5, 97.5]) if draws else None
    return best, interval


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--ratings", required=True)
    parser.add_argument("--loss", required=True, help="eval_loss.py on the reference set")
    parser.add_argument("--own", help="eval_loss.py on each net's own training labels")
    parser.add_argument("--exp", help="experiment prefix for the Elo(nodes) curve")
    parser.add_argument("--final", help="checkpoint names standing for each budget, one per line")
    parser.add_argument("--out", default="figs")
    args = parser.parse_args()

    rng = np.random.default_rng(1)
    ratings = read_ratings(args.ratings)
    losses = {"reference": read_loss(args.loss)}
    if args.own:
        losses["own labels"] = read_loss(args.own)

    try:
        import matplotlib

        matplotlib.use("Agg")
        import matplotlib.pyplot as plt
    except ImportError:
        plt = None
        print("matplotlib is not installed: numbers only, no figures")
    Path(args.out).mkdir(exist_ok=True)

    if args.exp:
        picked = None
        if args.final:
            with open(args.final) as file:
                picked = {line.strip() for line in file if line.strip()}

        # Each budget is represented by its picked checkpoint, else its last one.
        final = {}
        for name in ratings:
            match = NAME.match(name)
            if not match or match["exp"] != args.exp or (picked is not None and name not in picked):
                continue
            nodes, superbatch = int(match["nodes"]), int(match["superbatch"])
            if superbatch >= final.get(nodes, ("", -1))[1]:
                final[nodes] = (name, superbatch)

        nodes = np.array(sorted(final), dtype=float)
        elo = np.array([ratings[final[n][0]][0] for n in nodes])
        error = np.array([ratings[final[n][0]][1] for n in nodes])

        print(f"{args.exp}: Elo by nodes per position")
        for n, e, err in zip(nodes, elo, error):
            print(f"  {int(n):>8}  {e:+7.1f} +/- {err:.1f}  {final[n][0]}")

        if len(nodes) >= 3:
            best, interval = optimum(np.log2(nodes), elo, error, rng)
            if best is None:
                print("  no interior optimum: the fitted parabola opens upwards")
            else:
                text = f"  optimum near {2 ** best:.0f} nodes"
                if interval is not None:
                    text += f", 95% [{2 ** interval[0]:.0f}, {2 ** interval[1]:.0f}]"
                print(text)

        if plt:
            fig, ax = plt.subplots(figsize=(6, 4))
            ax.errorbar(nodes, elo, yerr=error, fmt="o-", capsize=3)
            ax.set_xscale("log", base=2)
            ax.set_xlabel("nodes per position")
            ax.set_ylabel("Elo vs anchor")
            ax.grid(alpha=0.3)
            fig.tight_layout()
            fig.savefig(Path(args.out) / f"{args.exp}-elo-by-nodes.png", dpi=200)

    for label, loss in losses.items():
        names = sorted(set(loss) & set(ratings))
        if len(names) < 3:
            print(f"{label}: fewer than 3 nets with both a loss and a rating")
            continue

        x = np.array([loss[n] for n in names])
        y = np.array([ratings[n][0] for n in names])
        rho = spearman(x, y)
        low, high = spearman_interval(x, y, rng)
        print(f"{label}: Spearman(loss, Elo) = {rho:+.3f}, 95% [{low:+.3f}, {high:+.3f}], {len(names)} nets")

        if plt:
            fig, ax = plt.subplots(figsize=(6, 4))
            ax.errorbar(x, y, yerr=[ratings[n][1] for n in names], fmt="o", capsize=3)
            ax.set_xlabel(f"validation MSE ({label})")
            ax.set_ylabel("Elo vs anchor")
            ax.set_title(f"Spearman ρ = {rho:+.2f}")
            ax.grid(alpha=0.3)
            fig.tight_layout()
            fig.savefig(Path(args.out) / f"loss-vs-elo-{label.replace(' ', '-')}.png", dpi=200)


if __name__ == "__main__":
    main()
