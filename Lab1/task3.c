#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <omp.h>
#include <stdbool.h>
#include <stdio.h>

// find all primes up to but not including n
IntSlice find_primes(int n) {
  IntSlice primes = make_slice(0, 10);
  if (n < 2)
    return primes;

  int sum = 0;
#pragma omp parallel
  {
    IntSlice lprimes = make_slice(0, 10);
#pragma omp for reduction(+ : sum)
    for (int i = 2; i < n; i++) {
      printf("Thread %d: i: %d\n", omp_get_thread_num(), i);

      int sqroot = floor(sqrt(i));
      bool is_prime = true;
      for (int j = 2; j <= sqroot; j++) {
        if (i % j == 0) {
          is_prime = false;
          break;
        }
      }
      if (is_prime) {
        append_slice(&lprimes, i);
        sum += 1;
      }
    }

#pragma omp single
    {
      printf("Thread %d: Resize\n", omp_get_thread_num());
      primes = make_slice(0, sum);
    }

#pragma omp critical
    {
      printf("Thread %d: Copy memory\n", omp_get_thread_num());
      for (int i = 0; i < lprimes.len; i++) {
        printf("Thread %d: Copy %i\n", omp_get_thread_num(), lprimes.arr[i]);
        append_slice(&primes, lprimes.arr[i]);
      }
    }
  }
  return primes;
}

int main(int argc, char *argv[]) { return run_task(argc, argv, find_primes); }
