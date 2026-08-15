#include "slice.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int run_task(int argc, char *argv[], IntSlice (*func)(int)) {
  int n = 100;
  if (argc >= 2) {
    n = atoi(argv[1]);
  }

  FILE *f = stdout;
  if (n > 100) {
    // Write output to file if exceeding 100
    if (argc >= 3) {
      // if the output file is given
      f = fopen(argv[2], "w");
    } else {
      char buf[100];
      sprintf(buf, "primes-%d.log", n);
      f = fopen(buf, "w");
    }
    if (f == NULL) {
      fprintf(stderr, "Cannot open file\n");
      return 1;
    }
  }

  fprintf(f, "N: %d\n", n);
  clock_t start = clock();
  IntSlice primes = func(n);
  clock_t end = clock();
  fprintf(f, "Total: %d\n", primes.len);
  fprintf(f, "Compute time: %.4fs\n", (double)(end - start) / CLOCKS_PER_SEC);
  for (int i = 0; i < primes.len; i++) {
    fprintf(f, "%d\n", primes.arr[i]);
  }
  fclose(f);

  free_slice(&primes);
  return 0;
}
