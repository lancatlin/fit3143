// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#include "slice.h"
#include <bits/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

int run_task(int argc, char *argv[], int (*func)(int, IntSlice), char *label) {
    int n = 100;
    char *dir_name = "logs";

    if (argc >= 2) {
        n = atoi(argv[1]);
    }

    // Set output stream to stdout or file
    FILE *f = stdout;
    if (n > 100) {
        // Write output to file if exceeding 100

        // Check log directory
        struct stat statbuf;
        if (stat(dir_name, &statbuf) == 0) {
            printf("Output directory %s exists\n", dir_name);
        } else {
            // dir does not exist
            if (mkdir(dir_name, 0755) == 0) {
                printf("%s directory successfully created\n", dir_name);
            } else {
                perror("Failed to create dir");
            }
        }
        char filename[100];

        sprintf(filename, "%s/%s-%d.log", dir_name, label, n);
        f = fopen(filename, "w");
        if (f == NULL) {
            fprintf(stderr, "Cannot open file\n");
            return 1;
        }
        printf("Output saving to %s\n", filename);
    }

    fprintf(f, "N: %d\n", n);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    IntSlice primes = make_slice(n, n);
    int count = func(n, primes);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec);
    elapsed += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    fprintf(f, "Total: %d\n", count);
    fprintf(f, "Compute time: %.4f\n", elapsed);

    for (int i = 0; i < primes.len; i++) {
        if (primes.arr[i]) {
            fprintf(f, "%d\n", i);
        }
    }
    fclose(f);

    free_slice(&primes);
    return 0;
}
