#!/bin/bash


DIR=logs

N=(1000 10000 100000 1000000 10000000 50000000 100000000 200000000)


echo "N,Task 1,Task 2,Task 3"
for n in "${N[@]}"; do
  echo -n "$n"
  for p in {1..3}; do
    file="$DIR/task$p-$n.log"
    t=$(grep "^Compute time:" "$file" | cut -d':' -f2 | xargs)
    echo -n ",$t"
  done
  echo
done
