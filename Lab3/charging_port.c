#include "charging_port.h"
#include "random_generator.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define OCCUPIED_RATE 2
#define POLL_TIMEOUT 1

ChargingPort create_charging_port(int id) {
    ChargingPort port = {id, 0, false, false};
    return port;
}

struct PThreadParameter {
    ChargingPort* port;
    RandomGenerator* generator;
    sem_t* on_update;
    sem_t* new_car;
};

void* _start_port(void* args) {
    struct PThreadParameter* params = (struct PThreadParameter*)args;
    ChargingPort* port = params->port;
    RandomGenerator* generator = params->generator;
    sem_t* on_update = params->on_update;
    sem_t* new_car = params->new_car;
    struct timespec ts;

    while (port->running) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += POLL_TIMEOUT;
        int result = sem_timedwait(new_car, &ts);
        if (result != 0) {
            continue;
        }
        printf("Port %d got a new car\n", port->id);
        port->occupied = true;
        sem_post(on_update);
        int interval = next_random_value(generator);
        if (port->occupied) {
            interval *= OCCUPIED_RATE;
        }
        usleep(interval);
        port->occupied = false;
        printf("Port %d released\n", port->id);
        sem_post(on_update);
    }

    printf("Port %d shutting down\n", port->id);

    free(params);
    return NULL;
}

void start_port(ChargingPort* port, RandomGenerator* generator,
                sem_t* on_update, sem_t* new_car) {
    port->running = true;
    struct PThreadParameter* params =
        (struct PThreadParameter*)malloc(sizeof(struct PThreadParameter));
    params->port = port;
    params->generator = generator;
    params->on_update = on_update;
    params->new_car = new_car;
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
