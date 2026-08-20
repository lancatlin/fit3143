#!/bin/bash

# Benchmark Task 1-3 and show the value for each n
N=10000000

for n in {1..16}; do
  echo "$n"
  NUM_THREADS=$n ./task2 $N
  cp logs/task2-$N.log logs/task2-$N-$n.log
  sleep 1
done
