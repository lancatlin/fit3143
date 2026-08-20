// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
// Compile by running `make`
// Execute by `./task2`
// Set number of threads by `NUM_THREADS=8 ./task2`
#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MIN(a, b) (a < b ? a : b)

typedef struct {
    int startNumber; // Range includes start number,
    int endNumber;   // But not end number
    IntSlice primes; // Output parameter for result
} ThreadParameters;

typedef struct {
    int next; // the start number for the next thread
    int step; // the size of numbers to be checked by each thread
    int n;    // the end number to which main is finding all primes from 1
} InitializerState;

int NUM_THREADS = 1;

// Assign each thread with start and end numbers along with a data structure
// (IntSlice) to store result
void initialize_para(InitializerState *states, ThreadParameters *tPara) {
    tPara->startNumber = states->next;
    states->next = MIN(states->next + states->step, states->n);
    tPara->endNumber = states->next;
    tPara->primes = make_slice(0, 10);
}

// Free data structure (IntSlice) in thread parameters struct
void free_para(ThreadParameters *tPara) { free_slice(&(tPara->primes)); }

// find all primes between [start, end) and store the results in an output
// parameter
void *find_primes_from(void *tPara) {
    // Getting start and end numbers
    ThreadParameters *tP = (ThreadParameters *)tPara;
    int start = tP->startNumber;
    int end = tP->endNumber;
    IntSlice *pPrimes = &(tP->primes);

    // Edge case for prime number 2
    if (start < 2 && end > 2) {
        append_slice(pPrimes, 2);
        start = 3;
    }
    // Prime Finding Algorithm
    for (int i = start; i < end; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 2; j <= sqroot; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        if (is_prime) {
            append_slice(pPrimes, i);
        }
    }
    return NULL;
}

IntSlice find_primes(int n) {
    // POSIX threads parallelization
    pthread_t *tid = malloc(sizeof(pthread_t) * NUM_THREADS);
    ThreadParameters *tArg = malloc(sizeof(ThreadParameters) * NUM_THREADS);

    InitializerState states;
    states.next = 0;
    states.step = ceil((float)n / NUM_THREADS);
    states.n = n;

    for (int i = 0; i < NUM_THREADS; i++) {
        initialize_para(&states, &tArg[i]);
        pthread_create(&tid[i], NULL, &find_primes_from, &tArg[i]);
    }

    IntSlice result = make_slice(0, 10);

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);

        IntSlice p = tArg[i].primes;
        for (int j = 0; j < p.len; j++) {
            append_slice(&result, p.arr[j]);
        }

        free_para(&tArg[i]);
    }
    free(tid);
    free(tArg);
    return result;
}

int main(int argc, char *argv[]) {
    if (getenv("NUM_THREADS") != NULL) {
        NUM_THREADS = atoi(getenv("NUM_THREADS"));
    }
    printf("Threads: %d\n", NUM_THREADS);

    return run_task(argc, argv, find_primes, "task2");
}
