#include "charging_port.h"
#include "random_generator.h"
#include <bits/pthreadtypes.h>
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

void* _start_port(void* args) {
    StartPortParameter* params = (StartPortParameter*)args;
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
        clock_gettime(CLOCK_REALTIME, &ts);
        ts.tv_sec += interval;

        pthread_mutex_lock(params->mutex);
        if (pthread_cond_timedwait(params->stop, params->mutex, &ts) == 0) {
            port->running = false;
        }
        pthread_mutex_unlock(params->mutex);

        port->occupied = false;
        printf("Port %d released\n", port->id);
        sem_post(on_update);
    }

    printf("Port %d shutting down\n", port->id);

    free(params);
    return NULL;
}

int start_port(StartPortParameter* params) {
    params->port->running = true;
    int result =
        pthread_create(&params->port->thread_id, NULL, _start_port, params);
    if (result != 0) {
        perror("Failed to create pthread");
        return -1;
    }
    return 0;
}

void stop_port(ChargingPort* port) {
    port->running = false;
}

void join_port(ChargingPort* port) {
    pthread_join(port->thread_id, NULL);
}
