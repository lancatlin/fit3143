#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// find all primes up to but not including n
void find_primes(int n, FILE *f) {
  if (n < 2)
    return;
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
      fprintf(f, "%d\n", i);
      count += 1;
    }
  }
  fprintf(f, "Total: %d\n", count);
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
  find_primes(n, f);
  clock_t end = clock();
  fprintf(f, "Duration: %.4fs\n", (double)(end - start) / CLOCKS_PER_SEC);
  fclose(f);
  return 0;
}
