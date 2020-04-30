#include "WTF.h"


char* genLiveHash(char* fileName){
	struct stat buffer;
	stat(fileName, &buffer);

	char* hash;

	if(buffer.st_size == 0){
		hash = generateHash(fileName);
	} else {
		int contents = open(fileName, readFlag, mode);
		char* fileText = malloc(buffer.st_size*sizeof(char) + 1);
		memset(fileText, 0x0, buffer.st_size*sizeof(char) + 1);
		read(contents, fileText, buffer.st_size);
		close(contents);
		hash = generateHash(fileText);
		free(fileText);
	}

	return hash;
}
void compareManifestEntries(manEntry** clientEntry, int clientNumEntries, manEntry** serverEntry, int serversionEntries){
  int version = clientEntry[0]->version - serverEntry[0]->version;
  int i, j;
  char* liveHash;
  if(version == 0){
    int fd = open(".Update", newFlag, 0644);
    remove(".Conflict");
    printf("Up To Date");
    return;
  }
  int exist = 0;
  for(i = 0; i < clientNumEntries; i++){
    exist = 0;
    if(strstr(clientEntry[i]->name, ".manifest") == 0){
      for(j = 0; j < serversionEntries; j++){
        if(strcmp(clientEntry[i]->name, serverEntry[i]->name) == 0){
          exist = 1;
          liveHash = genLiveHash(clientEntry[i]->name);
          if(version != 0 && serverEntry[j]->version != clientEntry[i]->version &&
            strcmp(clientEntry[i]->hash, serverEntry[j]->hash) != 0){
  						clientEntry[i]->code = 'M';
  				}else if(version != 0 && strcmp(clientEntry[i]->hash,serverEntry[j]->hash) != 0 &&
          strcmp(clientEntry[i]->hash,liveHash) != 0 ){
            clientEntry[i]->code = 'C';
          }
        }
        //In client but not server
        if(!exist){
          clientEntry[i]->code = 'D';
        }
      }
    }
  }

  for(j = 0; j < serversionEntries; j++){
    exist = 0;
    if(strstr(serverEntry[i]->name, ".manifest") == 0){
    for(i = 0; i < clientNumEntries; i++){
        if(strcmp(clientEntry[i]->name, serverEntry[j]->name) == 0){
          exist = 1;
        }
      }
    }
    if(!exist){
      serverEntry[j]->code = 'A';
    }else{
      //Serves as a placeholder so theres no errors when writing to update file
      //Files that already been looked at in previous loop
      serverEntry[j]->code = 'i';
    }
  }
  //Serves as a placeholder so tehres no errors when writing to update file
  clientEntry[0]->code = 'i';
  serverEntry[0]->code  = 'i';
}
void writeUpdateFile(manEntry* entry, int fdUpdate, int fdConflict){
  if(entry->code != 'i'){
    char path[1090];
    memset(path, 0x0, 1090);
    char *filePath = realpath(entry->name, path);

    int filePathLength = strlen(filePath);
    int hashLength = strlen(entry->hash);
    char *text = malloc((strlen(filePath)+4+hashLength)*sizeof(char));

    sprintf(text, "%c,%s,%s\n", entry->code, filePath, entry->hash);
		printf("%s", text);

    if(entry->code == 'M'){
      write(fdUpdate, text, strlen(text));
    }else if(entry->code == 'A'){
      write(fdUpdate, text, strlen(text));
    }else if(entry->code == 'D'){
      write(fdUpdate, text, strlen(text));
    }else if(entry->code == 'C'){
      write(fdConflict, text, strlen(text));
    }
  }
}
char* createUpdateName(char* projectName){
	char* conflictPath = malloc(strlen(projectName)*2+10);
	memset(conflictPath, 0x0, strlen(projectName)*2+10);
	sprintf(conflictPath, "%s/%s.update", projectName, projectName);
	return conflictPath;
}
