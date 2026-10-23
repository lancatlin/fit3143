#include "charging_port.h"
#include "context.h"
#include "random_generator.h"
#include "time_helper.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define OCCUPIED_RATE 2
#define POLL_TIMEOUT 100

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
    Context* ctx = params->ctx;

    while (context_running(ctx)) {
        struct timespec ts;
        ts = get_future(POLL_TIMEOUT);
        int result = sem_timedwait(new_car, &ts);
        if (result != 0) {
            // No new car received, continue
            continue;
        }
        port->occupied = true;
        sem_post(on_update);

        // wait for the car finish charging
        int interval = next_random_value(generator);
        if (context_wait(params->ctx, interval) == 0) {
            // Received shutdown signal
            break;
        }

        port->occupied = false;
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
