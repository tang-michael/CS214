#ifndef WTF_H
#define WTF_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <math.h>
#include <dirent.h>
#include <ctype.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <dirent.h>
#include <errno.h>

#include <openssl/sha.h>

int MANIFEST_ENTRIES;

char* IP;
int PORT;
static int newFlag = O_WRONLY | O_CREAT;
static int readFlag = O_RDONLY;
static int addFlag = O_RDWR | O_APPEND;
static int remFlag = O_RDWR | O_TRUNC;
static mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH;
int SERVER;

typedef struct node{
    char* data;
    struct node* next;
} node;


typedef struct _manEntry {
	int verNum;
	char* name;
	char* hash;
	char code;
} manEntry;

//Basic Manifest Functions
char* createManifestName(char* projectName);
char* generateHash(char* inputText);
manEntry* newManEntry(char* fileName);
void updateManEntry(manEntry* entry);
void writeManEntry(manEntry* entry, int fileDescriptor);
manEntry* getManEntry(char* rawText, int trailer);
void freeManifest(manEntry** manifest);
void freeManEntry(manEntry* entry);

//Core client Functions
void createProject(char* projectName);
void addFile(char* projectName, char* fileName);
void removeFile(char* projectName, char* fileName);
void destroyProject(char* projectName);
void updateProject(char* projectName);
void upgradeProject(char* projectName);
void commitProject(char* projectName);
void pushProject(char* projectName);

// functions for the connection
void connect_server (char * Ip, int port);
void readConfigure();
void configure ( char* IPAddress, char* port );

#endif
