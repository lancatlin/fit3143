#include "slice.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <pthread.h>

#define NUM_THREADS 2
#define MIN(a, b) (a < b ? a : b)

typedef struct {
    int startNumber;    // Range includes start number,
    int endNumber;      // But not end number
    IntSlice primes;   // Output parameter for result
} ThreadParameters;

typedef struct {
    int next;           // the start number for the next thread
    int step;           // the size of numbers to be checked by each thread
    int n;              // the end number to which main is finding all primes from 1
} InitializerState;

// Assign each thread with start and end numbers along with a data structure (IntSlice) to store result
void initialize_para(InitializerState *states, ThreadParameters *tPara) {
    tPara->startNumber = states->next;
    states->next = MIN(states->next + states->step, states->n);
    tPara->endNumber = states->next;
    tPara->primes = make_slice(0, 10);
}

// Free data structure (IntSlice) in thread parameters struct
void free_para(ThreadParameters *tPara) {
    free_slice(&(tPara->primes));
}


// find all primes between [start, end) and store the results in an output parameter
void* find_primes_from(void *tPara) {
    // Getting start and end numbers
    ThreadParameters *tP = (ThreadParameters *) tPara;
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



int main(int argc, char *argv[]) {
  int n = 100;
  if (argc >= 2) {
    n = atoi(argv[1]);
  }
  printf("N: %d\n", n);
  FILE *f = stdout;
  if (n > 100) {
    if (argc >= 3) {
      f = fopen(argv[2], "w");
    } else {
      f = fopen("output.txt", "w");
    }
    if (f == NULL) {
      printf("Cannot open file\n");
      return 1;
    }
  }

  clock_t start = clock();
  
  // POSIX threads parallelization
  pthread_t tid[NUM_THREADS];
  ThreadParameters tArg[NUM_THREADS];

  InitializerState states;
  states.next = 0;
  states.step = ceil(n / NUM_THREADS);
  states.n = n;

  for (int i = 0; i < NUM_THREADS; i++) {
    initialize_para(&states, &tArg[i]);
    pthread_create(&tid[i], NULL, &find_primes_from, &tArg[i]);
  }

  int prime_count = 0;
  for (int i = 0; i < NUM_THREADS; i++) {
    pthread_join(tid[i], NULL);
    
    IntSlice p = tArg[i].primes;
    for (int j = 0; j < p.len; j++) {
        fprintf(f, "%d\n", p.arr[j]);
        prime_count++;
    }
    
    free_para(&tArg[i]);
  }

  fprintf(f, "Total: %d\n", prime_count);

  clock_t end = clock();
  fprintf(f, "Duration: %.4fs\n", (double)(end - start) / CLOCKS_PER_SEC);
  fclose(f);
  return 0;
}
