#pragma once

#include "slice.h"

int run_task(int argc, char *argv[], IntSlice (*func)(int));
