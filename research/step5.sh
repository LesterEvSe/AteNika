#!/bin/bash

CK=../trainer/checkpoints
python3 analysis/eval_loss.py data/ref.txt $CK/e[12]-n*/raw.bin > results/loss-ref.csv

# кожна мережа на свіжих позиціях з мітками її власного бюджету
echo "net,positions,mse_score,mse_result,sign_agree" > results/loss-own.csv
for d in $CK/e[12]-n*; do
    b=$(basename $d | sed -E 's/^e[12]-n([0-9]+)-.*/\1/')
    python3 analysis/eval_loss.py data/val-n$b.txt $d/raw.bin | tail -n 1 >> results/loss-own.csv
done

R=$(ls -t results/rate-*.csv | head -1)
python3 analysis/plots.py --ratings $R --loss results/loss-ref.csv --own results/loss-own.csv \
    --exp e2 --final results/picked.txt --out figs
python3 analysis/plots.py --ratings $R --loss results/loss-ref.csv --exp e1 --final results/picked.txt --out figs
