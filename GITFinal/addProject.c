#include "utils.h"
#include "addProject.h"
#include "manifest.h"

void addFile(char*projectName, char* fileName)
{
  // char* filePath = malloc(strlen(projectName) + strlen(fileName)+1);
  // sprintf(filePath, "%s/%s", projectName, fileName);
  // printf("%s\n", filePath);
  // if(open(filePath, O_RDONLY, 0644) < 0){
  //   printf("File does not exist in project\n");
  //   exit(0);
  // }
  // int versionNumber;
  // get_version_number(projectName , &versionNumber);
  // manEntry* newFile = newManEntry(fileName, versionNumber);
  // char* manifestFilePath = generate_manifest_name(projectName, versionNumber);
  // int contents = open(manifestFilePath, addFlag, mode);
  // updateManEntry(newFile, contents);
  // close(contents);
  // freeManEntry(newFile);
}
