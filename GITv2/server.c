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
