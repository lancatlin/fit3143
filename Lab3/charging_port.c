#include "charging_port.h"
#include "random_generator.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ChargingPort create_charging_port(int id) {
    ChargingPort port = {id, 0, false, false};
    return port;
}

struct PThreadParameter {
    ChargingPort* port;
    RandomGenerator* generator;
    sem_t* sem;
};

void* _start_port(void* args) {
    struct PThreadParameter* params = (struct PThreadParameter*)args;
    ChargingPort* port = params->port;
    RandomGenerator* generator = params->generator;
    sem_t* sem = params->sem;

    while (port->running) {
        // printf("Port %d running: Status %b, Seed: %u\n", port->id,
        // port->occupied, generator->seed);
        int interval = next_random_value(generator);
        sleep(interval);
        port->occupied = !port->occupied;
        sem_post(sem);
    }

    printf("Port %d shutting down\n", port->id);

    free(params);
    return NULL;
}

void start_port(ChargingPort* port, RandomGenerator* generator, sem_t* sem) {
    port->running = true;
    struct PThreadParameter* params =
        (struct PThreadParameter*)malloc(sizeof(struct PThreadParameter));
    params->port = port;
    params->generator = generator;
    params->sem = sem;
    int result = pthread_create(&port->thread_id, NULL, _start_port, params);
    if (result != 0) {
        perror("Failed to create pthread");
    }
}

void stop_port(ChargingPort* port) {
    port->running = false;
}

void join_port(ChargingPort* port) {
    pthread_join(port->thread_id, NULL);
}
