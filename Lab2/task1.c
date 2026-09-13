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

int mpi_rank = 0;
int mpi_size = 1;
MPI_Datatype Request;

struct ProcessRequest {
    int start; // Range includes start number,
    int end;   // But not end number
};

int find_primes(struct ProcessRequest req, IntSlice is_primes) {
    // input validation
    if (req.end < 2)
        return 0;

    // prime finding algorithm
    int count = 0;
    for (int i = req.start; i < req.end; i++) {
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

// find all primes up to but not including n
int dispatch_jobs(int n, IntSlice is_primes) {
    printf("Rank: %d, size: %d\n", mpi_rank, mpi_size);
    struct ProcessRequest *send_data = (struct ProcessRequest *)malloc(
        sizeof(struct ProcessRequest) * mpi_size);

    if (mpi_rank == 0) {
        int step = n / mpi_size;
        for (int i = 0; i < mpi_size; i++) {
            send_data[i].start = i * step;
            send_data[i].end = (i + 1) * step;
            printf("senddata[%d].start = %d\n", i, send_data[i].start);
        }
    }

    struct ProcessRequest received = {0, 0};
    struct ProcessRequest send = {1, 2};

    MPI_Scatter(send_data, 1, Request, &received, 1, Request, 0,
                MPI_COMM_WORLD);

    int count = find_primes(received, is_primes);

    printf("Received: rank: %d start: %d, end: %d\n", mpi_rank, received.start,
           received.end);

    MPI_Barrier(MPI_COMM_WORLD);

    if (mpi_rank == 0) {
        int collected_count = 1;
        IntSlice collected = make_slice(mpi_size, mpi_size);
        collected.arr[0] = true;
        while (collected_count < mpi_size) {
            for (int node = 1; node < mpi_size; node++) {
                if (collected.arr[node]) {
                    continue;
                }
                int lcount = 0;
                MPI_Recv(&count, 1, MPI_INT, node, 0, MPI_COMM_WORLD,
                         MPI_STATUS_IGNORE);

                printf("lcount of %d: %d\n", node, lcount);

                if (lcount > 0) {
                    IntSlice buffer = make_slice(lcount, lcount);
                    MPI_Recv(buffer.arr, lcount, MPI_INT, node, 0,
                             MPI_COMM_WORLD, MPI_STATUSES_IGNORE);
                    for (int j = 0; j < buffer.len; j++) {
                        is_primes.arr[j] = true;
                    }
                    collected_count++;
                    collected.arr[node] = true;
                }
            }
        }
    } else {
        printf("Rank %d: count: %d\n", mpi_rank, count);
        MPI_Bsend(&count, 1, MPI_INT, 0, 0, MPI_COMM_WORLD);

        IntSlice buffer = make_slice(0, count);
        for (int j = received.start; j < received.end; j++) {
            if (is_primes.arr[j]) {
                append_slice(&buffer, j);
            }
        }
        MPI_Bsend(buffer.arr, count, MPI_INT, 0, 0, MPI_COMM_WORLD);
    }

    return count;
}

int main(int argc, char *argv[]) {
    struct ProcessRequest req;

    MPI_Datatype type[2] = {MPI_INT, MPI_INT};
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

    int exit_code = run_task(argc, argv, dispatch_jobs, "task1");

    MPI_Finalize();
    return 0;
}
