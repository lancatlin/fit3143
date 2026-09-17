# Justin Lin       35808217    hlin0094@student.monash.edu
# Clement Angelo   35664614    cang0028@student.monash.edu
CC=mpicc
LIBS= -lm -pthread
.PHONY: all
all: task1 task2 prime_growth

.PHONY: run
run: all
	mpirun -np 4 ./task1

task1: task1.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

task2: task2.o slice.o runtask.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

prime_growth: prime_growth.o slice.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -g -c $^ -o $@
