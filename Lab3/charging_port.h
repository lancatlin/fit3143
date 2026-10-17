#pragma once

#include "random_generator.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct {
    int id;
    pthread_t thread_id;
    bool occupied;
    bool running;
} ChargingPort;

ChargingPort create_charging_port(int id);

void start_port(ChargingPort* port, RandomGenerator* generator, sem_t* sem);

void stop_port(ChargingPort* port);

void join_port(ChargingPort* port);
