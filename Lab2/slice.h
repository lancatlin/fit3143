// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#pragma once

typedef struct {
    int *arr; // Pointer to an array of ints
    int len;  // the number of ints in the array
    int cap;  // the maximum capacity of array
} IntSlice;

// Returns an IntSlice
IntSlice make_slice(int len, int cap);

// Appends given value to Intslice
void append_slice(IntSlice *slice, int value);

// Free memory used by Intslice
void free_slice(IntSlice *slice);
