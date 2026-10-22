/* my_mpi encapsulates all MPI related logic */
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

/* Different tags to indicate message purpose */
#define TAG_NODE_AVAILABILITY 1

/* Set up neighbour list for a node */
void set_neighbours(ChargingNode* node, MPI_Comm* comm2D);

/*
 * RecvQueueParams holds the parameter to start the receiving queue
 */
typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    Context* ctx;
} RecvQueueParams;

/*
 * SendQueueParams holds the parameter to start the sending queue
 */
typedef struct {
    ChargingNode* node;
    MPI_Comm* comm2D;
    Context* ctx;
} SendQueueParams;

/* Start the receving message thread */
pthread_t start_recv_queue(RecvQueueParams* params);

/* Start the sending message thread */
pthread_t start_send_queue(SendQueueParams* params);
