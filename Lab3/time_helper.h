#pragma once

#include <time.h>

struct timespec get_future(int ms);

unsigned long get_duration_ms(struct timespec start, struct timespec end);
