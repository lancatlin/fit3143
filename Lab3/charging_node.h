#pragma once

#include "charging_port.h"

typedef struct {
    int id;
    int port_count;
    ChargingPort* ports;
} ChargingNode;

ChargingNode create_charging_node(int id, int port_count);

void free_charging_node(ChargingNode* node);
