#pragma once

#include "random_generator.h"
#include <pthread.h>
#include <stdbool.h>

typedef struct {
    int id;
    pthread_t thread_id;
    bool occupied;
    bool running;
} ChargingPort;

ChargingPort create_charging_port(int id);

void start_port(ChargingPort* port, RandomGenerator* generator);

void stop_port(ChargingPort* port);
