#pragma once

#include "charging_node.h"
#include <mpi.h>
#include <pthread.h>

#define NDIMS 2
#define SHIFT_ROW 0
#define SHIFT_COL 1
#define DISP 1
#define STATUS_UPDATE_INTERVAL 1

#define TAG_NODE_AVAILABILITY 1

void set_neighbours(ChargingNode* node, MPI_Comm* comm2D);

typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    // pthread_mutex_t* mutex;
    // pthread_cond_t* stop;
} RecvQueueParams;

typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    // pthread_mutex_t* mutex;
    // pthread_cond_t* stop;
} SendQueueParams;

void start_recv_queue(RecvQueueParams* params);

void start_send_queue(SendQueueParams* params);
