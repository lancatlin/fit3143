#include "my_mpi.h"
#include "charging_node.h"
#include <stdio.h>

void set_neighbours(ChargingNode* node, MPI_Comm* comm2D) {
    int coord[NDIMS];
    int my_cart_rank;

    MPI_Cart_coords(*comm2D, node->id, NDIMS, coord);
    MPI_Cart_rank(*comm2D, coord, &my_cart_rank);

    printf("PW[%d]: my_cart_rank PCM[%d], my coords = (%d, %d)\n", node->id,
           my_cart_rank, coord[0], coord[1]);

    MPI_Cart_shift(*comm2D, SHIFT_ROW, DISP, &node->neighbors[LEFT],
                   &node->neighbors[RIGHT]);
    MPI_Cart_shift(*comm2D, SHIFT_COL, DISP, &node->neighbors[DOWN],
                   &node->neighbors[UP]);
    printf("PW[%2d] Coord(%d,%d): UP[%2d] DOWN[%2d] \n", node->id, coord[0],
           coord[1], node->neighbors[UP], node->neighbors[DOWN]);
    printf("PW[%2d] Coord(%d,%d): LEFT[%2d] RIGHT[%2d] \n", node->id, coord[0],
           coord[1], node->neighbors[LEFT], node->neighbors[RIGHT]);
}
