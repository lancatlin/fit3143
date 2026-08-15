#pragma once

typedef struct {
  int *arr;
  int len;
  int cap;
} IntSlice;

IntSlice make_slice(int len, int cap);

void append_slice(IntSlice *slice, int value);

void free_slice(IntSlice *slice);
