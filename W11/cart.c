/* mpi-cart-2D.c -- test basic -cartesian functions
* Written by Mary Thomas- Updated Mar, 2015
* Based loosely on code from Pacheco’97,
*
Chap 7, pp. 121 & ff in PPMPI */
#include "mpi.h"
#include <stdio.h>
#define NDIMS 2
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
/* create cartesian topology for processes */

int main(int argc, char* argv[]) {
    int p;
    int my_rank;
    int my_cart_rank;
    MPI_Comm comm2D;
    int dims[NDIMS] = {2, 4};
    int coord[NDIMS];
    int wrap_around[NDIMS] = {0, 0};

    MPI_Init(&argc, &argv);
    MPI_Comm_size(MPI_COMM_WORLD, &p);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    MPI_Dims_create(p, NDIMS, dims);
    if (my_rank == 0) {
        printf("PW[%d]/[%d]: PEdims = [%d x %d]\n", my_rank, p, dims[0],
               dims[1]);
    }

    int reorder = 1;
    int ierr = MPI_Cart_create(MPI_COMM_WORLD, NDIMS, dims, wrap_around,
                               reorder, &comm2D);
    if (ierr != 0) {
        perror("Failed to create cartesion");
    }

    MPI_Cart_coords(comm2D, my_rank, NDIMS, coord);
    MPI_Cart_rank(comm2D, coord, &my_cart_rank);

    printf("PW[%d]: my_cart_rank PCM[%d], my coords = (%d, %d)\n", my_rank,
           my_cart_rank, coord[0], coord[1]);

    int nbr_i_lo, nbr_i_hi, nbr_j_lo, nbr_j_hi;
    MPI_Cart_shift(comm2D, SHIFT_ROW, DISP, &nbr_i_lo, &nbr_i_hi);
    MPI_Cart_shift(comm2D, SHIFT_COL, DISP, &nbr_j_lo, &nbr_j_hi);
    printf("PW[%2d] Coord(%d,%d): SHIFT_DIM[%d], Shift=%d: nbr_lo[%2d] P[%2d] "
           "nbr_hi[%2d] \n",
           my_rank, coord[0], coord[1], SHIFT_ROW, DISP, nbr_i_lo, my_rank,
           nbr_i_hi);
    printf("PW[%2d] Coord(%d,%d): SHIFT_DIM[%d], Shift=%d: nbr_lo[%2d] P[%2d] "
           "nbr_hi[%2d] \n",
           my_rank, coord[0], coord[1], SHIFT_COL, DISP, nbr_j_lo, my_rank,
           nbr_j_hi);
    fflush(stdout);

    MPI_Comm_free(&comm2D);

    MPI_Finalize();
}
