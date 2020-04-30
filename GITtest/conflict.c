#include "WTF.h"

char* createConflictName(char* projectName){
	char* conflictPath = malloc(strlen(projectName)*2+10);
	memset(conflictPath, 0x0, strlen(projectName)*2+10);
	sprintf(conflictPath, "%s/%s.conflict", projectName, projectName);
	return conflictPath;
}
