#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>

// using MPI_Scatter and MPI_Gather for collective communication
int main(int argc, char **argv) {
    int rank, size;
    int received_data;
    int *send_buffer = NULL; // Buffer for root to send data

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    if (rank == 0) {
        // Allocate and initialize send buffer on root process
        // size should be the number of processes (in integer)
        send_buffer = (int *)malloc(size * sizeof(int));
    }

    while (1) {
        if (rank == 0) {
            printf("Enter an integer (negative to quit): ");
            fflush(stdout);
            // Read input into the first element of send_buffer
            int input_val;
            scanf("%d", &input_val);
            // Initialize the send buffer with the input value for all processes
            for (int i = 0; i < size; i++) {
                send_buffer[i] = input_val * i;
            }
        }

        // root node scatters data to all processes
        MPI_Scatter(send_buffer, 1, MPI_INT, &received_data, 1, MPI_INT, 0,
                    MPI_COMM_WORLD);

        // Check if the data is negative to terminate
        if (received_data < 0) {
            break;
        }

        printf("Process %d received data: %d\n", rank, received_data);
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD); // Collective operations that synchronize
                                     // all processes before next iteration.
    }

    if (rank == 0) {
        free(send_buffer); // clear allocated memory
    }

    MPI_Finalize();
    return 0;
}
