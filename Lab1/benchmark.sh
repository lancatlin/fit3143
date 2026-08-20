#!/bin/bash

# Benchmark Task 1-3 and show the value for each n

n=1000

for i in {1..6}; do
  for p in {1..3}; do
    echo "$p: $n"
    ./task$p $n
    sleep 1
  done
  n=$(($n * 10))
done
