#include "WTF.h"

void serverCreate(int fd){
	 //Buffer read
    int input_buffer;
    read(fd,&input_buffer,sizeof(int));

    //Gets project name
    char* projectName = malloc(input_buffer*sizeof(char)+1);
    read(fd,projectName,input_buffer);

    //Make directory
    if(mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
        printf("Unable to create the project\n");
    }
    //Creates manifest
  char* manifestName = malloc(strlen(projectName)*2+10);
  sprintf(manifestName, "%s/%s.manifest", projectName, projectName);
	int contents = open(manifestName, newFlag, mode);
  //Create manfiest entry for new project
	manEntry* newProject = newManEntry(manifestName);
  //Add to manifest file
	writeManEntry(newProject, contents);
	close(contents);
	free(manifestName);
	freeManEntry(newProject);
}

void serverDestroy(int fd){
	//Read project Name
    int input_buffer;
    read(fd,&input_buffer,sizeof(int));
    char* project_name = malloc(input_buffer*sizeof(char));
    read(fd,project_name,input_buffer);
    printf("%s\n", project_name);

    DIR* project = opendir(project_name);
    struct dirent* file;

    readdir(project);
    readdir(project);

    char* folderName = malloc(strlen(project_name)+2);
    memset(folderName,0x0,strlen(project_name)+2);
	strncpy(folderName, project_name, strlen(project_name));
	strncat(folderName, "/", 1);

	while(file = readdir(project)){
		char* filePath = malloc(strlen(folderName)+strlen(file->d_name)+1);
		strncpy(filePath, folderName, strlen(folderName));
		strncat(filePath, file->d_name, strlen(file->d_name));
		printf("%s\n", filePath);
		remove(filePath);
		free(filePath);
	}
	free(folderName);
	rmdir(project_name);
    return;
}
void serverUpdate(int fd){
	//read project name
	int buffer;
	read(fd, &buffer, sizeof(int));
	char * project_name = malloc(buffer * sizeof(char));
	read(fd, project_name, buffer);

	//Create manifest name
  char* manifestPath = malloc(strlen(project_name)*2+10);
	memset(manifestPath, 0x0, strlen(project_name)*2+10);
	sprintf(manifestPath, "%s/%s.manifest", project_name, project_name);

	//read manifest
	struct stat manifest;
	stat(manifestPath, &manifest);
	int contents = open(manifestPath, readFlag);
	char* manifestText = malloc(manifest.st_size+1);
	memset(manifestText, 0x0, manifest.st_size+1);
	read(contents, manifestText, manifest.st_size);
	close(contents);

	//send manifest back to client
	buffer = manifest.st_size;
	write(CLIENT_ADDR, &buffer, sizeof(int));
	write(CLIENT_ADDR, manifestText, buffer);
}
// void serverCheckout(int fd){
//   char* manifestName = createManifestName(projectName);
//
//
//   read(fd, &buffer, sizeof(int));
//   char * projectName = malloc(buffer*sizeof(char)+1);
//   read(fd,projectName,buffer);
//
// }
void serverCurrentVersion(int fd){
  //read project name
  int buffer;
  read(fd, &buffer, sizeof(int));
  char * project_name = malloc(buffer * sizeof(char));
  read(fd, project_name, buffer);

  //Create manifest name
  char* manifestPath = createManifestName(project_name);

  //read manifest
  struct stat manifest;
  stat(manifestPath, &manifest);
  int contents = open(manifestPath, readFlag);
  char* manifestText = malloc(manifest.st_size+1);
  memset(manifestText, 0x0, manifest.st_size+1);
  read(contents, manifestText, manifest.st_size);
  close(contents);

  //send manifest back to client
  buffer = manifest.st_size;
  write(SERVER, &buffer, sizeof(int));
  write(SERVER, manifestText, buffer);
}
