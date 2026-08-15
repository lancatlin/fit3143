#include "slice.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// find all primes up to but not including n
IntSlice find_primes(int n) {
  IntSlice primes = make_slice(0, 10);
  if (n < 2)
    return primes;
  int count = 0;
  for (int i = 2; i < n; i++) {
    int sqroot = floor(sqrt(i));
    bool is_prime = true;
    for (int j = 2; j <= sqroot; j++) {
      if (i % j == 0) {
        is_prime = false;
      }
    }
    if (is_prime) {
      append_slice(&primes, i);
    }
  }
  return primes;
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
      f = fopen("output.log", "w");
    }
    if (f == NULL) {
      printf("Cannot open file\n");
      return 1;
    }
  }

  clock_t start = clock();
  IntSlice primes = find_primes(n);
  clock_t end = clock();
  fprintf(f, "Total: %d\n", primes.len);
  fprintf(f, "Duration: %.4fs\n", (double)(end - start) / CLOCKS_PER_SEC);
  for (int i = 0; i < primes.len; i++) {
    fprintf(f, "%d\n", primes.arr[i]);
  }
  fclose(f);

  free_slice(&primes);
  return 0;
}
