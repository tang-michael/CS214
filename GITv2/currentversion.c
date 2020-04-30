#include "WTF.h"

void printCurrentVersion(manEntry** contents, int numEntries){
  int i;
  for(i  = 0; i < numEntries; i++){
    printf("%s%i\n",contents[i]->name, contents[i]->version );
  }
}
