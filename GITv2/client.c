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

	if(mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
			printf("Unable to create the project\n");
	}

	char* manifestName = malloc(strlen(projectName)*2+10);
  //Creates new .manifest file
	sprintf(manifestName, "%s/%s.manifest", projectName, projectName);
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
  //Send data to server to remove from server
	int buffer = 8;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER, "destroy", 8);

  //Send projectname to server
	buffer = strlen(projectName)+1;
	write(SERVER,&buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);
}
void clientUpdate(char* projectName){
	int buffer = 7;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER, "update", 7);

	//Send projectname to server
	buffer = strlen(projectName)+1;
	write(SERVER,&buffer,sizeof(int));
	write(SERVER,projectName,strlen(projectName)+1);

	read(SERVER,&buffer, sizeof(int));
	char* serverManifestContents = malloc(sizeof(char)*(buffer+1));
	read(SERVER, serverManifestContents, buffer);
	printf("%s\n", serverManifestContents);

	char *serverManifest = "server.manifest";
	int server_manifest = open(serverManifest, newFlag, mode);
	write(server_manifest, serverManifestContents, strlen(serverManifestContents));
	close(server_manifest);


	char* manifestPath = malloc(strlen(projectName)*2+10);
	memset(manifestPath, 0x0, strlen(projectName)*2+10);
	sprintf(manifestPath, "%s/%s.manifest", projectName, projectName);


	manEntry** clientManifestEntry = readManifest(manifestPath);
	int clientNumEntries = MANIFEST_ENTRIES;

 	manEntry** serverManifestEntry = readManifest("server.manifest");
	// remove("server.manifest");
	int serverNumEntries = MANIFEST_ENTRIES;

	// Writes code to server and client entries for update file
	compareManifestEntries(clientManifestEntry, clientNumEntries, serverManifestEntry, serverNumEntries);

	// Write to .Update file
	char* update_name = createUpdateName(projectName);
	int update_file = open(update_name, newFlag, mode);
	char *conflict_name = createConflictName(projectName);
	int conflict_file = open(conflict_name, newFlag, mode);
	int i;
	int j;
	for(i = 0; i < clientNumEntries; i++){
		writeUpdateFile(clientManifestEntry[i], update_file, conflict_name);
	}
	for(j = 0; j < clientNumEntries; j++){
		writeUpdateFile(serverManifestEntry[i], update_file, conflict_name);
	}
	close(update_file);
	close(conflict_file);
	exit(0);

}
// void clientCheckout(char *projectName){
// 	int fd = open(projectName, O_RDONLY);
//
// 	if(fd > 0){
// 		printf("File already on client side");
// 		return;
// 	}
// 	fd = open(".configure", O_RDONLY);
//
// 	if(fd < 0){
// 		printf("Configure file not found");
// 		return;
// 	}
//
//
// 	//Buffer
// 	int buffer = 7;
// 	write(SERVER, &buffer, sizeof(int));
// 	write(SERVER,"create", 7);
//
// 	write(SERVER, &buffer, sizeof(int));
// 	write(SERVER,"checkout", 5);
//
// 	//Project name
// 	buffer = strlen(projectName)+1;
// 	write(SERVER, &buffer,sizeof(int));
// 	write(SERVER,projectName,strlen(projectName)+1);
//
// }
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
void clientCurrentVersion(char *projectName){
	int buffer = 8;
	write(SERVER, &buffer, sizeof(int));
	write(SERVER, "currentVersion", 8);

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

	manEntry** manContents = readManifest("server.manifest");
	remove("server.manifest");
	int serverNumEntries = MANIFEST_ENTRIES;

	printCurrentVersion(manContents, serverNumEntries);
}
void removeFile(char *projectName, char *fileName){
	char *manifestName = createManifestName(projectName);
	manEntry** manContents = readManifest(manifestName);

	//Check if file is in manifest
	struct stat buffer;
	stat(manifestName, &buffer);
	int openMan = open(manifestName, readFlag, mode);
	char* text = malloc(buffer.st_size*sizeof(char)+1);
	read(openMan, text, buffer.st_size);
	close(openMan);
	if(strstr(text, fileName) == 0){
		printf("File not found \n");
		exit(0);
	}

	//Remove file from manifest
	int i;

	int removeOpen = open(manifestName, remFlag, mode);
	for(i = 0; i < MANIFEST_ENTRIES; i++){
		if(strcmp(manContents[i]->name, fileName) != 0){
			writeManEntry(manContents[i], removeOpen);
		}
	}

	close(removeOpen);

}
char* createFilePath(char* projectName, char* fileName){
	char* filePath = malloc(strlen(projectName)+strlen(fileName)+2);
	memset(filePath, 0x0, strlen(projectName)*2+10);
	sprintf(filePath, "%s/%s", projectName, fileName);
	return filePath;

}
void addFile(char* projectName, char* fileName){
	char* manifestName = createManifestName(projectName);
	char* manifestPath = createManifestPath(projectName);
	char* filePath = createFilePath(projectName, fileName);
	manEntry** manifest = readManifest(manifestPath);
	//
	// if(access(filePath,F_OK) < 0){
	// 	printf("File not in project");
	// 	exit(0);
	// }
	manEntry* newFile = newManEntryWithPath(fileName,manifestPath);
	int i;
	for(i = 0; i < MANIFEST_ENTRIES; i++){
		printf("%s\n", manifest[i]->name);
		if(strcmp(manifest[i]->name,fileName) == 0){
			char* tempPath = createFilePath(projectName, fileName);
			updateManEntry(newFile, tempPath);
			freeManifest(manifest);
			exit(0);
		}
	}

	int contents = open(manifestPath, addFlag, mode);
	writeManEntry(newFile, contents);
	close(contents);
	freeManEntry(newFile);
	freeManifest(manifest);
	free(manifest);
	free(manifestName);
}
