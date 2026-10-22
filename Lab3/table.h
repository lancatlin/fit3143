#pragma once

#include "message.h"
#include <pthread.h>
#include <time.h>
#define TABLE_SIZE 100

typedef struct {
    int next_id;
    int message_id[TABLE_SIZE];
    struct timespec start_time[TABLE_SIZE];
    struct timespec end_time[TABLE_SIZE];
    pthread_mutex_t mutex;
} RequestTable;

void request_table_init(RequestTable* table);

int request_table_send(RequestTable* table);

int request_table_received(RequestTable* table, int id);

void request_table_destroy(RequestTable* table);
