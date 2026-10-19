#include "my_mpi.h"
#include "charging_node.h"
#include "context.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

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

void* _start_recv_queue(void* args) {
    RecvQueueParams* params = (RecvQueueParams*)args;
    int count = 0;
    int buffer = 0;
    MPI_Status status;
    ChargingNode* node = params->node;
    while (context_running(params->ctx)) {
        int ierr = MPI_Recv(&buffer, 1, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
                            *params->comm2D, &status);
        if (ierr != 0) {
            perror("Failed to receive");
        }
        int source = status.MPI_SOURCE;

        for (int i = 0; i < NBR_COUNT; i++) {
            if (node->neighbors[i].rank == source) {
                node->neighbors[i].availability = buffer;
                printf("Node %d: Updated from %d: %d\n", node->id, source,
                       buffer);
                break;
            }
        }
    }
    free(params);
    return NULL;
}

void start_recv_queue(RecvQueueParams* params) {
    pthread_t thread_id;
    int ierr = pthread_create(&thread_id, NULL, _start_recv_queue, params);
    if (ierr != 0) {
        perror("Failed to create thread for recv queue");
    }
}

void* _start_send_queue(void* args) {
    SendQueueParams* params = (SendQueueParams*)args;
    int count = 0;
    int buffer = 0;
    ChargingNode* node = params->node;
    while (context_running(params->ctx)) {
        buffer = available_ports(node);
        if (buffer < AVAILABILITY_BASELINE) {
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

void start_send_queue(SendQueueParams* params) {
    pthread_t thread_id;
    int ierr = pthread_create(&thread_id, NULL, _start_send_queue, params);
    if (ierr != 0) {
        perror("Failed to create thread for send queue");
    }
}
