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
#define SERIAL_LIMIT 1000

void check_primes_serial(int n, IntSlice *is_primes, IntSlice *primes) {
    printf("Serial: %d\n", n);
    for (int i = 2; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 0; j < primes->len && primes->arr[j] <= sqroot; j++) {
            if (i % primes->arr[j] == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            // Do not require critical region because it is saved locally
            is_primes->arr[i] = true;
            append_slice(primes, i);
        }
    }
}

void check_primes(int n, IntSlice *is_primes, IntSlice *primes) {
    printf("Check N: %d\n", n);
    int start = ceil(sqrt(n));
    if (n <= SERIAL_LIMIT) {
        return check_primes_serial(n, is_primes, primes);
    } else {
        check_primes(start, is_primes, primes);
    }
#pragma omp parallel for schedule(guided)
    for (int i = start; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 0; j < primes->len && primes->arr[j] <= sqroot; j++) {
            if (i % primes->arr[j] == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            // Do not require critical region because it is saved locally
            is_primes->arr[i] = true;
        }
    }

    printf("Finished N: %d\n", n);
    for (int i = start; i < n; i++) {
        if (is_primes->arr[i]) {
            append_slice(primes, i);
        }
    }
}

IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    IntSlice is_primes = make_slice(n, n);

    IntSlice primes = make_slice(0, 100);
    check_primes(n, &is_primes, &primes);

    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task4");
}
