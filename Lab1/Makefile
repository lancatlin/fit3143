# Justin Lin       35808217    hlin0094@student.monash.edu
# Clement Angelo   35664614    cang0028@student.monash.edu
CC=gcc
LIBS= -lm -pthread
.PHONY: all
all: task1 task2 task3 task4 task5 task6 miller_rabin

task1: task1.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

task2: task2.o slice.o runtask.o
	$(CC) -g $^ -o $@ $(LIBS)

task3: task3.o slice.o runtask.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

task4: task4.o slice.o runtask.o
	$(CC) -fopenmp -fdump-tree-optimized $^ -o $@ $(LIBS)

task5: task5.o slice.o runtask.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

task6: task6.o slice.o runtask.o
	$(CC) -fopenmp -g $^ -o $@ $(LIBS)

miller_rabin: miller_rabin.o slice.o runtask.o
	$(CC) -fopenmp -g $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -fdump-tree-optimized -g -c $^ -o $@
