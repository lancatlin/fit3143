# Justin Lin       35808217    hlin0094@student.monash.edu
# Clement Angelo   35664614    cang0028@student.monash.edu
CC=mpicc
LIBS= -lm -lpthread
.PHONY: all
all: main test

.PHONY: run
run: all
	mpirun -np 4 ./task1

.PHONY: run
run-test: test
	./test

main: main.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

test: test.o charging_node.o charging_port.o random_generator.o
	$(CC) -fopenmp $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) -fopenmp -g -c $^ -o $@
