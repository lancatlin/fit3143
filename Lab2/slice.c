// Justin Lin       35808217    hlin0094@student.monash.edu
// Clement Angelo   35664614    cang0028@student.monash.edu
#include "slice.h"
#include <stdlib.h>

const int GROWTH_RATE = 2;

// Returns an IntSlice
IntSlice make_slice(int len, int cap) {
    if (len > cap) {
        cap = len;
    }
    int *arr = (int *)malloc(sizeof(int) * cap);
    for (int i = 0; i < len; i++) {
        arr[i] = 0;
    }
    IntSlice slice = {arr, len, cap};
    return slice;
}

// Appends given value to Intslice
void append_slice(IntSlice *slice, int value) {
    if (slice->len + 1 > slice->cap) {
        int cap = (GROWTH_RATE * slice->cap);
        slice->arr = realloc(slice->arr, sizeof(int) * cap);
        slice->cap = cap;
    }
    slice->arr[slice->len] = value;
    slice->len += 1;
}

// Free memory used by Intslice
void free_slice(IntSlice *slice) {
    free(slice->arr);
    slice->arr = NULL;
    slice->len = 0;
    slice->cap = 0;
}

// Returns an IntSlice
LongSlice make_slice_long(long len, long cap) {
    if (len > cap) {
        cap = len;
    }
    long *arr = (long *)malloc(sizeof(long) * cap);
    for (int i = 0; i < len; i++) {
        arr[i] = 0;
    }
    LongSlice slice = {arr, len, cap};
    return slice;
}

// Appends given value to Intslice
void append_slice_long(LongSlice *slice, long value) {
    if (slice->len + 1 > slice->cap) {
        long cap = (GROWTH_RATE * slice->cap);
        slice->arr = realloc(slice->arr, sizeof(long) * cap);
        slice->cap = cap;
    }
    slice->arr[slice->len] = value;
    slice->len += 1;
}

// Free memory used by Intslice
void free_slice_long(LongSlice *slice) {
    free(slice->arr);
    slice->arr = NULL;
    slice->len = 0;
    slice->cap = 0;
}
