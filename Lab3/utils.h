#pragma once

#include <pthread.h>

/* Return 0 if condition is met */
int wait_cond_or_sec(pthread_mutex_t* mutex, pthread_cond_t* cond, int sec);
