#pragma once

#include <pthread.h>
#include <semaphore.h>
#define PAYLOAD_SIZE 1
#define QUEUE_SIZE 10

/* Different tags to indicate message purpose */
#define TAG_NODE_AVAILABILITY_REQUEST 1
#define TAG_NODE_AVAILABILITY_RESPONSE 2

typedef struct {
    int id;
    int tag;
    int recipient;
    int buffer[PAYLOAD_SIZE];
} Message;

typedef struct {
    int next_id;
    int count;
    int cap;
    int read_head;
    int write_head;
    Message messages[QUEUE_SIZE];
    pthread_mutex_t mutex;
} MessageQueue;

void message_queue_init(MessageQueue* queue);

int message_queue_put(MessageQueue* queue, Message msg);

int message_queue_take(MessageQueue* queue, Message* msg);

void message_queue_destroy(MessageQueue* queue);