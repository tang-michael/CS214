#ifndef UTILS_H_
#define UTILS_H_

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


char* IP;
int PORT;

static int newFlag = O_WRONLY | O_CREAT;
static int readFlag = O_RDONLY;
static int addFlag = O_RDWR | O_APPEND;
static int remFlag = O_RDWR | O_TRUNC;
static mode_t mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IROTH;
int SERVER;

void error(const char *msg);
void sendTo(int fd, char *contents);
char *getFrom(int fd);
char* generate_manifest_name(char *projectName, int versionNumber);
void save_version_number(char *projectName , int *version);
void get_version_number(char *projectName , int *version);
int judge_file_exists(char *fileName);
void connect_server (char * Ip, int port);
void send_file(int fd, char * path);
void receive_file(int fd, char * path);
void copy_file(char *path, char * newPath);
void save_direct_index(char *pathName, int fd);
void save_file_index(char *pathName, int fd);
void delete_direct_and_file(char *pathName);
void mkdir_direct_tree(char *projectDir, char *directIndexfile);
char *truncate_version_number(char *fileName, int versionNumber);
int return_file_size(char *fileName);
char* generate_direct_index_name(char *projectName, int versionNumber);

#endif /* UTILS_H_ */
