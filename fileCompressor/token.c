#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <errno.h>
#include <dirent.h>
#include <unistd.h>
#include <fcntl.h>

#include "DLL.h"

char * Get_string(char * file_name);

int main(int argc, char *argv[]){
  // int length = lseek(argv[1], 0, SEEK_END);
  // char * contents = malloc(length*sizeof(char)+1);
  char *contents;
  DLL *list = get_tokens(Get_string("test.txt"));
  printList(list);
  return 0;
}

// Converts file into string
char * toString(char * file_name){
    char * input = file_name;
    int file = open(input, O_RDONLY);
    if (file==-1){
        //printf("%d: %s\n", errno, strerror(errno));
        return NULL;
      }
    // Gets length of file for malloc
    int length = lseek(file, 0, SEEK_END);
    lseek(file,0, SEEK_SET);
    char * contents = malloc(length*sizeof(char)+1);
    // Check if file read is success
    int success = read(file, contents, length);
    if (success == -1){
      // If not success close and free memory contents 
        free(contents);
        close(file);
        return NULL;
    }
    close(file);
    contents[length]='\0';
    return contents;

}
// Converts string into tokens and stores it into DLL
DLL * string_to_tokens( char * string){
    char * input = string;
    // Create DLL
    DLL * list = DLLCreate();
    int i,j;
    i=0;
    while(i<strlen(input)){
        // Check if whitespace or control character
        if ((!iscntrl(input[i]) && !isspace(input[i]))){
            //Start where there is no whitespace
            j=i;
            //Increment untill we find whitespace/delimiter
            while (!iscntrl(input[i]) && !isspace(input[i]) && i<strlen(input)){
                i++;
            }
            // Size of token
            char * new_token= malloc(sizeof(char)* (i-j+2));
            // Copies string from last blank space to end of word
            // Input+j=last blank space
            strncpy(new_token,input+j,i-j+1);
            new_token[i-j]='\0';
            //Insert into DLL
            DLLinsert(list,new_token);
        }else {
            // If whitespace, still store it as a token
            char * delimiter = malloc(sizeof(char)*2);
            strncpy(delimiter, input+i,1);
            delimiter[1] = '\0';
            // Insert into DLL
            DLLinsert(list,delimiter);
            // Increment i
            i++;
        }

    }
    //Returns list of tokens
    return list;
}
