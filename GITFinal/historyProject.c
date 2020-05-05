/*
 * historyProject.c
 *
 *  Created on: 2020年4月29日
 *      Author: mfj
 */
 #include "utils.h"

 void clientHistory(char* projectName){
   //1 send "historyy" to server, then send projectName to server
 	sendTo(SERVER, "history");
 	sendTo(SERVER, projectName);


  char* historyFile = malloc(strlen(".history"));
  sprintf(historyFile, "%s/.history", projectName);
  receive_file(SERVER ,historyFile);

  int openHistory = open(historyFile, readFlag, 0644);
  char c;
  char *text;
  while(read(openHistory, &c, 1) != 0){
    printf("%c", c);
  }

  remove(historyFile);
 }
 void serverHistory(int fd){
   char* projectName = getFrom(fd);
   char* historyPath = malloc(strlen(projectName)+9);
   sprintf(historyPath, "%s/.history", projectName);

   send_file(fd, historyPath);
 }
