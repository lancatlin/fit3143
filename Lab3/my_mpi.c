#include "my_mpi.h"
#include "charging_node.h"
#include "context.h"
#include "message.h"
#include "mpi.h"
#include "table.h"
#include <assert.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
                                    int* buffer) {
    for (int i = 0; i < NBR_COUNT; i++) {
        if (node->neighbors[i].rank == source) {
            node->neighbors[i].availability = buffer[0];
            // printf("Node %d: Updated from %d: %d\n", node->id, source,
            //        buffer);
            break;
        }
    }
}

void _send_availability(MessageQueue* queue, ChargingNode* node, int recipient,
                        int* buffer) {
    Message msg;
    msg.recipient = recipient;
    msg.tag = TAG_NODE_AVAILABILITY_RESPONSE;
    msg.buffer[0] = available_ports(node);
    msg.buffer[1] = buffer[1]; // reply with same message id
    message_queue_put(queue, msg);
}
#define RECV_POLL_US 1000

/*
 * Receive messages from any node, and call corresponding handler by tag
 */
void* _start_recv_queue(void* args) {
    RecvQueueParams* params = (RecvQueueParams*)args;
    int count = 0;
    int buffer[PAYLOAD_SIZE];
    MPI_Request request;
    MPI_Status status;
    ChargingNode* node = params->node;
    MessageQueue* queue = &params->ctx->queue;

    int ierr = MPI_Irecv(&buffer, PAYLOAD_SIZE, MPI_INT, MPI_ANY_SOURCE,
                         MPI_ANY_TAG, *params->comm2D, &request);
    if (ierr != MPI_SUCCESS) {
        perror("Failed to post receive");
        return NULL;
    }
    while (context_running(params->ctx)) {
        int done = 0;
        MPI_Test(&request, &done, &status);
        if (!done) {
            usleep(RECV_POLL_US);
            continue;
        }

        int source = status.MPI_SOURCE;
        int tag = status.MPI_TAG;
        switch (tag) {
        case TAG_NODE_AVAILABILITY_REQUEST:
            _send_availability(queue, node, source, buffer);
            break;
        case TAG_NODE_AVAILABILITY_RESPONSE:
            request_table_received(&params->ctx->table, buffer[1]);
            _update_neighbour_availability(node, source, buffer);
            break;

        default:
            printf("Node %d: received undefined message tag: %d\n", node->id,
                   tag);
        }

        MPI_Irecv(&buffer, PAYLOAD_SIZE, MPI_INT, MPI_ANY_SOURCE, MPI_ANY_TAG,
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

#define SEND_SLOTS 8

/*
 * Periodically send the current availability to neighbours when it is changed
 */
void* _start_send_queue(void* args) {
    SendQueueParams* params = (SendQueueParams*)args;
    int count = 0;
    int buffer = 0;
    ChargingNode* node = params->node;

    // Each in-flight MPI_Isend needs its own buffer and request. The buffer
    // must stay untouched until the request completes.
    Message msgs[SEND_SLOTS];
    MPI_Request reqs[SEND_SLOTS];
    for (int i = 0; i < SEND_SLOTS; i++) {
        reqs[i] = MPI_REQUEST_NULL;
    }

    MessageQueue* queue = &params->ctx->queue;
    while (context_running(params->ctx)) {
        // Reap finished sends and find a free slot
        int slot = -1;
        for (int i = 0; i < SEND_SLOTS; i++) {
            if (reqs[i] != MPI_REQUEST_NULL) {
                int done = 0;
                MPI_Test(&reqs[i], &done, MPI_STATUS_IGNORE);
            }
            if (reqs[i] == MPI_REQUEST_NULL && slot < 0) {
                slot = i;
            }
        }

        if (slot >= 0 && queue->count > 0) {
            int err = message_queue_take(queue, &msgs[slot]);
            assert(err == 0);
            Message* msg = &msgs[slot];
            MPI_Isend(msg->buffer, PAYLOAD_SIZE, MPI_INT, msg->recipient,
                      msg->tag, *params->comm2D, &reqs[slot]);
            printf("Node %d: Sent message id %d tag %d to %d: %d, %d\n",
                   node->id, msg->id, msg->tag, msg->recipient, msg->buffer[0],
                   msg->buffer[1]);
        } else {
            struct timespec ts = get_future(STATUS_UPDATE_INTERVAL);
            sem_timedwait(&queue->new_message, &ts);
        }
    }
    // Let outstanding sends finish before the buffers go away
    MPI_Waitall(SEND_SLOTS, reqs, MPI_STATUSES_IGNORE);
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
