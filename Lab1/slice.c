#include "slice.h"
#include <stdio.h>
#include <stdlib.h>

const int GROWTH_RATE = 2;

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

void append_slice(IntSlice *slice, int value) {
  if (slice->len + 1 > slice->cap) {
    int cap = (GROWTH_RATE * slice->cap);
    int *arr = (int *)malloc(sizeof(int) * cap);
    for (int i = 0; i < slice->len; i++) {
      arr[i] = slice->arr[i];
    }
    free(slice->arr);
    slice->arr = arr;
    slice->cap = cap;
  }
  slice->arr[slice->len] = value;
  slice->len += 1;
}

void free_slice(IntSlice *slice) {
  free(slice->arr);
  slice->arr = NULL;
  slice->len = 0;
  slice->cap = 0;
}
