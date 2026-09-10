// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
// Compile by running `make`
// Execute by `./task3`
// Set number of threads by `OMP_NUM_THREADS=8 ./task3`
#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#define K 20

// Return a random number from 2 to n-2
long rand_base(int n) { return rand() % (n - 3) + 2; }

bool miller_rabin(long a, long n) {
    // return a^b mod n
    // printf("repeated squaring: a %ld, n %ld\n", a, n);
    long x = 1;
    long b = n - 1;
    long t = b;
    long s = 0;
    while (t % 2 == 0) {
        s++;
        t >>= 1;
    }

    long d = floor(log2(b)) + 1;

    for (long i = 0; i < d; i++) {
        bool bit = b >> (d - i - 1) & 1;
        x = (x * x) % n;
        if (bit) {
            x = (x * a) % n;
        }
        if (i >= (d - s)) {
            if (x == n - 1) {
                return i != d;
            } else if (x == 1) {
                return i == d - s;
            }
        }
    }
    return x == 1;
}

bool is_prime(int n, int k) {
    if (n % 2 == 0) {
        return n == 2;
    }
    if (n == 3) {
        return true;
    }
    // printf("is_prime: %d\n", n);
    for (int i = 0; i < k; i++) {
        long base = rand_base(n);
        if (!miller_rabin(base, n)) {
            return false;
        }
    }

    return true;
}

IntSlice find_primes(int n) {
    if (n < 2)
        return make_slice(0, 1);

    IntSlice primes = make_slice(0, 10);
    printf("Finished N: %d\n", n);
    for (int i = 2; i < n; i++) {
        if (is_prime(i, K)) {
            append_slice(&primes, i);
        }
    }
    return primes;
}

int main(int argc, char *argv[]) {
    return run_task(argc, argv, find_primes, "miller_rabin");
}
