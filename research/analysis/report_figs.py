# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Yevhenii Sekhin
#
# The result figures of the report, from the files the pipeline leaves.
#
#   python3 analysis/report_figs.py results/rate-<stamp>.csv
#
# Writes figs/elo-by-depth.png (both experiments against mean search depth),
# figs/loss-vs-elo.png (reference and own-label loss against Elo) and
# figs/training-curves.png (reference loss during training, fixed budget).

import csv
import re
import sys
from pathlib import Path

import matplotlib

matplotlib.use("Agg")
import matplotlib.pyplot as plt
import numpy as np

from plots import spearman

plt.rcParams["font.family"] = "Liberation Serif"
plt.rcParams["mathtext.fontset"] = "stix"

BLUE, ORANGE = "#2a78d6", "#eb6834"
INK, MUTED, GRID = "#1a1a1a", "#6b6b6b", "#e4e4e4"
BUDGETS = (2500, 5000, 10000, 20000, 40000)


def ratings(path):
    with open(path) as file:
        return {row["PLAYER"]: (float(row["RATING"]), 0.0 if row["ERROR"] == "-" else float(row["ERROR"]))
                for row in csv.DictReader(file)}


def losses(path):
    with open(path) as file:
        return {Path(row["net"]).parent.name: float(row["mse_score"]) for row in csv.DictReader(file)}


def mean_depth(name):
    depths = [float(m) for m in re.findall(r"depth ([0-9.]+)", Path(f"data/{name}.log").read_text())]
    return sum(depths) / len(depths)


def style(ax):
    ax.grid(color=GRID, linewidth=0.8)
    ax.set_axisbelow(True)
    ax.tick_params(colors=MUTED, labelsize=9, length=3)
    for side in ("top", "right"):
        ax.spines[side].set_visible(False)
    for side in ("left", "bottom"):
        ax.spines[side].set_color("#9a9a9a")


def elo_by_depth(rate, picked):
    fig, ax = plt.subplots(figsize=(6.4, 3.6))
    style(ax)

    for prefix, budgets, colour, marker, label in (
        ("e2", BUDGETS, BLUE, "o", "fixed budget, N·B = const"),
        ("e1", (2500, 10000, 40000), ORANGE, "s", "same 1M positions, relabelled"),
    ):
        names = [next(p for p in picked if p.startswith(f"{prefix}-n{b}-")) for b in budgets]
        depth = [mean_depth(f"{prefix}-n{b}") for b in budgets]
        elo = [rate[n][0] for n in names]
        err = [rate[n][1] for n in names]
        ax.errorbar(depth, elo, yerr=err, color=colour, marker=marker, markersize=6, linewidth=2,
                    capsize=3, label=label)
        for d, e, b in zip(depth, elo, budgets):
            ax.annotate(f"{b // 1000 if b >= 10000 else b / 1000:g}k", (d, e), textcoords="offset points",
                        xytext=(7, 4), fontsize=9, color=MUTED)

    ax.set_xlabel("mean search depth of labels, plies", fontsize=10, color=INK)
    ax.set_ylabel("Elo vs anchor", fontsize=10, color=INK)
    ax.legend(frameon=False, fontsize=9.5, loc="upper right")
    fig.tight_layout()
    fig.savefig("figs/elo-by-depth.png", dpi=300, facecolor="white")


def loss_vs_elo(rate):
    fig, axes = plt.subplots(1, 2, figsize=(7.2, 3.2), sharey=True)
    for ax, path, title in ((axes[0], "results/loss-ref.csv", "reference set"),
                            (axes[1], "results/loss-own.csv", "own-budget labels")):
        style(ax)
        loss = losses(path)
        names = sorted(set(loss) & set(rate))
        x = np.array([loss[n] for n in names])
        y = np.array([rate[n][0] for n in names])
        ax.errorbar(x, y, yerr=[rate[n][1] for n in names], fmt="o", color=BLUE, markersize=4.5,
                    elinewidth=1, capsize=2)
        rho = f"{spearman(x, y):+.2f}".replace("-", "−")
        ax.set_title(f"{title}: Spearman ρ = {rho}", fontsize=10.5, color=INK)
        ax.set_xlabel("validation MSE", fontsize=10, color=INK)
    axes[0].set_ylabel("Elo vs anchor", fontsize=10, color=INK)
    fig.tight_layout()
    fig.savefig("figs/loss-vs-elo.png", dpi=300, facecolor="white")


def training_curves(picked):
    loss = losses("results/loss-ref.csv")
    ramp = ("#86b6ef", "#5598e7", "#2a78d6", "#1c5cab", "#104281")
    examples = 250 * 16384 / 1e6

    fig, ax = plt.subplots(figsize=(6.4, 3.4))
    style(ax)
    for budget, colour in zip(reversed(BUDGETS), ramp):
        positions = 4e10 / budget / 1e6
        superbatches = sorted(int(n.rsplit("-", 1)[1]) for n in loss if n.startswith(f"e2-n{budget}-"))
        x = [s * examples for s in superbatches]
        y = [loss[f"e2-n{budget}-{s}"] for s in superbatches]
        ax.plot(x, y, color=colour, linewidth=2, label=f"N = {positions:g}M")
        best = next(p for p in picked if p.startswith(f"e2-n{budget}-"))
        s = int(best.rsplit("-", 1)[1])
        ax.plot([s * examples], [loss[best]], marker="o", markersize=7, color=colour,
                markeredgecolor="white", markeredgewidth=1.2, zorder=3)

    ax.set_xlabel("training examples, millions", fontsize=10, color=INK)
    ax.set_ylabel("MSE on reference set", fontsize=10, color=INK)
    ax.legend(frameon=False, fontsize=9.5, loc="upper left", ncol=5, handlelength=1.5,
              columnspacing=1.2, bbox_to_anchor=(0, 1.13))
    fig.tight_layout()
    fig.savefig("figs/training-curves.png", dpi=300, facecolor="white")


def main():
    if len(sys.argv) != 2:
        raise SystemExit("usage: report_figs.py <rate.csv>")
    rate = ratings(sys.argv[1])
    picked = Path("results/picked.txt").read_text().split()
    Path("figs").mkdir(exist_ok=True)
    elo_by_depth(rate, picked)
    loss_vs_elo(rate)
    training_curves(picked)
    print("figs/elo-by-depth.png figs/loss-vs-elo.png figs/training-curves.png")


if __name__ == "__main__":
    main()
