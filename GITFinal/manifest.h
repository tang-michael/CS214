#ifndef MANIFEST_H_
#define MANIFEST_H_

#include "utils.h"

//Manifest Functions
char* createManifestName(char* projectName);
char* generateHash(char* inputText);
manEntry* newManEntry(char* fileName, int version);
void updateManEntry(manEntry* entry);
void writeManEntry(manEntry* entry, int fileDescriptor);
manEntry* getManEntry(char* rawText, int trailer);
void freeManEntry(manEntry* entry);
manEntry **readManifest(char *manifestName, int* size);
manEntry* extractEntries(char* rawText, int trailer);
void freeManifest(manEntry** manifest, int *size);
void recursive_newManEntry(int fd, char *pathName, int version);
void compareManifestEntries(manEntry** clientEntry, int clientNumEntries, manEntry** serverEntry, int serverNumEntries);
#endif /* MANIFEST_H_ */
