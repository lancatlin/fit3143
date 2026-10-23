#pragma once

#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#define MAX_THREADS 10

typedef struct {
    int num_of_nodes;
    FILE* log_file;
    int row_number;
    int column_number;
    bool is_node_available[];
} BaseStation;

BaseStation* create_base_station(int mpi_size, int dims_no, int* dims);

void free_base_station(BaseStation* baseStation);

typedef struct {
    int nodeRank;
    int special_arguement; // this value's meaning depends on the MPI tag it is
                           // sent with
    double timeReceived;
} NodeReport;

typedef struct {
    NodeReport* report;
    BaseStation* baseStation;
    int mpi_send_dest;
    atomic_bool* has_ended;
    int mpi_flag;
} ThreadArgs;

void* respond_message(void* arg);

int run_base_station(BaseStation* baseStation);

void _get_neighbour(int curr, int row_number, int column_number, int res[4]);

void* respond_message(void* arg);
