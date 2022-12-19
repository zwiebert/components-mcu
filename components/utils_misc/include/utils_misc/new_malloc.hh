#pragma once

#include <malloc.h>

void *operator new(size_t size) {
  return malloc(size);
}

void operator delete(void *p) {
  free(p);
}


void operator delete(void *p, size_t) {
  free(p);
}
