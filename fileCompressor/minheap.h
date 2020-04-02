
/*
* minheap.h
*
* Generic min heap implementation.
*/

#ifndef MINHEAP_H
#define MINHEAP_H

#include <stdlib.h>


/*
* Generic min heap.
*
* Example usage:
*
* int int_comparator(void *a, void *b) {
*     int x = (int)a, y = (int)b;
*     if (x == y) {
*         return 0;
*     } else if (x > y) {
*         return 1;
*     } else {
*         return -1;
*     }
* }
*
* MinHeap *heap = MH_create(3, int_comparator);
*
* MH_insert(heap, 2);
* MH_insert(heap, 1);
* MH_insert(heap, 3);
*
* int a = MH_pop(heap); // 1
* int b = MH_pop(heap); // 2
* int c = MH_pop(heap); // 3
*
* MH_destroy(heap);
*
*/
typedef struct minheap MinHeap;


/*
* Create a new min heap instance.
* Capacity determines the maximum number of elements the heap can hold.
* Comparator is the function used to determine which elements are smaller
* than others, and must return:
* - 1 if the first argument is greater than the second
* - 0 if the two arguments are equivalent
* - -1 if the first argument is less than the second
*/
MinHeap *MH_create(size_t capacity, int (*comparator)(void *, void *));


/*
* Deallocate the instance.
*/
void MH_destroy(MinHeap *heap);


/*
* Return the number of elements the heap currently contains.
*/
size_t MH_size(MinHeap *heap);


/*
* Insert an element into the heap.
*/
void MH_insert(MinHeap *heap, void *item);


/*
* Remove and return the smallest element from the heap.
*/
void *MH_pop(MinHeap *heap);

#endif
