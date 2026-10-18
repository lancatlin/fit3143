#include "charging_node.h"
#include "charging_port.h"
#include "random_generator.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define AVAILABILITY_BASELINE 2

ChargingNode create_charging_node(int id, int port_count) {
    ChargingPort* ports = malloc(sizeof(ChargingPort) * port_count);
    for (int i = 0; i < port_count; i++) {
        ports[i] = create_charging_port((1 + id) * 100 + i);
    }
    ChargingNode node = {id, port_count, ports};
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
    pthread_mutex_t* mutex;
    pthread_cond_t* stop;
};

void* _generate_cars(void* args);

int run_node(ChargingNode* node, int duration) {
    RandomGenerator* generators =
        malloc(sizeof(RandomGenerator) * node->port_count);
    int seed = node->id;
    sem_t on_update, new_car;
    pthread_mutex_t mut = PTHREAD_MUTEX_INITIALIZER;
    pthread_cond_t stop = PTHREAD_COND_INITIALIZER;
    pthread_t generator_id;

    int result = sem_init(&on_update, 1, 0);
    if (result != 0) {
        perror("Failed to create on_update semaphore");
        return -1;
    }
    result = sem_init(&new_car, 1, 0);
    if (result != 0) {
        perror("Failed to create new_car semaphore");
        return -1;
    }

    // start car generator
    struct GenerateCarsParams* params =
        (struct GenerateCarsParams*)malloc(sizeof(struct GenerateCarsParams));
    params->node = node;
    params->new_car = &new_car;
    params->mutex = &mut;
    params->stop = &stop;

    pthread_create(&generator_id, NULL, _generate_cars, params);

    // start ports
    for (int i = 0; i < node->port_count; i++) {
        generators[i] = create_random_generator(i, seed, 1000000, 5000000);
        start_port(&node->ports[i], &generators[i], &on_update, &new_car);
    }
    printf("Node %d started\n", node->id);

    // listen for changes
    int availability = available_ports(node);
    printf("Node %d: available: %d\n", node->id, availability);
    for (int i = 0; i < 10; i++) {
        sem_wait(&on_update); // wait for any port status changes
        availability = available_ports(node);
        printf("Node %d: available: %d\n", node->id, availability);
        if (availability < AVAILABILITY_BASELINE) {
            printf("Node %d: low availability: %d\n", node->id, availability);
        }
    }

    printf("Node %d shutting down\n", node->id);

    // Send stop signal
    pthread_mutex_lock(&mut);
    pthread_cond_broadcast(&stop);
    pthread_mutex_unlock(&mut);

    // Send stop signal to all ports
    for (int i = 0; i < node->port_count; i++) {
        stop_port(&node->ports[i]);
    }
    pthread_join(generator_id, NULL);

    // join all threads
    for (int i = 0; i < node->port_count; i++) {
        join_port(&node->ports[i]);
    }
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

void* _generate_cars(void* args) {
    struct GenerateCarsParams* params = (struct GenerateCarsParams*)args;
    struct timespec ts;
    RandomGenerator generator =
        create_random_generator(params->node->id, 1000, 1, 3);

    while (true) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += next_random_value(&generator);
        int result = pthread_cond_timedwait(params->stop, params->mutex, &ts);
        if (result == 0) {
            // stop
            break;
        }
        // put a new car
        printf("Generator post a new car\n");
        sem_post(params->new_car);
    }

    free(params);
    return NULL;
}
