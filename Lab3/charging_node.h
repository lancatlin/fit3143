#pragma once

#include "charging_port.h"

#define NBR_COUNT 4
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3

typedef struct {
    int id;
    int port_count;
    ChargingPort* ports;
    int neighbors[NBR_COUNT];
} ChargingNode;

ChargingNode create_charging_node(int id, int port_count);

void free_charging_node(ChargingNode* node);

int run_node(ChargingNode* node, int duration);

int available_ports(ChargingNode* node);
