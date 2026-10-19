#pragma once

#include "context.h"
#include "random_generator.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

typedef struct {
    int id;
    pthread_t thread_id;
    bool occupied;
} ChargingPort;

ChargingPort create_charging_port(int id);

typedef struct {
    ChargingPort* port;
    RandomGenerator* generator;
    sem_t* on_update;
    sem_t* new_car;
    Context* ctx;
} StartPortParameter;

int start_port(StartPortParameter* params);

void join_port(ChargingPort* port);
