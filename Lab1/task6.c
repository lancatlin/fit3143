// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
// Compile by running `make`
// Execute by `./task3`
// Set number of threads by `OMP_NUM_THREADS=8 ./task3`
#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#define SERIAL_LIMIT 100

int check_composite_serial(int n, int end, IntSlice *is_composite) {
    printf("N: %d, end: %d\n", n, end);
    int count = 0;
    for (int i = 2; i < end; i++) {
        if (!is_composite->arr[i]) {
            count++;
            for (int j = 2; j * i < n; j++) {
                is_composite->arr[i * j] = true;
            }
        }
    }
    return count;
}

int check_composite(int n, int end, IntSlice *is_composite) {
    printf("Check: %d, %d\n", n, end);
    int count = 0;
    int start = ceil(sqrt(end));
    if (end <= SERIAL_LIMIT) {
        return check_composite_serial(n, end, is_composite);
    } else {
        count += check_composite(n, start, is_composite);
    }
    for (int i = start; i < end; i++) {
        if (!is_composite->arr[i]) {
            count++;
            for (int j = 2; j * i < n; j++) {
                is_composite->arr[i * j] = true;
            }
        }
    }
    return count;
}

IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    IntSlice is_composite = make_slice(n, n);

    int count = check_composite(n, n, &is_composite);

    IntSlice primes = make_slice(0, count);
    // check_primes(n, &is_primes, &primes);
    printf("Finished N: %d\n", n);
    for (int i = 2; i < n; i++) {
        if (!is_composite.arr[i]) {
            append_slice(&primes, i);
        }
    }
    free_slice(&is_composite);

    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task6");
}
