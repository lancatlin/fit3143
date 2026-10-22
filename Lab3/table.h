#pragma once

#include "message.h"
#include <time.h>
#define TABLE_SIZE 100

typedef struct {
    int next_id;
    int message_id[TABLE_SIZE];
    struct timespec start_time[TABLE_SIZE];
    struct timespec end_time[TABLE_SIZE];
} RequestTable;

void request_table_init(RequestTable* table);

int request_table_send(RequestTable* table, Message* msg);

int request_table_received(RequestTable* table, int id);
