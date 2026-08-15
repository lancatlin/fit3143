CC=gcc
.PHONY: all
all: task1 task1-slice task3

task1: task1.o
	gcc $^ -o $@ -lm

task1-slice: task1-slice.o slice.o runtask.o
	gcc $^ -o $@ -lm

task3: task3.o slice.o runtask.o
	gcc -fopenmp $^ -o $@ -lm

task3.o: task3.c
	gcc -fopenmp -c $^ -o $@ -lm

%.o: %.c
	gcc -c $^ -o $@

run1: task1
	@./task1
