#include <mpi/mpi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int master_io(MPI_Comm master_comm, MPI_Comm comm);
int slave_io(MPI_Comm master_comm, MPI_Comm comm);

int main(int argc, char **argv) {
    int rank, local_rank;
    MPI_Comm new_comm;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_split(MPI_COMM_WORLD, rank % 2, 0, &new_comm);
    MPI_Comm_rank(new_comm, &local_rank);
    /* Using MPI_Comm_split to create a new communicator (new_comm)*/
    if (local_rank == 0)
        master_io(MPI_COMM_WORLD, new_comm);
    else
        slave_io(MPI_COMM_WORLD, new_comm);
    MPI_Finalize();
    return 0;
}

/* This is the master */
int master_io(MPI_Comm master_comm, MPI_Comm comm) {
    int i, j, size;
    char buf[256];
    MPI_Status status;
    MPI_Comm_size(comm, &size);
    for (j = 1; j <= 2; j++) {
        for (i = 1; i < size; i++) {
            MPI_Recv(buf, 256, MPI_CHAR, i, 0, comm, &status);
            fputs(buf, stdout);
        }
    }
    return 0;
}

/* This is the slave */
int slave_io(MPI_Comm master_comm, MPI_Comm comm) {
    char buf[256];
    int rank;
    MPI_Comm_rank(comm, &rank);
    sprintf(buf, "Hello from slave %d\n", rank);
    /*Sending data in buffer to master*/
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, 0, comm);
    sprintf(buf, "Goodbye from slave %d\n", rank);
    MPI_Send(buf, strlen(buf) + 1, MPI_CHAR, 0, 0, comm);
    /*Sending data in buffer to master*/
    return 0;
}
