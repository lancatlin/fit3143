task1: task1.o
	gcc $^ -o $@ -lm

task1-slice: task1-slice.o slice.o
	gcc $^ -o $@ -lm

%.o: %.c
	gcc -c $^ -o $@

run1: task1
	@./task1
