#!/bin/bash

export SAVE_RATE=1 SUPERBATCHES=100 BATCHES_PER_SUPERBATCH=250
for b in 2500 5000 10000 20000 40000; do ./train.sh e2-n$b data/e2-n$b.bin; done
./train.sh e1-n2500  data/e1-n2500.bin
./train.sh e1-n10000 data/e1-n10000.bin
./train.sh e1-n40000 data/e1-n40000.bin
./train.sh s2-n40000 data/e2-n40000.bin    # ті самі дані, що й e2-n40000: другий прогін показує шум навчання

for n in e2-n2500 e2-n5000 e2-n10000 e2-n20000 e2-n40000 e1-n2500 e1-n10000 e1-n40000 s2-n40000; do
    b=${n#*-n}
    ./pick.sh $n data/val-n$b.txt
done
cat results/picked.txt