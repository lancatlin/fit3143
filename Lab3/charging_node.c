#include "charging_node.h"
#include "charging_port.h"
#include <stdlib.h>

ChargingNode create_charging_node(int id, int port_count) {
    ChargingPort* ports = malloc(sizeof(ChargingPort) * port_count);
    for (int i = 0; i < port_count; i++) {
        ports[i] = create_charging_port(i);
    }
    ChargingNode node = {id, port_count, ports};
    return node;
}

void free_charging_node(ChargingNode* node) {
    free(node->ports);
    node->ports = NULL;
    node->port_count = 0;
}
