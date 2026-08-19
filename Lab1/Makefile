CC=gcc
LIBS= -lm -pthread
.PHONY: all
all: task1 task2 task3

task1: task1.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

task2: task2.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

task3: task3.o slice.o runtask.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -c $^ -o $@
