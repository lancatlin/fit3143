#include "context.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

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
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += msec / 1000;
    ts.tv_nsec += (long)(msec % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    pthread_mutex_lock(&ctx->mutex);
    int result = pthread_cond_timedwait(&ctx->cond, &ctx->mutex, &ts);
    pthread_mutex_unlock(&ctx->mutex);
    return result;
}

void context_destroy(Context* ctx) {
    pthread_mutex_destroy(&ctx->mutex);
    pthread_cond_destroy(&ctx->cond);
}
