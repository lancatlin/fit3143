// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
// Compile by running `make`
// Execute by `./task1`
#include "slice.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

struct ProcessRequest {
    long start; // Range includes start number,
    long end;   // But not end number
};

LongSlice find_primes_sieve(long n) {
    // Can only be run serially
    if (n < 2)
        return make_slice_long(0, 1);

    LongSlice is_composite = make_slice_long(n, n);
    LongSlice primes = make_slice_long(0, 100);

    long count = 0;
    long end = ceil(sqrt(n));
    for (int i = 2; i < end; i++) {
        if (!is_composite.arr[i]) {
            count++;
            for (int j = i * i; j < n; j += i) {
                is_composite.arr[j] = true;
            }
        }
    }

    for (int i = 2; i < n; i++) {
        if (!is_composite.arr[i]) {
            append_slice_long(&primes, i);
        }
    }

    free_slice_long(&is_composite);

    return primes;
}

LongSlice find_primes(struct ProcessRequest req, LongSlice base_primes) {
    // input validation
    LongSlice primes = make_slice_long(0, 100);
    if (req.end < 2)
        return primes;

    if (req.start < 2) {
        req.start = 2;
    }
    // prime finding algorithm

    long sum_scan = 0;
    for (int i = req.start; i < req.end; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        long scan_count = 0;
        for (int j = 0; j < base_primes.len && base_primes.arr[j] <= sqroot;
             j++) {
            scan_count++;
            if (i % base_primes.arr[j] == 0) {
                is_prime = false;
                break;
            }
        }
        // append prime to result
        sum_scan += scan_count;
        printf("%d,%ld,%ld\n", i, scan_count, sum_scan);
        if (is_prime) {
            append_slice_long(&primes, i);
        }
    }

    return primes;
}

int main(int argc, char *argv[]) {
    long n = 100;

    if (argc >= 2) {
        n = atol(argv[1]);
    }

    bool save = false;
    if (argc >= 3) {
        save = true;
    }

    LongSlice base_primes = find_primes_sieve(ceil(sqrt(n)));
    struct ProcessRequest req = {2, n};
    LongSlice primes = find_primes(req, base_primes);

    free_slice_long(&base_primes);
    free_slice_long(&primes);

    return 0;
}
