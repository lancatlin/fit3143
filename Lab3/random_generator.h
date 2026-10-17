#pragma once

typedef struct {
    unsigned int seed;
    int min;
    int max;
} RandomGenerator;

RandomGenerator create_random_generator(int id, int seed, int min, int max);

int next_random_value(RandomGenerator* generator);
