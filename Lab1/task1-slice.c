#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <stdbool.h>

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
  return run_task(argc, argv, find_primes, "task1");
}
