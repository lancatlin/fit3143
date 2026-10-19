#include "utils.h"
#include <time.h>

int wait_cond_or_sec(pthread_mutex_t* mutex, pthread_cond_t* cond, int sec) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += sec;
    pthread_mutex_lock(mutex);
    int result = pthread_cond_timedwait(cond, mutex, &ts);
    pthread_mutex_unlock(mutex);
    return result;
}
