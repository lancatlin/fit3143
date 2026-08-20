#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdlib.h>

// Number of parallel threads 
#define NUM_THREADS 16
// Helper function to return the min of two numbers
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
void *thread_func(void *tPara) {
    // Fetching function parameters
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
        // Append prime to result
        if (is_prime) {
            append_slice(pPrimes, i);
        }
    }
    return NULL;
}

IntSlice find_primes(int n) {
    // POSIX threads parallelization
    pthread_t tid[NUM_THREADS];
    ThreadParameters tArg[NUM_THREADS];
    
    // primes to be returned
    IntSlice result = make_slice(0, 10);
    
    // States to initialize start and end range for threads
    InitializerState states;
    states.next = 0;
    states.step = ceil(n / NUM_THREADS);
    states.n = n;

    // Thread Creation: Forking
    for (int i = 0; i < NUM_THREADS; i++) {
        initialize_para(&states, &tArg[i]);
        pthread_create(&tid[i], NULL, &thread_func, &tArg[i]);
    }

    // Thread Elimination: Joining
    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(tid[i], NULL);
        // Collating primes to result (IntSlice)
        IntSlice p = tArg[i].primes;
        for (int j = 0; j < p.len; j++) {
            append_slice(&result, p.arr[j]);
        }
        // Free ThreadParameter struct
        free_para(&tArg[i]);
    }
    // Output
    return result;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "task2");
}
