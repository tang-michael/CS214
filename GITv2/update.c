void compareManifestEntries(manEntry* clientEntry, int clientNumEntries, manEntry* serverEntry, int serverNumEntries){
  int version = clientEntry[0]->version - server[0]->version;
  int i, j;
  char *liveHash;
  if(version == 0){
    int fd = open(".Update", newFlag, 0644);
    remove(".Conflict");
    printf("Up To Date");
    return;
  }
  int exist = 0;
  for(i = 0; i < clientNumEntries; i++){
    exist = 0;
    if(strstr(client[i]->name, ".manifest") == 0){
      for(j = 0; j < serverNumEntries; j++){
        if(strcmp(client[i]->name, server[i]->name) == 0){
          exist = 1;
          liveHash = genLiveHash(client[i]->name);
          if(version != 0 && server[j]->version != client[i]->version && strcmp(client[i]->hash, server[j]->hash) != 0){
  						client[i]->code = 'M';
  				}else if(version != 0 && strcmp(client[i]->hash,server[j]->hash) != 0 && strcmp(client[i]->hash,liveHash) != 0 ){
            client[i]->code = 'C';
          }
        }
        //In client but not server
        if(!exist){
          client[i]->code = 'D';
        }
      }
    }
  }

  for(j = 0; j < serverNumEntries; j++){
    exist = 0;
    if(strstr(server[i], ".manifest") == 0){
    for(i = 0; i < clientNumEntries; i++){
        if(strcmp(client[i]->name, server[j]->name) == 0){
          exist = 1;
        }
      }
    }
    if(!exist){
      server[j]->code = 'A';
    }else{
      //Serves as a placeholder so theres no errors when writing to update file
      //Files that already been looked at in previous loop
      server[j]->code = 'i';
    }
  }
  //Serves as a placeholder so tehres no errors when writing to update file
  clinet[0]->code = 'i';
  server[0]->code  = 'i;'
}
void writeUpdateFile(manEntry* entry, int fdUpdate, int fdConflict){
  if(entry->code != '-'){
    char path[1090];
    memset(path, 0x0, 1090);
    char *filepath = realpath(entry->name, path);

    int filePathLength = strlen(filePath);
    int hashLength = strlen(entry->hash);
    char *text = malloc((filePath+4+hashLength)*sizeof(char)+1);

    sprintf(output, "%c,%d,%s,%s\n", entry->code, entry->verNum, filePath, entry->hash);
		printf("%s", output);

    if(entry->code == 'M'){
      write(fdUpdate, output, strlen(output));
    }else if(entry->code == 'A'){
      write(fdUpdate, output, strlen(output));
    }else if(entry->code == 'D'){
      write(fdUpdate, output, strlen(output));
    }else if(entr->code == 'C'){
      wrote(fdConflict, output, strlen(output));
    }
  }
}
