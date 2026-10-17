#include "charging_node.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int mpi_rank;
    int mpi_size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    ChargingNode node = create_charging_node(mpi_rank, 9);
    int result = run_node(&node, 10);
    if (result != 0) {
        perror("run node error");
    }

    MPI_Finalize();

    return 0;
}
