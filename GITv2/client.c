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
voud clientUpdate(char *projectName){
	int buffer = 8;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER, "update", 8);

	//Send projectname to server
	buffer = strlen(projectName)+1;
	write(SERVER,&buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);

	read(SERVER,&buffer, sizeof(int));
	char *serverManifestContents = malloc(sizeof(buffer)+1);
	read(SERVER, serverManifestContents, buffer);

	int server_manifest = open("server.manfiest", readFlag, 0644);
	write(server_manifest, serverManifestContents, strlen(serverManifestContents));
	close(server_manifest);

	char *clientManifest = createManifestName(projectName);
	manEntry** clientManifestEntry = readManifest(clientManifest);
	int clientNumEntries = MANIFEST_ENTRIES;

	manEntry** serverManifestEntry = readManifest("server.manifest");
	remove("server.manifest");
	int clientNumEntries = MANIFEST_ENTRIES;

	//Writes code to server and client entries for update file
	compareManifestEntries(clientManifestEntry, clientNumEntries, serverManifestentry, clientNumEntries);

	//Write to .Update file
	char* update_name = createUpdateName(projectName);
	int update_file = open(update_name, newFlag, mode);
	char *conflic_name = createConflictName(projectName);
	int conflict_file = open(conflict_name, newFlag, mode);
	int i;
	for(i = 0; i < clientNumEntries; i++){
		writeUpdateFile(clientManifestEntry[i], update_file, conflic_name);
	}
	for(j = 0; j < clientNumEntries; j++){
		writeUpdateFile(serverManifestEntry[i], update_file, conflic_name);
	}
	close(contents);
	exit(0);

}
void clientCheckout(char *projectName){
	int fd = open(projecName, O_RDONLY);

	if(fd > 0){
		printf("File already on client side");
		return;
	}
	fd = open(".configure", O_RDONLY);

	if(fd < 0){
		printf("Configure file not found");
		return;
	}


	//Buffer
	int buffer = 7;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER,"create", 7);

	write(SERVER, &buffer, sizeof(int));
	write(SERVER,"checkout", 5);

	//Project name
	buffer = strlen(projectName)+1;
	write(SERVER, &buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);

}
int send_file(char * path){
	int send_file = open (path, O_RDONLY);
	if (send_file<0){
		return 0;
	}
	int length = lseek(send_file,0,SEEK_END);
	lseek(send_file,0, SEEK_SET);
	write(SERVER, &length ,sizeof(int));
	char * buffer = malloc(length*sizeof(char));
	int bytes_read = 0;
	while (bytes_read<length){
		bytes_read += read(send_file,buffer,1024);
	}

	int bytes_written=0;
	while(bytes_written<length){
		bytes_written+=write(SERVER,buffer,strlen(buffer));

	}
}
