#pragma once

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#define SECOND 1000

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    atomic_bool stopped;
} Context;

int context_init(Context* ctx);

int context_shutdown(Context* ctx);

bool context_running(Context* ctx);

// Waits up to msec milliseconds. Returns 0 if the context is stopped.
int context_wait(Context* ctx, int msec);

void context_destroy(Context* ctx);
