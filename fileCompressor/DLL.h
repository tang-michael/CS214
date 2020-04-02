/*
 * Doubly Linked List Implementation
 *
 *
 */

#ifndef DLL_H
#define DLL_H


typedef struct DLL_node{
     struct DLL_node *prev, *next;
     char * data;
} DLL_node;

typedef struct   {
    DLL_node *head, *tail;
    int size;
} DLL;


/*
 * Insert item into DLL
 */
void DLLinsert (DLL * list, char * data);

/*
 * Delete item from list
 */
void DLLdelete( DLL * list, DLL_node * data);

/*
 * Create new list
 */
DLL * DLLCreate();

/*
 * Deletes DLL list
 *
 */
void DLLdestroy(DLL* list);


#endif
