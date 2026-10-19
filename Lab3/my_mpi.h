#pragma once

#include "charging_node.h"
#include <mpi.h>

#define NDIMS 2
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1

void set_neighbours(ChargingNode* node, MPI_Comm* comm2D);
