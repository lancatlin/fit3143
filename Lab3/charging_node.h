#pragma once

#include "charging_port.h"
#include "context.h"

#define NBR_COUNT 4
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define AVAILABILITY_BASELINE 4

typedef struct {
    int rank;
    int availability;
} Neighbour;

typedef struct {
    int id;
    int port_count;
    pthread_t thread_id;
    ChargingPort* ports;
    Neighbour neighbors[NBR_COUNT];
} ChargingNode;

typedef struct {
    ChargingNode* node;
    Context* ctx;
} RunNodeParams;

ChargingNode create_charging_node(int id, int port_count);

void free_charging_node(ChargingNode* node);

int run_node(RunNodeParams* params);

int available_ports(ChargingNode* node);
