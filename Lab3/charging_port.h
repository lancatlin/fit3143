#pragma once

#include "context.h"
#include "random_generator.h"
#include <pthread.h>
#include <semaphore.h>
#include <stdbool.h>

/*
 * Data structure for a charging port
 * id: the port id
 * thread_id: the thread it is running
 * occupied: indicate if a port is in used
 */
typedef struct {
    int id;
    pthread_t thread_id;
    bool occupied;
} ChargingPort;

ChargingPort create_charging_port(int id);

/*
 * Parameters to start the port loop
 *
 * generator: A random generator to simulate how long a car stay
 * on_update (post-only): a semaphore to report the status change of the port
 * new_car (read-only): a semaphore to get new car
 * ctx: the Context object to stop the loop
 */
typedef struct {
    ChargingPort* port;
    RandomGenerator* generator;
    sem_t* on_update;
    sem_t* new_car;
    Context* ctx;
} StartPortParameter;

/* Run the port loop in the a new thread */
int start_port(StartPortParameter* params);

/* Wait for the port loop to end */
void join_port(ChargingPort* port);
