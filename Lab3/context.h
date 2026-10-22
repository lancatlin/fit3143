#pragma once

#include "message.h"
#include "table.h"
#include <pthread.h>
#include <stdatomic.h>
#include <stdbool.h>

#define SECOND 1000

/*
 * Context is a data structure to gracefully stop multiple thread
 * It allow clients to use pthread_cond_timedwait to sleep,
 * with respect to program stop at the same time
 *
 * Note: Context MUST NOT BE COPIED, always pass by pointer
 */
typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    atomic_bool stopped;
    MessageQueue queue;
    RequestTable table;
} Context;

/* initialise a Context */
int context_init(Context* ctx);

/* Send shutdown signal to all listeners */
int context_shutdown(Context* ctx);

/* Return if the context is still running */
bool context_running(Context* ctx);

/* Waits up to msec milliseconds. Returns 0 if the context is stopped. */
int context_wait(Context* ctx, int msec);

/* Destroy a context */
void context_destroy(Context* ctx);
