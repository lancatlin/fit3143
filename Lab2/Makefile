# Justin Lin       35808217    hlin0094@student.monash.edu
# Clement Angelo   35664614    cang0028@student.monash.edu
CC=gcc
LIBS= -lm -pthread
.PHONY: all
all: task1

task1: task1.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -fdump-tree-optimized -g -c $^ -o $@
