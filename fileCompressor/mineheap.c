#include <stdlib.h>

#include "minheap.h"
#include "util.h"


struct minheap {
    // Max elements in hea[]
    size_t capacity;
    // Elements in heap
    size_t size;
    // Function pointer to compare items in heap
    int (*comparator)(void *, void *);
    // Contents in the heap
    void **items;
};

// Creates default minheap with nothing inside
MinHeap *minHeap_create(size_t capacity, int (*comparator)(void *, void*)) {
    MinHeap *heap = malloc(sizeof(MinHeap));
    if (heap == NULL) {
        return NULL;
    }
    heap->capacity = capacity;
    heap->size = 0;
    heap->comparator = comparator;
    heap->items = malloc(capacity * sizeof(void *));
    return heap;
}

// Destroys heap
void minHeap_destroy(MinHeap *heap) {
    free(heap->items);
    free(heap);
}

//Returns size
size_t minHeap_size(MinHeap *heap) {
    return heap->size;
}

int minHeap_parent(int i) {
    return (i - 1) / 2;
}

int minHeap_left_child(int i) {
    return 2 * i + 1;
}


int minHeap_right_child(int i) {
    return 2 * i + 2;
}


// Sift up algorithm for heap to rearrange heap into proper order
void siftUp(MinHeap *heap) {
    int i = MH_size(heap) - 1;
    int parent;

    while (i > 0) {
        parent = minHeap_parent(i);

        if (heap->comparator(heap->items[i], heap->items[parent]) <= 0) {
            void *temp = heap->items[parent];
            heap->items[parent] = heap->items[i];
            heap->items[i] = temp;
            i = parent;
        } else {
            break;
        }
    }
}


// Sift down algorithm to order elements properly
void siftDown(MinHeap *heap) {
    int i = 0;
    int child;

    while (minHeap_left_child(i) < minHeap_size(heap)) {
        int left = minHeap_left_child(i), right = minHeap_right_child(i);

        if (right < minHeap_size(heap)) {
            // Node has 2 children, determine min child
            int cmp = heap->comparator(heap->items[left], heap->items[right]);
            child = cmp <= 0 ? left : right;
        } else {
            child = left;
        }

        if (heap->comparator(heap->items[child], heap->items[i]) <= 0) {
            void *temp = heap->items[child];
            heap->items[child] = heap->items[i];
            heap->items[i] = temp;
            i = child;
        } else {
            break;
        }
    }
}

// Insert item into heap
void minHeap_insert(MinHeap *heap, void *item) {
    if (minHeap_size(heap) == heap->capacity) {
        error("----- Heap full -----");
    }

    heap->items[heap->size++] = item;
    siftUp(heap);
}

// Remove element from heap
void *minHeap_pop(MinHeap *heap) {
    if (minHeap_size(heap) == 0) {
        error("----- Empty heap -----");
    }

    void *item = heap->items[0];
    heap->items[0] = heap->items[--heap->size];
    siftDown(heap);
    return item;
}
