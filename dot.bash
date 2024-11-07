gcc main.c -o main
./main
for f in quicksort_step_*.dot; do
    dot -Tpng "$f" -o "${f%.dot}.png"
done

