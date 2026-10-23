#include "charging_node.h"
#include "charging_port.h"
#include "context.h"
#include "message.h"
#include "random_generator.h"
#include "table.h"
#include <mpi.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

void _request_neighbours(ChargingNode* node, Context* ctx);

ChargingNode create_charging_node(int id, int port_count) {
    ChargingPort* ports = malloc(sizeof(ChargingPort) * port_count);
    for (int i = 0; i < port_count; i++) {
        ports[i] = create_charging_port((1 + id) * 100 + i);
    }
    ChargingNode node;
    node.id = id;
    node.port_count = port_count;
    node.ports = ports;
    return node;
}

void free_charging_node(ChargingNode* node) {
    free(node->ports);
    node->ports = NULL;
    node->port_count = 0;
}

struct GenerateCarsParams {
    ChargingNode* node;
    sem_t* new_car;
    Context* ctx;
};

void* _generate_cars(void* args);

void* _run_node(void* args);

int run_node(RunNodeParams* params) {
    pthread_create(&params->node->thread_id, NULL, _run_node, params);
    return 0;
}

void* _run_node(void* args) {
    RunNodeParams* params = (RunNodeParams*)args;
    ChargingNode* node = params->node;
    Context* ctx = params->ctx;

    RandomGenerator* generators =
        malloc(sizeof(RandomGenerator) * node->port_count);
    int seed = node->id;
    sem_t on_update, new_car;
    pthread_t generator_id;

    int ierr = sem_init(&on_update, 1, 0);
    if (ierr != 0) {
        perror("Failed to create on_update semaphore");
        return NULL;
    }
    ierr = sem_init(&new_car, 1, 0);
    if (ierr != 0) {
        perror("Failed to create new_car semaphore");
        return NULL;
    }

    // start car generator
    struct GenerateCarsParams* gen_params =
        (struct GenerateCarsParams*)malloc(sizeof(struct GenerateCarsParams));
    gen_params->node = node;
    gen_params->new_car = &new_car;
    gen_params->ctx = ctx;
    pthread_create(&generator_id, NULL, _generate_cars, gen_params);

    // start ports
    for (int i = 0; i < node->port_count; i++) {
        generators[i] =
            create_random_generator(i, seed, 7 * SECOND, 10 * SECOND);
        StartPortParameter* params =
            (StartPortParameter*)malloc(sizeof(StartPortParameter));
        params->port = &node->ports[i];
        params->generator = &generators[i];
        params->on_update = &on_update;
        params->new_car = &new_car;
        params->ctx = ctx;
        start_port(params);
    }
    printf("Node %d started\n", node->id);

    // listen for changes
    int availability = available_ports(node);
    printf("Node %d: available: %d\n", node->id, availability);
    while (context_running(ctx)) {
        if (sem_trywait(&on_update) == 0) {
            availability = available_ports(node);
            // printf("Node %d: available: %d\n", node->id, availability);
            if (availability < AVAILABILITY_BASELINE) {
                _request_neighbours(node, params->ctx);
                printf("Node %d: low availability: %d\n", node->id,
                       availability);
            }
        }
        if (context_wait(ctx, 1) == 0) {
            break;
        }
    }

    printf("Node %d shutting down\n", node->id);

    pthread_join(generator_id, NULL);

    // join all threads
    for (int i = 0; i < node->port_count; i++) {
        join_port(&node->ports[i]);
    }

    // cleanup
    sem_destroy(&on_update);
    sem_destroy(&new_car);
    free(params);
    return 0;
}

int available_ports(ChargingNode* node) {
    int sum = 0;
    for (int i = 0; i < node->port_count; i++) {
        if (!node->ports[i].occupied) {
            sum++;
        }
    }
    return sum;
}

Neighbour* _max_neighbour(ChargingNode* node) {
    Neighbour* max_neighbour = NULL;
    int max_availability = -1;
    for (int i = 0; i < NBR_COUNT; i++) {
        Neighbour* nbr = &node->neighbors[i];
        if (nbr->rank >= 0 && nbr->availability > max_availability) {
            max_availability = nbr->availability;
            max_neighbour = nbr;
        }
    }
    return max_neighbour;
}

void _request_neighbours(ChargingNode* node, Context* ctx) {
    Message msg;
    msg.buffer[0] = 0;
    msg.tag = TAG_NODE_AVAILABILITY_REQUEST;
    int err;
    for (int i = 0; i < NBR_COUNT; i++) {
        msg.recipient = node->neighbors[i].rank;
        if (msg.recipient >= 0) {
            msg.buffer[1] = request_table_send(&ctx->table);
            err = message_queue_put(&ctx->queue, msg);
            if (err < 0) {
                perror("Failed to put message into queue\n");
            }
        }
    }
}

/* Generate random car traffic */
void* _generate_cars(void* args) {
    struct GenerateCarsParams* params = (struct GenerateCarsParams*)args;
    struct timespec ts;
    // Generate a new car every 0.5 to 3 seconds
    RandomGenerator generator = create_random_generator(
        params->node->id, 1000, 0.1 * SECOND, 1.5 * SECOND);
    ChargingNode* node = params->node;

    int mpi_size;
    MPI_Comm_size(MPI_COMM_WORLD, &mpi_size);
    MPI_Request request;

    while (true) {
        int interval = next_random_value(&generator);
        if (context_wait(params->ctx, interval) == 0) {
            // Received shutdown signal, exit
            break;
        }
        // put a new car
        // printf("Generator post a new car\n");
        int availability = available_ports(params->node);
        // _request_neighbours(node, params->ctx);
        Neighbour* nbr = _max_neighbour(node);
        if (availability > AVAILABILITY_BASELINE) {
            // Update status to base station
            int data[2] = {node->id, availability - 1};
            MPI_Send(data, 2, MPI_INT, mpi_size - 1, MPI_UPDATE_TAG,
                     MPI_COMM_WORLD);
            printf("Node %d: accepted new car. available: %d\n", node->id,
                   availability - 1);
            // Let a port to take this car
            sem_post(params->new_car);
        } else {

            if (nbr->availability <= AVAILABILITY_BASELINE) {
                printf("Node %d: Congestion happened\n", node->id);

                // Inform base station that charging node and surrounding nodes
                // are full
                int data[2] = {node->id, -1};
                MPI_Send(data, 2, MPI_INT, mpi_size - 1, MPI_FULL_TAG,
                         MPI_COMM_WORLD);
                int response[8]; // eight is the max num of nodes exactly 2
                                 // steps away from any node
                // MPI_Status rStatus;
                // MPI_Recv(response, 8, MPI_INT, mpi_size-1,
                // MPI_BASE_STATION_RESPONSE_TAG, MPI_COMM_WORLD, &rStatus);

                // Convey infomation from base station to EVs
                printf("Node %d: Here are some free nodes: ", node->id);
                bool has_answered = false;
                for (int i = 0; i < 8; i++) {
                    if (response[i] != -1) {
                        printf("%d ", response[i]);
                        has_answered = true;
                    }
                }
                if (!has_answered) {
                    printf("None");
                }
                printf("\n");

            } else {
                // Inform base station that charging node has low availability
                int data[2] = {node->id, nbr->rank};
                MPI_Send(data, 2, MPI_INT, mpi_size - 1, MPI_LOW_AVAIL_TAG,
                         MPI_COMM_WORLD);
                printf("Node %d is low availability %d: please go to Node %d "
                       "(%d free)\n",
                       node->id, availability, nbr->rank, nbr->availability);
            }
        }
    }

    printf("Generator stopped\n");
    free(params);
    return NULL;
}
