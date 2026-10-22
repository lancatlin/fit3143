#include "context.h"
#include "message.h"
#include "table.h"
#include "time_helper.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int context_init(Context* ctx) {
    if (pthread_mutex_init(&ctx->mutex, NULL) != 0) {
        perror("Failed to init mutex");
        return -1;
    }
    if (pthread_cond_init(&ctx->cond, NULL) != 0) {
        perror("Failed to init cond");
        return -1;
    }
    atomic_init(&ctx->stopped, NULL);
    message_queue_init(&ctx->queue);
    request_table_init(&ctx->table);
    return 0;
}

int context_shutdown(Context* ctx) {
    pthread_mutex_lock(&ctx->mutex);
    atomic_store(&ctx->stopped, true);
    pthread_cond_broadcast(&ctx->cond);
    pthread_mutex_unlock(&ctx->mutex);
    return 0;
}

bool context_running(Context* ctx) {
    return !atomic_load(&ctx->stopped);
}

int context_wait(Context* ctx, int msec) {
    if (!context_running(ctx)) {
        return 0;
    }
    pthread_mutex_lock(&ctx->mutex);
    struct timespec ts = get_future(msec);
    int result = pthread_cond_timedwait(&ctx->cond, &ctx->mutex, &ts);
    pthread_mutex_unlock(&ctx->mutex);
    return result;
}

void context_destroy(Context* ctx) {
    pthread_mutex_destroy(&ctx->mutex);
    pthread_cond_destroy(&ctx->cond);
    message_queue_destroy(&ctx->queue);
    request_table_destroy(&ctx->table);
}
