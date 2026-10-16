# Runbook: що запускати і в якому порядку

Усі команди виконуються з каталогу `research/` у **bash**: цикли нижче написані
в синтаксисі bash, тож у fish спершу набери `bash`.

`gen.sh` і `relabel.sh` продовжують з місця збою. Готові шматки (~5 хв роботи
одного процесу) лежать у `data/<name>.parts/`, і та сама команда продовжує
з них, а готовий датасет (`data/<name>.done`) пропускається. Тож після збою чи
Ctrl-C достатньо запустити ту саму команду або весь ланцюжок ще раз: втрачається
не більше кількох хвилин, а результат побайтово такий самий, як без переривання.
`train.sh` не перезаписує готові чекпоінти.

## Звідки оцінки часу

Виміряно 2026-10-11 на Core 5 210H (4P+4E, 12 потоків), RTX 5060 Laptop.
Пілот без навантаження у фоні генерував ~2000 позицій/с, з браузером було ~1200:

| що | швидкість | правило |
|---|---|---|
| `gen.sh` / `relabel.sh`, 12 процесів | ~2000 позицій/с на 5k вузлів (пілот) | **години ≈ N × B / 3.5·10¹⁰** |
| навчання 512, GPU | ~5.5M позицій/с | 655M позицій (40 суперпакетів) ≈ 2–3 хв |
| матч на 20k вузлів, 12 партій паралельно | ~9 партій/с | 4000 партій ≈ 8 хв |

Через ліміт вузлів фонове навантаження змінює лише тривалість, а не дані чи
результати матчів. Ноутбук під тривалим навантаженням тротлить, тож до часу
генерації варто додавати ~10–20%.

## 0. Одноразова підготовка (вже зроблено в цій сесії)

```bash
# вчитель для всіх датасетів; більше його не перезбирати
cp ../build/release/AteNika builds/teacher
# bullet-utils з того самого коміту bullet, що й тренер (convert у bulletformat)
B=$HOME/.cargo/git/checkouts/bullet-8a69ed9a26c6f599/629ee50
cargo build --offline --release --manifest-path $B/crates/utils/Cargo.toml --target-dir /tmp/bullet-utils
mkdir -p tools && cp /tmp/bullet-utils/release/bullet-utils tools/
# рушій шириною 512, в який матчі підставляють мережі через EvalFile
git show 80668a8:nets/atenika.nnue > data/v060.nnue
./build_net.sh data/v060.nnue 512 h512
```

Для графіків потрібен matplotlib: `sudo pacman -S python-matplotlib`.

## 1. Пілот, ~10 хв

```bash
./gen.sh 5000 200000 900 pilot                          # ~3 хв, дивись pos/s у data/pilot.log
SUPERBATCHES=10 ./train.sh pilot data/pilot.bin         # ~1 хв
./match.sh ../trainer/checkpoints/pilot-10/quantised.bin data/v060.nnue 1000   # ~2 хв
```

Що перевірити:
- у `data/pilot.log` усі процеси дійшли до кінця, `pos/s` у сумі ≈ 1000+;
- loss у `results/train-pilot.log` падає;
- пілотна мережа помітно програє v060, і це нормально: 200k позицій проти сотень
  мільйонів, на яких навчена v060. Сотні Elo мінуса очікувані. Тривожний знак —
  майже нуль очок (~−700 і гірше), як у мережі на 50k позицій після 200 кроків.
  Експериментальні мережі теж будуть слабші за v060; їх порівнюють між собою
  через якір, а не з v060.

**Результат пілоту (2026-10-11).** Генерація ~2000 позицій/с. Мережа програла v060
−559 Elo через перенавчання: 819 проходів по 200k позицій дали loss 0.000165 на
навчальних позиціях і 0.0284 на нових (у 170 разів більше). Звідси рання зупинка
на кроці 3.

**Бюджет C = 4·10¹⁰**, генерація ≈ 1.1 год на точку, ~6 год на весь Exp2.

## 2. Генерація (ніч, ~6.5–7 год)

Усе одним ланцюжком у фоні. Ноутбук має бути на зарядці, а сон вимкнений: під час
сну генерація просто стоїть. Прогрес видно через `tail -f data/night.log`. Якщо
щось обірвалося, запусти цю саму команду ще раз.

```bash
nohup bash -c '
./gen.sh  2500 16000000 1 e2-n2500 &&
./gen.sh  5000  8000000 2 e2-n5000 &&
./gen.sh 10000  4000000 3 e2-n10000 &&
./gen.sh 20000  2000000 4 e2-n20000 &&
./gen.sh 40000  1000000 5 e2-n40000 &&
./relabel.sh  2500 data/e2-n40000.txt e1-n2500 &&
./relabel.sh 10000 data/e2-n40000.txt e1-n10000 &&
./relabel.sh 40000 data/e2-n40000.txt e1-n40000 &&
./gen.sh 5000 30000 99 ref-pos &&
./relabel.sh 500000 data/ref-pos.txt ref &&
for b in 2500 5000 10000 20000 40000; do ./gen.sh $b 30000 $((50 + b / 2500)) val-n$b || exit 1; done
' >> data/night.log 2>&1 &
```

Що робить кожна частина ланцюжка:

| команди | для чого | час |
|---|---|---|
| `gen.sh … e2-n*` | **Exp2**: однаковий загальний бюджет N × B = 4·10¹⁰, від 16M позицій по 2.5k вузлів до 1M по 40k. Сіди 1–5 різні, щоб датасети не ділили партій | ~6 год |
| `relabel.sh … e1-n*` | **Exp1**: ті самі 1M позицій з `e2-n40000`, переоцінені з порожньої таблиці на 2.5k, 10k і 40k вузлів | ~1.5 год |
| `ref-pos`, `ref` | спільний еталонний набір для loss: 30k нових позицій, оцінених на 500k вузлів | ~25 хв |
| `val-n*` | свіжі позиції на кожному бюджеті: рання зупинка (крок 3) і loss «на власних мітках» | ~5 хв |

## 3. Навчання і рання зупинка, ~30 хв

**Глибина, яку дали бюджети (з `data/*.log`).** Self-play (Exp2): 2.5k → 5.1 півходу,
5k → 6.3, 10k → 7.6, 20k → 9.0, 40k → 10.4; еталон 500k → 14.2. Переоцінка (Exp1)
шукає з порожньої таблиці транспозицій, тож на тих самих вузлах мілкіша: 2.5k → 4.3,
10k → 6.3. Тому всі три точки Exp1, включно з 40k, переоцінені однаково, а Exp1 з
Exp2 порівнюються за глибиною, а не за вузлами.

Усі мережі навчаються однакові 40 суперпакетів, чекпоінт зберігається кожні 2.
Мережу, що представлятиме бюджет, вибирає `pick.sh`: це чекпоінт з найменшим
loss на свіжих позиціях з мітками того самого бюджету (`val-n*`). Без цього
мережі на 1–2M позицій перенавчаються, як у пілоті, і порівняння міряло б
тривалість навчання, а не дані.

```bash
export SAVE_RATE=2
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
```

## 4. Матчі, ~1.5 год

Якір — вибраний чекпоінт `e2-n10000`, середина діапазону.

```bash
CK=../trainer/checkpoints
A=$(grep '^e2-n10000-' results/picked.txt)

# вибрані мережі, по 4000 партій
for n in $(grep -vx "$A" results/picked.txt); do
    ./match.sh $CK/$n/quantised.bin $CK/$A/quantised.bin 4000
done

# інші чекпоінти для H3, по 1000 партій: від недонавченої мережі до перенавченої
for n in e2-n2500 e2-n5000 e2-n10000 e2-n20000 e2-n40000 e1-n2500 e1-n10000; do
    for s in 4 10 20 40; do
        grep -qx "$n-$s" results/picked.txt && continue
        ./match.sh $CK/$n-$s/quantised.bin $CK/$A/quantised.bin 1000
    done
done

./rate.sh $A results/*-vs-$A-*.pgn
```

`rate.sh` друкує таблицю і пише `results/rate-<час>.csv`; цей CSV потрібен на кроці 5.

## 5. Втрати й графіки, ~5 хв

```bash
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
```

Скрипт виводить Elo для кожного бюджету, оцінку оптимуму B* з 95% інтервалом і
кореляції Спірмена між loss та Elo, а графіки зберігає в `figs/`.

## 6. Підтвердження через SPRT

Найкраща мережа проти другої за силою. Для `sprt.sh` потрібні окремі бінарники:

```bash
./build_net.sh ../trainer/checkpoints/<best>/quantised.bin 512 best
./build_net.sh ../trainer/checkpoints/<second>/quantised.bin 512 second
# з лімітом вузлів усі 12 потоків чесні, sprt.sh за замовчуванням бере лише 6
cd ../matches && NODES=20000 CONCURRENCY=12 ./sprt.sh ../research/builds/best ../research/builds/second gainer
```

Тривалість залежить від різниці в силі: при 20+ Elo кілька хвилин, при 0–5 Elo до
~1 год, межа `sprt.sh` — 60 тис. партій (~2 год). У звіт іде рядок з рішенням SPRT (H0 чи H1). Оцінку Elo з SPRT не використовуй:
вона зміщена через правило зупинки.
