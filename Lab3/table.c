#include "table.h"
#include "message.h"
#include <bits/time.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>

void request_table_init(RequestTable* table) {
    table->next_id = 0;
    pthread_mutex_init(&table->mutex, NULL);
}

int request_table_send(RequestTable* table) {
    int id = table->next_id;
    int key = id % TABLE_SIZE;

    pthread_mutex_lock(&table->mutex);
    table->next_id += 1;
    table->message_id[key] = id;
    clock_gettime(CLOCK_REALTIME, &table->start_time[key]);
    pthread_mutex_unlock(&table->mutex);

    return id;
}

int request_table_received(RequestTable* table, int id) {
    int key = id % TABLE_SIZE;
    if (table->message_id[key] != id) {
        printf("Collision detected: %d != %d\n", table->message_id[key], id);
        return -1;
    }

    pthread_mutex_lock(&table->mutex);
    clock_gettime(CLOCK_REALTIME, &table->end_time[key]);
    pthread_mutex_unlock(&table->mutex);

    int sec = table->end_time[key].tv_sec - table->start_time[key].tv_sec;
    int nsec = table->end_time[key].tv_nsec - table->start_time[key].tv_nsec;
    int ms = sec * 1000 + nsec / 1000000;
    printf("message %d duration %d ms\n", id, ms);
    return 0;
}

void request_table_destroy(RequestTable* table) {
    pthread_mutex_destroy(&table->mutex);
}