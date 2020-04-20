#include <errno.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>

#include "util.h"


int write_string(int fd, char *s) {
    int to_write = strlen(s);
    int written = 0;
    int status;

    do {
        status = write(fd, s + written, to_write);
        written += status;
        to_write -= status;
    } while (to_write > 0 && status > 0);

    return status != -1;
}
char * toString(char * file_name){
    char * input = file_name;
    int file = open(input, O_RDONLY);
    if (file==-1){
        //printf("%d: %s\n", errno, strerror(errno));
        return NULL;
    }
    int length = lseek(file, 0, SEEK_END);
    lseek(file,0, SEEK_SET);
    char * contents = malloc(length*sizeof(char)+1);
    int status = read(file, contents, length);
    if (status == -1){
      //  printf("%d: %s\n", errno, strerror(errno));
        free(contents);
        close(file);
        return NULL;
    }
    //free(contents);
    close(file);
    contents[length]='\0';
    return contents;

}
