# SPDX-License-Identifier: GPL-3.0-or-later
# Copyright (C) 2023 Yevhenii Sekhin
#
# Writes a deterministic pseudo-random net, so the inference code can be built and
# tested before a trained one exists.
#
#   int16 feature_weights[768][512]   input-major
#   int16 feature_biases[512]
#   int16 output_weights[1024]        [stm accumulator, then non-stm]
#   int16 output_bias                 already scaled by QA * QB
#   padding to a multiple of 64       bullet aligns the struct to 64
#
#   usage: python3 nets/gen_placeholder.py nets/placeholder.nnue

import struct
import sys

INPUT, HIDDEN = 768, 512
QA, QB = 255, 64


def lcg(seed):
    """Explicit generator: identical bytes on every platform and Python build."""
    state = seed
    while True:
        state = (state * 6364136223846793005 + 1442695040888963407) & 0xFFFFFFFFFFFFFFFF
        yield (state >> 33) & 0xFFFFFFFF


def main() -> int:
    out = sys.argv[1] if len(sys.argv) > 1 else "nets/placeholder.nnue"
    rng = lcg(0x9E3779B97F4A7C15)

    blob = bytearray()
    # Small weights: a random net that saturates every activation would make the
    # tests pass for the wrong reason.
    for _ in range(INPUT * HIDDEN):
        blob += struct.pack("<h", (next(rng) % 121) - 60)
    for _ in range(HIDDEN):
        blob += struct.pack("<h", (next(rng) % 61) - 30)
    for _ in range(2 * HIDDEN):
        blob += struct.pack("<h", (next(rng) % 121) - 60)
    blob += struct.pack("<h", 0)
    blob += b"\x00" * (-len(blob) % 64)

    expected = -(-(INPUT * HIDDEN * 2 + HIDDEN * 2 + 2 * HIDDEN * 2 + 2) // 64) * 64
    assert len(blob) == expected, f"{len(blob)} != {expected}"

    with open(out, "wb") as f:
        f.write(blob)
    print(f"wrote {out}: {len(blob)} bytes ({INPUT} -> {HIDDEN}x2 -> 1, QA={QA} QB={QB})")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
