#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

typedef struct Node *node;
struct Node{
  void *data;
  struct Node *next;
};

node createNode(){
    node temp; // declare a node
    temp = (node)malloc(sizeof(node)); // allocate memory using malloc()
    temp->next = NULL;// make next point to NULL
    return temp;//return the new node
}
node addNode(node head, void* value){
    node temp,p;// declare two nodes temp and p
    temp = createNode();//createNode will return a new node with data = value and next pointing to NULL.
    temp->data = value; // add element's value to data part of node
    if(head == NULL){
        head = temp;     //when linked list is empty
    }
    else{
        p  = head;//assign head to p
        while(p->next != NULL){
            p = p->next;//traverse the list until p is the last node.The last node always points to NULL.
        }
        p->next = temp;//Point the previous last node to the new node created.
    }
    return head;
}

int main(int argc, char* argv[]){
  //Read and open files
  char buffer[100] = ""; //Buffer to read into
  int status = 1; //Status of read
  int bytesRead = 0; //Keep track of bytes read
  int fp = open(argv[2], O_RDONLY,"r"); //File descriptor
  while(status>0){ 
  status = read(fp,(&buffer[bytesRead]),1); //Read from the file and store in buffer
  bytesRead++;}

 int x = close(fp);
 const char s[2] = ",";
 char *token;

  if(!fp){
    printf("Cannot find file");
    return 0;
  }

  token = strtok(buffer, s);
  node head = NULL;
  while( token != NULL ) {
        printf( " %s\n", token );
        addNode(head, token);

        token = strtok(NULL, s);
     }

  //Check command line arguments
  //if(argv[1][0] == '-' && argv[1][1] == 'i'){
   // printf("Insertion sort");
     //insertSort();
 // }else if(argv[1][0] == '-' && argv[1][1] == 'q'){
  //   printf("Quick Sort");
     //quickSort();
 // }else{
  //  printf("Fatal error");
 // }
}

//
// int insertionSort( void* toSort, int (*comparator)(void*, void*)){
//
// }
// int quickSort( void* toSort, int (*comparator)(void*, void*) ){
//
// }