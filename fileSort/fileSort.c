#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct Node *node;
struct Node{
  void *data;
  struct Node *next;
};

int main(int argc, char* argv[]){
  //Read and open files
  FILE *fp;
  fp = fopen(argv[2],"r");
  char singleLine[100];
  const char s[2] = ",";
  char *token;

  if(!fp){
    printf("Cannot find file");
    return 0;
  }

  fgets(singleLine, 100, fp);

  printf(singleLine);
  printf("\n");

  token = strtok(singleLine, s);
node head = NULL;
  while( token != NULL ) {
        printf( " %s\n", token );
        addNode(head, token);

        token = strtok(NULL, s);
     }

  //Check command line arguments
  if(argv[1][0] == '-' && argv[1][1] == 'i'){
    printf("Insertion sort");
    // insertSort();
  }else if(argv[1][0] == '-' && argv[1][1] == 'q'){
    printf("Quick Sort");
    // quickSort();887
  }else{
    printf("Fatal error");
  }
}
node createNode(){
    node temp; // declare a node
    temp = (node)malloc(sizeof(node)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}
node addNode(node head, void *value){
    node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = value; // add element's value to data part of node
    if(head == NULL){
        head = temp;     //when linked list is empty
    }7
    else{
        p  = head;//assign head to p
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}
//
// int insertionSort( void* toSort, int (*comparator)(void*, void*)){
//
// }
// int quickSort( void* toSort, int (*comparator)(void*, void*) ){
//
// }
