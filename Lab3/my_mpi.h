#pragma once

#include "charging_node.h"
#include "context.h"
#include <mpi.h>
#include <pthread.h>

#define NDIMS 2
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define STATUS_UPDATE_INTERVAL 100 // milliseconds

#define TAG_NODE_AVAILABILITY 1

void set_neighbours(ChargingNode* node, MPI_Comm* comm2D);

typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    Context* ctx;
} RecvQueueParams;

typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    Context* ctx;
} SendQueueParams;

pthread_t start_recv_queue(RecvQueueParams* params);

pthread_t start_send_queue(SendQueueParams* params);
