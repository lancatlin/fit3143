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

long repeated_squaring(int a, int b, int n) {
    // return a^b mod n
    // printf("repeated squaring: a %d, b %d, n %d\n", a, b, n);
    long x = 1;
    long d = floor(log2(b)) + 1;
    for (int i = d - 1; i >= 0; i--) {
        bool bit = b >> i & 1;
        x = (x * x) % n;
        if (bit) {
            x = (x * a) % n;
        }
    }
    return x;
}

bool miller_rabin(long a, long n) {
    // return a^b mod n
    long t = n - 1;
    long s = 0;
    while (t % 2 == 0) {
        s++;
        t >>= 1;
    }

    long x = repeated_squaring(a, t, n);

    if (x == 1 || x == n - 1) {
        return true;
    }

    for (long i = 0; i < s; i++) {
        x = (x * x) % n;
        if (x == n - 1) {
            return i != s - 1;
        } else if (x == 1) {
            return false;
        }
    }
    return false;
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
