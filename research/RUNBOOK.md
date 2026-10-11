# Runbook: що запускати і в якому порядку

Усі команди виконуються з каталогу `research/` у **bash**: цикли нижче написані
в синтаксисі bash, тож у fish спершу набери `bash`. Будь-який крок можна
перервати Ctrl-C і перезапустити: скрипти відмовляються перезаписувати готові
дані та чекпоінти.

## Звідки оцінки часу

Виміряно 2026-10-11 на Core 5 210H (4P+4E, 12 потоків), RTX 5060 Laptop, з
браузером у фоні:

| що | швидкість | правило |
|---|---|---|
| `gen.sh` / `relabel.sh`, 12 процесів | ~1200 позицій/с на 5k вузлів | **години ≈ N × B / 2·10¹⁰** |
| навчання 512, GPU | ~5.5M позицій/с | 655M позицій (40 суперпакетів) ≈ 2–3 хв |
| матч на 20k вузлів, 12 партій паралельно | ~9 партій/с | 4000 партій ≈ 8 хв |

Через ліміт вузлів фонове навантаження змінює лише тривалість, а не дані чи
результати матчів. Ноутбук під тривалим навантаженням тротлить, тож до часу
генерації варто додавати ~10–20%.

## 0. Одноразова підготовка (вже зроблено в цій сесії)

```bash
# вчитель для всіх датасетів; більше його не перезбирати
cp ../build/release/AteNika builds/teacher
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

**Вибір бюджету C.** Якщо сумарна швидкість ≥ 1000 позицій/с, бери C = 4·10¹⁰
(нижче, ~9–10 год генерації). Якщо повільніше або хочеш вкластися в один вечір,
поділи всі N навпіл (C = 2·10¹⁰, ~5 год).

## 2. Генерація (ніч)

```bash
# Exp2: N × B = 4e10. Сіди 1–5 різні, щоб датасети не ділили партій.
./gen.sh  2500 16000000 1 e2-n2500
./gen.sh  5000  8000000 2 e2-n5000
./gen.sh 10000  4000000 3 e2-n10000
./gen.sh 20000  2000000 4 e2-n20000
./gen.sh 40000  1000000 5 e2-n40000
```

Потім, у будь-якому порядку (≈ 1.5–2 год разом):

```bash
# Exp1: ті самі 1M позицій з e2-n40000, переоцінені на менших бюджетах
./relabel.sh  2500 data/e2-n40000.txt e1-n2500
./relabel.sh 10000 data/e2-n40000.txt e1-n10000

# еталонний набір: 30k нових позицій, оцінених на 500k вузлів (~45 хв)
./gen.sh 5000 30000 99 ref-pos
./relabel.sh 500000 data/ref-pos.txt ref

# валідація «на власних мітках»: свіжі позиції на кожному бюджеті
for b in 2500 5000 10000 20000 40000; do ./gen.sh $b 30000 $((50 + b / 2500)) val-n$b; done
```

## 3. Навчання, ~20 хв

```bash
for b in 2500 5000 10000 20000 40000; do ./train.sh e2-n$b data/e2-n$b.bin; done
./train.sh e1-n2500  data/e1-n2500.bin
./train.sh e1-n10000 data/e1-n10000.bin
./train.sh e1-n40000 data/e2-n40000.bin    # ті самі дані, що й e2-n40000: другий прогін показує шум навчання
```

## 4. Матчі, ~1.5 год

Якір — `e2-n10000-40`, середина діапазону.

```bash
CK=../trainer/checkpoints
ANCHOR=$CK/e2-n10000-40/quantised.bin

# фінальні мережі, по 4000 партій
for n in e2-n2500 e2-n5000 e2-n20000 e2-n40000 e1-n2500 e1-n10000 e1-n40000; do
    ./match.sh $CK/$n-40/quantised.bin $ANCHOR 4000
done

# проміжні чекпоінти для H3, по 1000 партій
for n in e2-n2500 e2-n5000 e2-n10000 e2-n20000 e2-n40000 e1-n2500 e1-n10000; do
    for s in 10 20 30; do ./match.sh $CK/$n-$s/quantised.bin $ANCHOR 1000; done
done

./rate.sh e2-n10000-40 results/*-vs-e2-n10000-40-*.pgn
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
python3 analysis/plots.py --ratings $R --loss results/loss-ref.csv --own results/loss-own.csv --exp e2 --out figs
python3 analysis/plots.py --ratings $R --loss results/loss-ref.csv --exp e1 --out figs
```

Скрипт виводить Elo для кожного бюджету, оцінку оптимуму B* з 95% інтервалом і
кореляції Спірмена між loss та Elo, а графіки зберігає в `figs/`.

## 6. Підтвердження через SPRT

Найкраща мережа проти другої за силою. Для `sprt.sh` потрібні окремі бінарники:

```bash
./build_net.sh ../trainer/checkpoints/<best>-40/quantised.bin 512 best
./build_net.sh ../trainer/checkpoints/<second>-40/quantised.bin 512 second
cd ../matches && NODES=20000 ./sprt.sh ../research/builds/best ../research/builds/second gainer
```

У звіт іде рядок з рішенням SPRT (H0 чи H1). Оцінку Elo з SPRT не використовуй:
вона зміщена через правило зупинки.
