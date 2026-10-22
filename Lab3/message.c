#include "message.h"
#include <pthread.h>
#include <stdio.h>

void message_queue_init(MessageQueue* queue) {
    queue->next_id = 1;
    queue->cap = QUEUE_SIZE;
    queue->count = 0;
    queue->write_head = 0;
    queue->read_head = 0;
    pthread_mutex_init(&queue->mutex, NULL);
}

int message_queue_put(MessageQueue* queue, Message msg) {
    if (queue->count >= queue->cap) {
        printf("Exceeding queue size: overwriting");
        return -1;
    }
    pthread_mutex_lock(&queue->mutex);
    // msg.id = queue->next_id;
    queue->messages[queue->write_head] = msg;
    queue->write_head = (queue->write_head + 1) % queue->cap;
    queue->count += 1;
    queue->next_id += 1;
    pthread_mutex_unlock(&queue->mutex);
    return msg.id;
}

int message_queue_take(MessageQueue* queue, Message* msg) {
    if (queue->count <= 0) {
        return -1;
    }
    pthread_mutex_lock(&queue->mutex);
    *msg = queue->messages[queue->read_head];
    queue->read_head = (queue->read_head + 1) % queue->cap;
    queue->count -= 1;
    pthread_mutex_unlock(&queue->mutex);
    return 0;
}

void message_queue_destroy(MessageQueue* queue) {
    pthread_mutex_destroy(&queue->mutex);
}