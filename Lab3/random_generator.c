#include "random_generator.h"
#include <stdlib.h>
#define SEED_MULTIPLIER 11234

RandomGenerator create_random_generator(int id, int seed, int min, int max) {
    RandomGenerator generator = {
        seed * SEED_MULTIPLIER + id,
        min,
        max,
    };
    return generator;
}

int next_random_value(RandomGenerator* generator) {
    int value = rand_r(&(generator->seed));
    return value % (generator->max - generator->min) + generator->min;
}
