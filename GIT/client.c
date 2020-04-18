#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#include "util.h"


int main(int argc, char *argv[]) {
   int sockfd, portno, n;
   struct sockaddr_in serv_addr;
   struct hostent *server;
   char buffer[255];

   if(strcmp(argv[1], "configure") == 0){
    int file = open(".Configure", O_WRONLY | O_CREAT | O_TRUNC, 00600);
    int length1 = strlen(argv[2]);
    int length2 = strlen(argv[3]);
    write(file,argv[2], length1);
    write(file," ", 1);
    write(file,argv[3], length2);
    /*
    ADD ERROR CHECKING HERE
    FOR WRITING FILES
    */
  }else if(strcmp(argv[1], "configure") != 0){
    if(open(".Configure", O_RDONLY) < 0){
      printf("Can't find file");
      return 0;
    }else{
      char *contents = toString(".Configure");
      int i = 0;
      char word[255] = {0};
      char *name;
      while(i < strlen(contents)){
        if(isspace(contents[i])){
          name = word;
          printf("%s\n", name);
          memset(word,0,sizeof(word));
          i++;
          continue;
        }
        strncat(word, &contents[i], 1);
        i++;
      }
        printf("name = %s\n",  name);
        printf("portno = %s\n", word);
    }
  }
    // portno = atoi(argv[2]);
   // sockfd = socket(AF_INET, SOCK_STREAM, 0);
   //
   // if (sockfd < 0) {
   //    perror("ERROR opening socket");
   //    exit(1);
   // }
   //
   // server = gethostbyname(argv[1]);
   //
   // if (server == NULL) {
   //    fprintf(stderr,"ERROR, no such host\n");
   //    exit(0);
   // }
   //
   // bzero((char *) &serv_addr, sizeof(serv_addr));
   // serv_addr.sin_family = AF_INET;
   // bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
   // serv_addr.sin_port = htons(portno);
   //
   // /* Now connect to the server */
   // if (connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0) {
   //    perror("ERROR connecting");
   //    exit(1);
   // }
   //
   // if (n < 0) {
   //    perror("ERROR writing to socket");
   //    exit(1);
   // }
   //
   // /* Now read server response */
   // bzero(buffer,256);
   // n = read(sockfd, buffer, 255);
   //
   // if (n < 0) {
   //    perror("ERROR reading from socket");
   //    exit(1);
   // }
   //
   // printf("%s\n",buffer);
   return 0;
}
