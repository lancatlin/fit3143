#pragma once

#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    atomic_bool stopped;
} Context;

int context_init(Context* ctx);

int context_shutdown(Context* ctx);

bool context_running(Context* ctx);

int context_wait(Context* ctx, int duration);

void context_destroy(Context* ctx);
