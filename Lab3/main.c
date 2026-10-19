#include "charging_node.h"
#include "my_mpi.h"
#include <mpi.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    int mpi_rank;
    int mpi_size;
    MPI_Comm comm2D;
    int dims[NDIMS] = {2, 4};
    int wrap_around[NDIMS] = {0, 0};
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &mpi_rank);
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);

    MPI_Dims_create(mpi_size, NDIMS, dims);
    if (mpi_rank == 0) {
        printf("PW[%d]/[%d]: PEdims = [%d x %d]\n", mpi_rank, mpi_size, dims[0],
               dims[1]);
    }

    int reorder = 1;
    int ierr = MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, wrap_around,
                               reorder, &comm2D);
    if (ierr != 0) {
        perror("Failed to create cartesion");
    }
    ChargingNode node = create_charging_node(mpi_rank, 9);
    set_neighbours(&node, &comm2D);
    int result = run_node(&node, 10);
    if (result != 0) {
        perror("run node error");
    }

    MPI_Finalize();

    return 0;
}
