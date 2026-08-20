#!/bin/bash

# Benchmark Task 1-3 and show the value for each n

N=(1000 10000 100000 1000000 10000000 50000000 100000000 200000000)

for n in "${N[@]}"; do
  for p in {1..3}; do
    echo "$p: $n"
    ./task$p $n
    sleep 1
  done
done
