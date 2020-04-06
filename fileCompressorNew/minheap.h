#ifndef MINHEAP_H
#define MINHEAP_H

#include <stdlib.h>

typedef struct minheap MinHeap;

MinHeap *minHeap_create(size_t capacity, int (*comparator)(void *, void *));

void minHeap_destroy(MinHeap *heap);

size_t minHeap_size(MinHeap *heap);

void minHeap_insert(MinHeap *heap, void *item);

void *minHeap_pop(MinHeap *heap);

#endif
