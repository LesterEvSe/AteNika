# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Yevhenii Sekhin
#
# Need to test, that .nnue works correct.
# Compare against the engine, which must be rebuilt after the net is replaced:
#
#   cmake --build --preset release
#   printf 'position fen <FEN>\neval\nquit\n' | ./build/release/AteNika
#   python3 nets/verify_net.py nets/placeholder.nnue "<FEN>"

import struct
import sys

INPUT, HIDDEN = 768, 512
QA, QB, SCALE = 255, 64, 400

PIECES = "pnbrqk"

CHECKS = [
    ("start", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
    ("black loses a pawn", "rnbqkbnr/1ppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
    ("black loses a knight", "r1bqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
    ("black loses a rook", "1nbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
    ("black loses a queen", "rnb1kbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1"),
    ("white loses a queen", "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNB1KBNR w KQkq - 0 1"),
    ("KQ vs K, white to move", "4k3/8/8/8/8/8/8/3QK3 w - - 0 1"),
    ("KQ vs K, black to move", "4k3/8/8/8/8/8/8/3QK3 b - - 0 1"),
]


def load(path):
    """Split the quantised export into its four arrays.

    bullet pads the file to a multiple of 64 with the repeated ASCII string
    "bullet" rather than zeros, so a trailing remainder is expected.
    """
    blob = open(path, "rb").read()
    expected = INPUT * HIDDEN * 2 + HIDDEN * 2 + 2 * HIDDEN * 2 + 2
    if len(blob) < expected:
        raise SystemExit(f"{path}: {len(blob)} bytes, need at least {expected}")

    offset = 0

    def take(count):
        nonlocal offset
        values = struct.unpack_from(f"<{count}h", blob, offset)
        offset += count * 2
        return values

    return take(INPUT * HIDDEN), take(HIDDEN), take(2 * HIDDEN), take(1)[0]


def parse(fen):
    """FEN -> ([(piece, is_black, square)], side_to_move), square 0 = a1."""
    placement, side = fen.split()[0], fen.split()[1]
    pieces = []
    square = 56
    for symbol in placement:
        if symbol == "/":
            square -= 16
        elif symbol.isdigit():
            square += int(symbol)
        else:
            pieces.append((PIECES.index(symbol.lower()), int(symbol.islower()), square))
            square += 1
    return pieces, int(side == "b")


def accumulate(pieces, perspective, feature_weights, feature_biases):
    values = list(feature_biases)
    for piece, is_black, square in pieces:
        relative_color = 0 if is_black == perspective else 1
        relative_square = square if perspective == 0 else square ^ 56
        index = relative_color * 384 + piece * 64 + relative_square
        column = feature_weights[index * HIDDEN : (index + 1) * HIDDEN]
        for i in range(HIDDEN):
            values[i] += column[i]
    return values


def evaluate(net, fen):
    feature_weights, feature_biases, output_weights, output_bias = net
    pieces, side_to_move = parse(fen)

    us = accumulate(pieces, side_to_move, feature_weights, feature_biases)
    them = accumulate(pieces, 1 - side_to_move, feature_weights, feature_biases)

    # SCReLU: squaring leaves the sum in QA*QA*QB, so one QA comes off before
    # the bias, which is stored in QA*QB.
    total = 0
    for i in range(HIDDEN):
        a = min(max(us[i], 0), QA)
        b = min(max(them[i], 0), QA)
        total += a * a * output_weights[i] + b * b * output_weights[HIDDEN + i]

    total //= QA
    total += output_bias
    return total * SCALE // (QA * QB)


def main() -> int:
    if len(sys.argv) < 2:
        raise SystemExit("usage: verify_net.py <net.nnue> [fen]")

    net = load(sys.argv[1])

    if len(sys.argv) > 2:
        print(evaluate(net, sys.argv[2]))
        return 0

    for name, fen in CHECKS:
        print(f"  {name:24s} {evaluate(net, fen):7d} cp")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
