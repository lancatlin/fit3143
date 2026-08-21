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
#include <stdlib.h>

int check_primes(int n, IntSlice *result) {
    int count = 0;
    for (int i = 2; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 2; j <= sqroot; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            // Do not require critical region because it is saved locally
            result->arr[i] = true;
            count++;
        }
    }
    return count;
}

IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    IntSlice is_primes = make_slice(n, n);

    int count = check_primes(n, &is_primes);

    // Merge the result from each thread into one slice
    IntSlice primes = make_slice(0, count);
    for (int i = 0; i < n; i++) {
        if (is_primes.arr[i]) {
            append_slice(&primes, i);
        }
    }
    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task4");
}
