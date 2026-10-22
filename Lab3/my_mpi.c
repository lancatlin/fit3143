#include "my_mpi.h"
#include "charging_node.h"
#include "context.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

/*
 * Set the cartesion neighbours for a node
 */
void set_neighbours(ChargingNode* node, MPI_Comm* comm2D) {
    int coord[NDIMS];
    int my_cart_rank;

    MPI_Cart_coords(*comm2D, node->id, NDIMS, coord);
    MPI_Cart_rank(*comm2D, coord, &my_cart_rank);

    printf("PW[%d]: my_cart_rank PCM[%d], my coords = (%d, %d)\n", node->id,
           my_cart_rank, coord[0], coord[1]);

    MPI_Cart_shift(*comm2D, SHIFT_ROW, DISP, &node->neighbors[LEFT].rank,
                   &node->neighbors[RIGHT].rank);
    MPI_Cart_shift(*comm2D, SHIFT_COL, DISP, &node->neighbors[DOWN].rank,
                   &node->neighbors[UP].rank);
    printf("PW[%2d] Coord(%d,%d): UP[%2d] DOWN[%2d] \n", node->id, coord[0],
           coord[1], node->neighbors[UP].rank, node->neighbors[DOWN].rank);
    printf("PW[%2d] Coord(%d,%d): LEFT[%2d] RIGHT[%2d] \n", node->id, coord[0],
           coord[1], node->neighbors[LEFT].rank, node->neighbors[RIGHT].rank);
}

/* Update a neighbour's availability from received message */
void _update_neighbour_availability(ChargingNode* node, int source,
                                    int buffer) {
    for (int i = 0; i < NBR_COUNT; i++) {
        if (node->neighbors[i].rank == source) {
            node->neighbors[i].availability = buffer;
            // printf("Node %d: Updated from %d: %d\n", node->id, source,
            //        buffer);
            break;
        }
    }
}

/*
 * Receive messages from any node, and call corresponding handler by tag
 */
void* _start_recv_queue(void* args) {
    RecvQueueParams* params = (RecvQueueParams*)args;
    int count = 0;
    int buffer = 0;
    MPI_Request request;
    MPI_Status status;
    ChargingNode* node = params->node;

    int ierr = MPI_Irecv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                         *params->comm2D, &request);
    if (ierr != MPI_SUCCESS) {
        perror("Failed to post receive");
        return NULL;
    }
    while (context_running(params->ctx)) {
        int done = 0;
        MPI_Test(&request, &done, &status);
        if (!done) {
            if (context_wait(params->ctx, STATUS_UPDATE_INTERVAL) == 0) {
                // received shutdown signal, exit
                break;
            }
            continue;
        }

        int source = status.MPI_SOURCE;
        int tag = status.MPI_TAG;
        switch (tag) {
        case TAG_NODE_AVAILABILITY:
            _update_neighbour_availability(node, source, buffer);
            break;

        default:
            printf("Node %d: received undefined message tag: %d\n", node->id,
                   tag);
        }

        MPI_Irecv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                  *params->comm2D, &request);
    }
    MPI_Cancel(&request);
    MPI_Wait(&request, MPI_STATUS_IGNORE);

    free(params);
    return NULL;
}

pthread_t start_recv_queue(RecvQueueParams* params) {
    pthread_t thread_id;
    int ierr = pthread_create(&thread_id, NULL, _start_recv_queue, params);
    if (ierr != 0) {
        perror("Failed to create thread for recv queue");
    }
    return thread_id;
}

/*
 * Periodically send the current availability to neighbours when it is changed
 */
void* _start_send_queue(void* args) {
    SendQueueParams* params = (SendQueueParams*)args;
    int count = 0;
    int buffer = 0;
    ChargingNode* node = params->node;
    while (context_running(params->ctx)) {
        int availability = available_ports(node);
        if (buffer != availability) {
            buffer = availability;
            for (int i = 0; i < NBR_COUNT; i++) {
                if (node->neighbors[i].rank >= 0) {
                    MPI_Send(&buffer, 1, MPI_INT, node->neighbors[i].rank,
                             TAG_NODE_AVAILABILITY, *params->comm2D);
                }
            }
        }
        if (context_wait(params->ctx, STATUS_UPDATE_INTERVAL) == 0) {
            break;
        }
    }
    free(params);
    return NULL;
}

pthread_t start_send_queue(SendQueueParams* params) {
    pthread_t thread_id;
    int ierr = pthread_create(&thread_id, NULL, _start_send_queue, params);
    if (ierr != 0) {
        perror("Failed to create thread for send queue");
    }
    return thread_id;
}
