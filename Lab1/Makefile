CC=gcc
LIBS= -lm -pthread
.PHONY: all
all: task1 task1-slice task3

task1: task1.o
	$(CC) $^ -o $@ $(LIBS)

task1-slice: task1-slice.o slice.o runtask.o
	$(CC) $^ -o $@ $(LIBS)

task3: task3.o slice.o runtask.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -c $^ -o $@

run1: task1
	@./task1
