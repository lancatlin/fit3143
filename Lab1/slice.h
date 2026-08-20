// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#pragma once

typedef struct {
    int *arr;
    int len;
    int cap;
} IntSlice;

IntSlice make_slice(int len, int cap);

void append_slice(IntSlice *slice, int value);

void free_slice(IntSlice *slice);
