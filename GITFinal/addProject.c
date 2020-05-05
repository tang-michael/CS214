#include "utils.h"
#include "addProject.h"
#include "manifest.h"

void addFile(char*projectName, char* fileName)
{

	
	if(judge_file_exists(projectName) !=0 )
	{
		printf("The project you entered doesn't exist!\n");
		return ;
	}
	char* clientManifestName = malloc(strlen(projectName) + 25);  //hello/.git/clientCommit.manifest
	sprintf(clientManifestName, "%s/.git/client.manifest", projectName);


	manEntry* newProject = newManEntry(fileName, 2);

	int fd_clientCommitManifestName = 0;
	fd_clientCommitManifestName = open(clientManifestName, newFlag, mode);

	int sizeOfClientManifestEntry = 0;
	manEntry** clientManifestEntry = readManifest(clientManifestName, &sizeOfClientManifestEntry);


	int i = 0;
	int exist = 0;
	for(i = 0; i < sizeOfClientManifestEntry; i++)
	{
		if(strcmp(clientManifestEntry[i]->name, newProject->name)== 0)
		{
			clientManifestEntry[i]->version = 2;
			exist = 1;
		}
		writeManEntry(clientManifestEntry[i], fd_clientCommitManifestName);
	}
	if(exist == 0)
	{
		writeManEntry(newProject, fd_clientCommitManifestName);
	}

	close(fd_clientCommitManifestName);


	free(clientManifestName);
	free(newProject->name);
	free(newProject->hash);
	free(newProject);
	freeManifest(clientManifestEntry, &sizeOfClientManifestEntry);
}
