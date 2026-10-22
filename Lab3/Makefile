# Justin Lin       35808217    hlin0094@student.monash.edu
# Clement Angelo   35664614    cang0028@student.monash.edu
CC=mpicc
CFLAGS=-g -MMD -MP
LIBS= -lm -lpthread
.PHONY: all
all: main

.PHONY: run
run: all
	mpirun -np 4 ./main

.PHONY: run
run-test: test
	./test

main: main.o charging_node.o charging_port.o random_generator.o my_mpi.o context.o message.o table.o time_helper.o
	$(CC) $^ -o $@ $(LIBS)

test: test.o charging_node.o charging_port.o random_generator.o my_mpi.o context.o message.o table.o time_helper.o
	$(CC) $^ -o $@ $(LIBS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -f *.o *.d main test

-include $(wildcard *.d)
