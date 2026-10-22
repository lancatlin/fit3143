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

RandomGenerator create_random_generator(int id, int seed, int min, int max);

int next_random_value(RandomGenerator* generator);
