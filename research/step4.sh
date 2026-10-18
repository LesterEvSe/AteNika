CK=../trainer/checkpoints
A=$(grep '^e2-n10000-' results/picked.txt)

# вибрані мережі, по 4000 партій
for n in $(grep -vx "$A" results/picked.txt); do
    ./match.sh $CK/$n/quantised.bin $CK/$A/quantised.bin 4000
done

# інші чекпоінти для H3, по 1000 партій: від недонавченої мережі до перенавченої
for n in e2-n2500 e2-n5000 e2-n10000 e2-n20000 e2-n40000 e1-n2500 e1-n10000; do
    for s in 10 25 50 100; do
        grep -qx "$n-$s" results/picked.txt && continue
        ./match.sh $CK/$n-$s/quantised.bin $CK/$A/quantised.bin 1000
    done
done

./rate.sh $A results/*-vs-$A-*.pgn