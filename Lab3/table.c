#include "table.h"
#include "message.h"
#include <bits/time.h>
#include <stdio.h>
#include <time.h>

void request_table_init(RequestTable* table) {
    table->next_id = 0;
}

int request_table_send(RequestTable* table, Message* msg) {
    msg->id = table->next_id;
    table->next_id += 1;
    int key = msg->id % TABLE_SIZE;
    table->message_id[key] = msg->id;
    clock_gettime(CLOCK_REALTIME, &table->start_time[key]);
    return 0;
}

int request_table_received(RequestTable* table, int id) {
    int key = id % TABLE_SIZE;
    if (table->message_id[key] != id) {
        perror("Collision detected");
        return -1;
    }
    clock_gettime(CLOCK_REALTIME, &table->end_time[key]);
    int sec = table->end_time[key].tv_sec - table->start_time[key].tv_sec;
    int nsec = table->end_time[key].tv_nsec - table->start_time[key].tv_nsec;
    int ms = sec * 1000 + nsec / 1000000;
    printf("message %d duration %d ms\n", id, ms);
    return 0;
}