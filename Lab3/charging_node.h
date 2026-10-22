#pragma once

#include "charging_port.h"
#include "context.h"

#define NBR_COUNT 4
#define UP 0
#define DOWN 1
#define LEFT 2
#define RIGHT 3
#define AVAILABILITY_BASELINE 2

/*
 * Neighbour represent the data of neighbours of a node
 *
 * rank: the MPI rank of the neighbour
 * availability: the current availability of the neighbour
 */
typedef struct {
    int rank;
    int availability;
} Neighbour;

/*
 * ChargingNode holds the data of a node
 *
 * id: Node identifier
 * port_count: the number of ports
 * thread_id: the thread id of the node main loop (auto set)
 * ports: the array of ports. Dynamic allocated with port_count size
 * neighbours: the array of neighbour data
 */
typedef struct {
    int id;
    int port_count;
    pthread_t thread_id;
    ChargingPort* ports;
    Neighbour neighbors[NBR_COUNT];
} ChargingNode;

/*
 * RunNodeParams is the parameter to start the node main thread
 */
typedef struct {
    ChargingNode* node;
    Context* ctx;
} RunNodeParams;

/* ChargingNode constructor */
ChargingNode create_charging_node(int id, int port_count);

/* ChargingNode destructor */
void free_charging_node(ChargingNode* node);

/*
 * ChargingNode main loop
 * It creates a new thread, stopped by context
 */
int run_node(RunNodeParams* params);

/* Returns the number of current available ports */
int available_ports(ChargingNode* node);
