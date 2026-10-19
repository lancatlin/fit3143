#include "charging_port.h"
#include "context.h"
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
    ChargingPort port = {id, 0, false};
    return port;
}

void* _start_port(void* args) {
    StartPortParameter* params = (StartPortParameter*)args;
    ChargingPort* port = params->port;
    RandomGenerator* generator = params->generator;
    sem_t* on_update = params->on_update;
    sem_t* new_car = params->new_car;
    struct timespec ts;
    Context* ctx = params->ctx;

    while (context_running(ctx)) {
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += POLL_TIMEOUT;
        int result = sem_timedwait(new_car, &ts);
        if (result != 0) {
            continue;
        }
        // printf("Port %d got a new car\n", port->id);
        port->occupied = true;
        sem_post(on_update);

        // wait for timeout
        int interval = next_random_value(generator);
        if (context_wait(params->ctx, interval) == 0) {
            break;
        }

        port->occupied = false;
        // printf("Port %d released\n", port->id);
        sem_post(on_update);
    }

    printf("Port %d shutting down\n", port->id);

    free(params);
    return NULL;
}

int start_port(StartPortParameter* params) {
    int result =
        pthread_create(&params->port->thread_id, NULL, _start_port, params);
    if (result != 0) {
        perror("Failed to create pthread");
        return -1;
    }
    return 0;
}

void join_port(ChargingPort* port) {
    pthread_join(port->thread_id, NULL);
}
