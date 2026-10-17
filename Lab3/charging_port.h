#pragma once

#include <stdbool.h>

typedef struct {
    int id;
    bool occupied;
} ChargingPort;

ChargingPort create_charging_port(int id);
