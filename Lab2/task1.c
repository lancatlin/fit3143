// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
// Compile by running `make`
// Execute by `./task1`
#include "runtask.h"
#include "slice.h"
#include <math.h>
#include <mpi/mpi.h>
#include <stdbool.h>
#include <stdio.h>

int mpi_rank = 0;
int mpi_size = 1;

struct ProcessRequest {
    int startNumber; // Range includes start number,
    int endNumber;   // But not end number
};

// find all primes up to but not including n
int find_primes(int n, IntSlice is_primes) {
    // input validation
    if (n < 2)
        return 0;

    // prime finding algorithm
    int count = 0;
    for (int i = 2; i < n; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 2; j <= sqroot; j++) {
            if (i % j == 0) {
                is_prime = false;
                break;
            }
        }
        // append prime to result
        if (is_prime) {
            is_primes.arr[i] = is_prime;
            count++;
        }
    }
    return count;
}

int main(int argc, char *argv[]) {
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    printf("Rank: %d, size: %d\n", mpi_rank, mpi_size);

    int exit_code = run_task(argc, argv, find_primes, "task1");

    MPI_Finalize();
    return exit_code;
}
