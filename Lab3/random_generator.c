#include "random_generator.h"
#include <stdlib.h>

RandomGenerator create_random_generator(int id, int seed, int min, int max) {
    RandomGenerator generator = {
        seed + id,
        min,
        max,
    };
    return generator;
}

int next_random_value(RandomGenerator* generator) {
    int value = rand_r(&(generator->seed));
    return value % (generator->max - generator->min) + generator->min;
}
