#pragma once

/*
 * A thread-safe random number generator
 * Each thread should use a different generator
 *
 * seed: the seed to generate number (updated by rand_r)
 * min, max: min and max return value
 */
typedef struct {
    unsigned int seed;
    int min;
    int max;
} RandomGenerator;

/* Create a new RandomGenerator, use id and seed to generate seed */
RandomGenerator create_random_generator(int id, int seed, int min, int max);

/* Get the next random value (mutating the seed) */
int next_random_value(RandomGenerator* generator);
