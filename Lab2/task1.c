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
#include <stdlib.h>

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

int mpi_rank = 0;
int mpi_size = 1;
MPI_Datatype Request;

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
    long scan_count = 0;

    for (int i = req.start; i < req.end; i++) {
        int sqroot = floor(sqrt(i));
        bool is_prime = true;
        for (int j = 0; j < base_primes.len && base_primes.arr[j] <= sqroot;
             j++) {
            scan_count++;
            if (i % base_primes.arr[j] == 0) {
                is_prime = false;
                break;
            }
        }
        // append prime to result
        if (is_prime) {
            append_slice_long(&primes, i);
        }
    }
    printf("Done: rank: %d start: %ld, end: %ld, size: %ld, primes: %ld, scan: "
           "%ld\n",
           mpi_rank, req.start, req.end, req.end - req.start, primes.len,
           scan_count);

    return primes;
}

void receive_job() {
    struct ProcessRequest req = {0, 0};

    MPI_Scatter(NULL, 1, Request, &req, 1, Request, 0, MPI_COMM_WORLD);

    LongSlice base_primes = find_primes_sieve(ceil(sqrt(req.end)));

    LongSlice primes = find_primes(req, base_primes);

    MPI_Gather(&primes.len, 1, MPI_LONG, NULL, 1, MPI_LONG, 0, MPI_COMM_WORLD);
    MPI_Gatherv(primes.arr, primes.len, MPI_LONG, NULL, NULL, NULL, MPI_LONG, 0,
                MPI_COMM_WORLD);
    free_slice_long(&base_primes);
    free_slice_long(&primes);
}

// find all primes up to but not including n
LongSlice dispatch_jobs(long n) {
    printf("Rank: %d, size: %d, N: %ld\n", mpi_rank, mpi_size, n);
    struct ProcessRequest *send_data = (struct ProcessRequest *)malloc(
        sizeof(struct ProcessRequest) * mpi_size);

    // int base_step = n / mpi_size;
    // int l = n / mpi_size / 8;
    double ratio = (double)n * n / mpi_size / 4;
    long base = n / 2 / mpi_size;
    printf("ratio: %f\n", ratio);
    for (int i = 0; i < mpi_size; i++) {
        send_data[i].start = floor(sqrt((double)i * ratio)) + i * base;
        send_data[i].end =
            floor(sqrt(((double)i + 1) * ratio)) + (i + 1) * base;
        printf("senddata[%d].start = %ld, step=%ld, end = %ld\n", i,
               send_data[i].start, send_data[i].end - send_data[i].start,
               send_data[i].end);
    }
    send_data[mpi_size - 1].end = n;

    struct ProcessRequest req = {0, 0};

    MPI_Scatter(send_data, 1, Request, &req, 1, Request, 0, MPI_COMM_WORLD);

    // root starts at 0 so can directly use sieve
    LongSlice primes = find_primes_sieve(req.end);

    LongSlice counts = make_slice_long(mpi_size, mpi_size);

    MPI_Gather(&primes.len, 1, MPI_LONG, counts.arr, 1, MPI_LONG, 0,
               MPI_COMM_WORLD);

    int *displs = NULL;
    int *steps = NULL;

    int sum = 0;
    displs = (int *)malloc(sizeof(int) * mpi_size);
    steps = (int *)malloc(sizeof(int) * mpi_size);
    for (int i = 0; i < mpi_size; i++) {
        displs[i] = sum;
        steps[i] = counts.arr[i];
        sum += counts.arr[i];
    }
    LongSlice all_primes = make_slice_long(sum, sum);

    MPI_Gatherv(primes.arr, primes.len, MPI_LONG, all_primes.arr, steps, displs,
                MPI_LONG, 0, MPI_COMM_WORLD);

    free_slice_long(&primes);
    free_slice_long(&counts);
    free(displs);
    free(steps);
    return all_primes;
}

int main(int argc, char *argv[]) {
    struct ProcessRequest req;

    MPI_Datatype type[2] = {MPI_LONG, MPI_LONG};
    int blocklen[2] = {1, 1};
    MPI_Aint disp[2];

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    MPI_Get_address(&req.start, &disp[0]);
    MPI_Get_address(&req.end, &disp[1]);

    // Make relative
    disp[1] = disp[1] - disp[0];
    disp[0] = 0;

    MPI_Type_create_struct(2, blocklen, disp, type, &Request);
    MPI_Type_commit(&Request);

    long n = 100;

    if (argc >= 2) {
        n = atol(argv[1]);
    }

    if (mpi_rank == 0) {
        int exit_code = run_task(n, dispatch_jobs, "mpi-task1c", mpi_size);
    } else {
        receive_job();
    }

    MPI_Finalize();
    return 0;
}
