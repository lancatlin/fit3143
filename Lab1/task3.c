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

IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    // list of IntSlice to store primes from each thread
    IntSlice *local_result = NULL;
    // A shared var that will be updated in parallel region
    int THREAD_COUNT = 1;

    // Start parallelizing
#pragma omp parallel
    {
        // Use single block to initialise slices for each thread
#pragma omp single
        {
            THREAD_COUNT = omp_get_num_threads();
            local_result = malloc(sizeof(IntSlice) * THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; i++) {
                local_result[i] = make_slice(0, 10);
            }
        }
        int thread = omp_get_thread_num();

        // Separate the loop into different chunks and run independently
#pragma omp for
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
                append_slice(&local_result[thread], i);
            }
        }
    }

    // Count the total size of the result
    int count = 0;
    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        count += local_result[thread].len;
    }
    // Merge the result from each thread into one slice
    IntSlice primes = make_slice(0, count);
    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        for (int i = 0; i < local_result[thread].len; i++) {
            append_slice(&primes, local_result[thread].arr[i]);
        }
        free_slice(&local_result[thread]);
    }
    free(local_result);
    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task3");
}
