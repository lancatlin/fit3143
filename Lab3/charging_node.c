#include "charging_node.h"
#include "charging_port.h"
#include "random_generator.h"
#include "utils.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

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
        generators[i] = create_random_generator(i, seed, 5, 8);
        StartPortParameter* params =
            (StartPortParameter*)malloc(sizeof(StartPortParameter));
        params->port = &node->ports[i];
        params->generator = &generators[i];
        params->on_update = &on_update;
        params->new_car = &new_car;
        params->mutex = &mut;
        params->stop = &stop;
        start_port(params);
    }
    printf("Node %d started\n", node->id);

    // listen for changes
    int availability = available_ports(node);
    printf("Node %d: available: %d\n", node->id, availability);
    for (int i = 0; i < 50; i++) {
        sem_wait(&on_update); // wait for any port status changes
        availability = available_ports(node);
        printf("Node %d: available: %d\n", node->id, availability);
        if (availability < AVAILABILITY_BASELINE) {
            printf("Node %d: low availability: %d\n", node->id, availability);
        }
    }

    printf("Node %d shutting down\n", node->id);

    // Send stop signal to all ports
    for (int i = 0; i < node->port_count; i++) {
        stop_port(&node->ports[i]);
    }
    // Send stop signal
    pthread_mutex_lock(&mut);
    pthread_cond_broadcast(&stop);
    pthread_mutex_unlock(&mut);

    pthread_join(generator_id, NULL);

    // join all threads
    for (int i = 0; i < node->port_count; i++) {
        join_port(&node->ports[i]);
    }

    // cleanup
    pthread_mutex_destroy(&mut);
    pthread_cond_destroy(&stop);
    sem_destroy(&on_update);
    sem_destroy(&new_car);
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
        if (wait_cond_or_sec(params->mutex, params->stop, 1) == 0) {
            break;
        }
        // put a new car
        // printf("Generator post a new car\n");
        sem_post(params->new_car);
    }

    printf("Generator stopped\n");
    free(params);
    return NULL;
}
