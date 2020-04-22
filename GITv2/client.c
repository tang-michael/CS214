#include "WTF.h"

void createProject(char* projectName){

	//Buffer
	int buffer = 7;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER,"create", 7);

	//Project name
	buffer = strlen(projectName)+1;
	write(SERVER, &buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);

	char* manifestName = createManifestName(projectName);
  //Creates new .manifest file
	int contents = open(manifestName, newFlag, mode);
  //Creates  a manEntry for the manifest file
	manEntry* newProject = newManEntry(manifestName);
  //Store contents of manifest into file
	writeManEntry(newProject, contents);
	close(contents);
	free(manifestName);
	freeManEntry(newProject);
}

void destroyProject(char* projectName){
  //Remove from local directory
	char* manifestName = parseManifestName(projectName);
	remove(manifestName);
	free(manifestName);

  //Send data to server to remove from server
	int buffer = 8;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER, "destroy", 8);

  //Send projectname to server
	buffer = strlen(projectName)+1;
	write(SERVER,&buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);
}
