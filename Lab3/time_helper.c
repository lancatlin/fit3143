#include "time_helper.h"

struct timespec get_future(int ms) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    ts.tv_sec += ms / 1000;
    ts.tv_nsec += (long)(ms % 1000) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    return ts;
}

unsigned long get_duration_ms(struct timespec start, struct timespec end) {
    int sec = end.tv_sec - start.tv_sec;
    int nsec = end.tv_nsec - start.tv_nsec;
    int ms = sec * 1000 + nsec / 1e6;
    return ms;
}
