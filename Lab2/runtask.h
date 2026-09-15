// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#pragma once

#include "slice.h"

int run_task(long n, LongSlice (*func)(long), char *label, int mpi_size);
