// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#include "slice.h"
#include <bits/time.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/stat.h>
#include <time.h>

int run_task(long n, LongSlice (*func)(long), char *label, int mpi_size) {
    char *dir_name = "logs";
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

        sprintf(filename, "%s/%s-%d-%ld.log", dir_name, label, mpi_size, n);
        f = fopen(filename, "w");
        if (f == NULL) {
            fprintf(stderr, "Cannot open file\n");
            return 1;
        }
        printf("Output saving to %s\n", filename);
    }

    fprintf(f, "N: %ld\n", n);

    struct timespec start, end;
    clock_gettime(CLOCK_MONOTONIC, &start);

    LongSlice primes = func(n);

    clock_gettime(CLOCK_MONOTONIC, &end);

    double elapsed = (end.tv_sec - start.tv_sec);
    elapsed += (end.tv_nsec - start.tv_nsec) / 1000000000.0;
    printf("Compute done. Start writing results\n");
    printf("Compute time: %.4f\n", elapsed);
    fprintf(f, "Total: %ld\n", primes.len);
    fprintf(f, "Compute time: %.4f\n", elapsed);

    for (int i = 0; i < primes.len; i++) {
        fprintf(f, "%ld\n", primes.arr[i]);
    }
    fclose(f);

    free_slice_long(&primes);
    return 0;
}
