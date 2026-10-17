#include "charging_port.h"
#include "random_generator.h"
#include <bits/pthreadtypes.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ChargingPort create_charging_port(int id) {
    ChargingPort port = {id, false, false};
    return port;
}

struct PThreadParameter {
    ChargingPort* port;
    RandomGenerator* generator;
};

void* _start_port(void* args) {
    struct PThreadParameter* params = (struct PThreadParameter*)args;
    ChargingPort* port = params->port;
    RandomGenerator* generator = params->generator;

    while (port->running) {
        printf("Port %d running: Status %b, Seed: %u\n", port->id,
               port->occupied, generator->seed);
        int interval = next_random_value(generator);
        sleep(interval);
        port->occupied = !port->occupied;
    }

    free(params);
    return NULL;
}

pthread_t start_port(ChargingPort* port, RandomGenerator* generator) {
    port->running = true;
    pthread_t thread_id;
    struct PThreadParameter* params =
        (struct PThreadParameter*)malloc(sizeof(struct PThreadParameter));
    params->port = port;
    params->generator = generator;
    int result = pthread_create(&thread_id, NULL, _start_port, params);
    if (result != 0) {
        perror("Failed to create pthread");
    }
    return thread_id;
}

void stop_port(ChargingPort* port, pthread_t thread_id) {
    port->running = false;
    pthread_join(thread_id, NULL);
}
