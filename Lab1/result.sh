#!/bin/bash


DIR=logs

n=1000

echo "| N | Task 1 | Task 2 | Task 3 |"
echo "| -- | --- | --- | --- |"
for i in {1..6}; do
  echo -n "| $n |"
  for p in {1..3}; do
    file="$DIR/task$p-$n.log"
    t=$(grep "^Compute time:" "$file" | cut -d':' -f2 | xargs)
    echo -n " $t |"
  done
  echo
  n=$(($n * 10))
done
