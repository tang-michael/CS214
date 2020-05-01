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
#include <ifaddrs.h>

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
int CLIENT_ADDR;

typedef struct node{
    char* data;
    struct node* next;
} node;


typedef struct _manEntry {
	int version;
	char* name;
	char* hash;
	char code;
} manEntry;

void printCurrentVersion(manEntry **contents, int numEntries);

char* createUpdateName(char* projectName);
char* createConflictName(char* projectName);



//Basic Manifest Functions
char* createManifestName(char* projectName);
char* createManifestPath(char* projectName);
char* generateHash(char* inputText);
manEntry* newManEntry(char* fileName);
manEntry* newManEntryWithPath(char* fileName, char *filePath);
void updateManEntry(manEntry* entry, char* filePath);
void writeManEntry(manEntry* entry, int fileDescriptor);
manEntry* extractEntry(char* text, int trailer);
manEntry* getManEntry(char* rawText, int trailer);
manEntry** readManifest(char* manifestName);
void freeManifest(manEntry** manifest);
void freeManEntry(manEntry* entry);

//Core client Functions
void createProject(char* projectName);
void addFile(char* projectName, char* fileName);
void removeFile(char* projectName, char* fileName);
void destroyProject(char* projectName);
void updateProject(char* projectName);
void upgradeProject(char* projectName);
char* createFilePath(char* projectName, char* fileName);
void commitProject(char* projectName);
void pushProject(char* projectName);
void clientCurrentVersion(char *projectName);
void clientUpdate(char *projectName);


// functions for the connection
void connect_server (char * Ip, int port);
void readConfigure();
void configure ( char* IPAddress, char* port );

//Server functions
// void serverCommit(int fd);
// void serverPush(int fd);
void serverDestroy(int fd);
void serverCreate(int fd);
// void serverUpgrade(int fd);
void serverUpdate(int fd);




#endif
