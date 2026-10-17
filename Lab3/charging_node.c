#include "charging_node.h"
#include "charging_port.h"
#include "random_generator.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

ChargingNode create_charging_node(int id, int port_count) {
    ChargingPort* ports = malloc(sizeof(ChargingPort) * port_count);
    for (int i = 0; i < port_count; i++) {
        ports[i] = create_charging_port((1 + id) * 100 + i);
    }
    ChargingNode node = {id, port_count, ports};
    return node;
}

void free_charging_node(ChargingNode* node) {
    free(node->ports);
    node->ports = NULL;
    node->port_count = 0;
}

void run_node(ChargingNode* node, int duration) {
    RandomGenerator* generators =
        malloc(sizeof(RandomGenerator) * node->port_count);
    int seed = node->id;
    for (int i = 0; i < node->port_count; i++) {
        generators[i] = create_random_generator(i, seed, 1, 10);
        start_port(&node->ports[i], &generators[i]);
    }
    printf("Node %d started\n", node->id);

    sleep(duration);

    printf("Node %d shutting down\n", node->id);

    // Send stop signal to all ports
    for (int i = 0; i < node->port_count; i++) {
        stop_port(&node->ports[i]);
    }

    // join all threads
    for (int i = 0; i < node->port_count; i++) {
        join_port(&node->ports[i]);
    }
}
