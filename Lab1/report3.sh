#!/bin/bash


DIR=logs
N=10000000

echo "Threads,Time"
for n in {1..16}; do
  file="$DIR/task3-$N-$n.log"
  t=$(grep "^Compute time:" "$file" | cut -d':' -f2 | xargs)
  echo "$n,$t"
done
