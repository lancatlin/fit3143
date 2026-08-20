#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

// find all primes up to but not including n
IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    IntSlice *temp = NULL;
    int THREAD_COUNT = 1;
#pragma omp parallel
    {
#pragma omp single
        {
            THREAD_COUNT = omp_get_num_threads();
            temp = malloc(sizeof(IntSlice) * THREAD_COUNT);
            for (int i = 0; i < THREAD_COUNT; i++) {
                temp[i] = make_slice(0, 10);
            }
        }
        int thread = omp_get_thread_num();
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
                append_slice(&temp[thread], i);
            }
        }
    }

    int count = 0;
    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        count += temp[thread].len;
    }
    // merging results
    IntSlice primes = make_slice(0, count);
    for (int thread = 0; thread < THREAD_COUNT; thread++) {
        for (int i = 0; i < temp[thread].len; i++) {
            append_slice(&primes, temp[thread].arr[i]);
        }
        free_slice(&temp[thread]);
    }
    free(temp);
    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task3");
}
