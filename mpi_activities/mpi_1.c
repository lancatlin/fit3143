#include <mpi/mpi.h>
#include <stdio.h>

// using MPI_Send and MPI_Recv for point-to-point communication
int main(int argc, char **argv) {
    int rank, size;
    int data;

    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    while (1) {
        if (rank == 0) {
            printf("Enter an integer (negative to quit): ");
            fflush(stdout);
            scanf("%d", &data);

            // Point-to-point communication to send data to all other processes
            for (int i = 1; i < size; i++) {
                MPI_Send(&data, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
            }
        } else {
            // Receive data from process 0
            MPI_Recv(&data, 1, MPI_INT, 0, 0, MPI_COMM_WORLD,
                     MPI_STATUS_IGNORE);
        }

        // Check if the data is negative to terminate
        if (data < 0) {
            break;
        }

        printf("Process %d received data: %d\n", rank, data);
        fflush(stdout);

        MPI_Barrier(MPI_COMM_WORLD); // Collective operations that
        // synchronize all processes before next iteration.
    }

    MPI_Finalize();
    return 0;
}
