#!/bin/bash

# Benchmark Task 1-3 and show the value for each n
N=10000000

for n in {1..16}; do
  echo "$n"
  OMP_NUM_THREADS=$n ./task3 $N
  cp logs/task3-$N.log logs/task3-$N-$n.log
  sleep 1
done
