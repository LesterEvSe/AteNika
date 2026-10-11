# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Yevhenii Sekhin
#
# Validation loss of trained nets on a datagen text file, computed here rather
# than in bullet, which does not implement a test set.
#
#   python3 eval_loss.py <positions.txt> <raw.bin>...
#   python3 eval_loss.py ../data/ref.txt ../../trainer/checkpoints/b5k-*/raw.bin > ref-loss.csv
#
# One CSV row per net:
#   mse_score   against sigmoid(score / 400), the file's own labels
#   mse_result  against the game result
#   sign_agree  share of positions where net and label agree on who is better
#
# raw.bin is bullet's unquantised export: l0w [768][hidden], l0b, l1w [2*hidden],
# l1b, all f32. The net output o is a logit, o * 400 is centipawns.

import sys

import numpy as np

INPUT = 768
SCALE = 400.0
PIECES = "pnbrqk"
MAX_PIECES = 32
CHUNK = 2048


def load_net(path):
    weights = np.fromfile(path, dtype="<f4")
    hidden = (weights.size - 1) // (INPUT + 3)
    if weights.size != INPUT * hidden + 3 * hidden + 1:
        raise SystemExit(f"{path}: {weights.size} floats is no (768 -> H)x2 -> 1 net")

    l0w = weights[: INPUT * hidden].reshape(INPUT, hidden)
    rest = weights[INPUT * hidden :]
    l0b, l1w, l1b = rest[:hidden], rest[hidden : 3 * hidden], rest[3 * hidden]

    # One zero row past the end, for padding the feature lists.
    l0w = np.vstack([l0w, np.zeros((1, hidden), dtype=np.float32)])
    return l0w, l0b, l1w, float(l1b)


def features(placement, perspective):
    """Feature indices from one side's view, as nnue.cpp's feature_index."""
    out = []
    square = 56
    for symbol in placement:
        if symbol == "/":
            square -= 16
        elif symbol.isdigit():
            square += int(symbol)
        else:
            colour = int(symbol.islower())
            relative_colour = 0 if colour == perspective else 1
            relative_square = square if perspective == 0 else square ^ 56
            out.append(relative_colour * 384 + PIECES.index(symbol.lower()) * 64 + relative_square)
            square += 1
    return out + [INPUT] * (MAX_PIECES - len(out))


def load_positions(path):
    """Feature lists from the side to move's view, labels turned to match."""
    ours, theirs, scores, results = [], [], [], []
    with open(path) as lines:
        for line in lines:
            fen, score, result = (part.strip() for part in line.split("|"))
            placement, side = fen.split()[:2]
            stm = int(side == "b")

            ours.append(features(placement, stm))
            theirs.append(features(placement, 1 - stm))

            score, result = float(score), float(result)
            scores.append(-score if stm else score)
            results.append(1.0 - result if stm else result)

    return (np.array(ours, dtype=np.int32), np.array(theirs, dtype=np.int32),
            np.array(scores, dtype=np.float64), np.array(results, dtype=np.float64))


def forward(net, ours, theirs):
    l0w, l0b, l1w, l1b = net
    hidden = l0b.size
    out = np.empty(len(ours), dtype=np.float64)

    for start in range(0, len(ours), CHUNK):
        us = l0w[ours[start : start + CHUNK]].sum(axis=1) + l0b
        them = l0w[theirs[start : start + CHUNK]].sum(axis=1) + l0b
        us = np.clip(us, 0.0, 1.0) ** 2
        them = np.clip(them, 0.0, 1.0) ** 2
        out[start : start + CHUNK] = us @ l1w[:hidden] + them @ l1w[hidden:] + l1b

    return out


def sigmoid(x):
    return 1.0 / (1.0 + np.exp(-x))


def main():
    if len(sys.argv) < 3:
        raise SystemExit("usage: eval_loss.py <positions.txt> <raw.bin>...")

    ours, theirs, scores, results = load_positions(sys.argv[1])
    target = sigmoid(scores / SCALE)

    print("net,positions,mse_score,mse_result,sign_agree")
    for path in sys.argv[2:]:
        logits = forward(load_net(path), ours, theirs)
        predicted = sigmoid(logits)

        mse_score = np.mean((predicted - target) ** 2)
        mse_result = np.mean((predicted - results) ** 2)
        sign_agree = np.mean(np.sign(logits) == np.sign(scores))
        print(f"{path},{len(scores)},{mse_score:.6f},{mse_result:.6f},{sign_agree:.4f}")


if __name__ == "__main__":
    main()
