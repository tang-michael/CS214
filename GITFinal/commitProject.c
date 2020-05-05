#include "utils.h"
#include "manifest.h"
#include "commitProject.h"

void serverCommit(int fd)
{
	//Gets project name from client
	char *projectName = getFrom(fd);


	// If git server has the project

	if(judge_file_exists(projectName) !=0 )
	{
		sendTo(fd, "ERROR");
		free(projectName);
		return;

	}
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);
	sendTo(fd, "OK");
	get_version_number(projectName , &versionNumber);

	char* manifestName = generate_manifest_name(projectName, versionNumber);

	int len = floor(log10(versionNumber)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  versionNumber);
	sendTo(fd, versionStr);
	free(versionStr);
	free(projectName);
	free(manifestName);
}


static int judge_add_remove(manEntry** clientManifestEntry, int sizeOfClientManifestEntry, manEntry* node)
{
	int i;

	int returnValue = 0;
	for(i = 0; i < sizeOfClientManifestEntry; i++)
	{
		if(strcmp(node->name, clientManifestEntry[i]->name) != 0)
		{
			returnValue = clientManifestEntry[i]->version;
			clientManifestEntry[i]->version = 3;
			return returnValue;
		}
	}
	return 0;
}
//hello.manifest  clientCommit.manifest
static void generateCommitManifestEntries(char *projectName, manEntry** clientManifestEntry, int manifestNumEntries, manEntry** clientCommitEntry, int commitNumEntries)
{
	int i, j;

	char *commitFileName = malloc(strlen(projectName) + 14);
	sprintf(commitFileName, "%s/.git/.Commit", projectName);
	int fd = open(commitFileName, newFlag, 0644);

	char* clientManifestName = malloc(strlen(projectName) + 25);  //hello/.git/client.manifest
	sprintf(clientManifestName, "%s/.git/client.manifest", projectName);

	int sizeOfClientManifestEntry = 0;
	manEntry** clientCommitManifestEntry = readManifest(clientManifestName, &sizeOfClientManifestEntry);


	int exist = 0;
	for(i = 0; i < manifestNumEntries; i++)  //1
	{
		exist = 0;
		if(strstr(clientManifestEntry[i]->name, ".manifest") == 0)
		{
			for(j = 0; j < commitNumEntries; j++)
			{
				if(strstr(clientManifestEntry[i]->name, clientCommitEntry[j]->name) != NULL)
				{
					exist = 1;
					//M
					if(strcmp(clientManifestEntry[i]->hash, clientCommitEntry[j]->hash) != 0)
					{
						char *commitStr = malloc(strlen(clientCommitEntry[j]->name) + strlen(clientCommitEntry[j]->hash) + 5);
						sprintf(commitStr, "M,%s,%s\n", clientCommitEntry[j]->name, clientCommitEntry[j]->hash);
						if(judge_add_remove(clientCommitManifestEntry, sizeOfClientManifestEntry, clientCommitEntry[j]) != 1)
							write(fd, commitStr, strlen(commitStr));
						free(commitStr);
					}
					else
					{

					}
				}
			}
			//In commitManifest but not clientManifest
			if(!exist)
			{
				char *commitStr = malloc(strlen(clientManifestEntry[i]->name) + strlen(clientManifestEntry[i]->hash) + 5);
				sprintf(commitStr, "D,%s,%s\n", clientManifestEntry[i]->name, clientManifestEntry[i]->hash);
				if(judge_add_remove(clientCommitManifestEntry, sizeOfClientManifestEntry, clientManifestEntry[i]) != 1)
					write(fd, commitStr, strlen(commitStr));
				free(commitStr);
			}
		}
	}
	for(j = 0; j < commitNumEntries; j++)  //6
	{
		exist = 0;
		for(i = 0; i < manifestNumEntries; i++) //1
		{

			if(strstr(clientManifestEntry[i]->name, clientCommitEntry[j]->name) != 0)
			{
				exist = 1;
			}
		}
		if(!exist)
		{
			char *commitStr = malloc(strlen(clientCommitEntry[j]->name) + strlen(clientCommitEntry[j]->hash) + 10);
			sprintf(commitStr, "A,%s,%s\n", clientCommitEntry[j]->name, clientCommitEntry[j]->hash);
			if(judge_add_remove(clientCommitManifestEntry, sizeOfClientManifestEntry, clientCommitEntry[j]) != 1)
				write(fd, commitStr, strlen(commitStr));
			free(commitStr);
			commitStr = NULL;
		}

	}

	for(i = 0; i < sizeOfClientManifestEntry; i++)
	{
		if(clientCommitManifestEntry[i]->version == 2)
		{
			char *commitStr = malloc(strlen(clientCommitManifestEntry[i]->name) + strlen(clientCommitEntry[i]->hash) + 10);
			sprintf(commitStr, "A,%s,%s\n", clientCommitEntry[i]->name, clientCommitEntry[i]->hash);
			write(fd, commitStr, strlen(commitStr));
			free(commitStr);
		}
	}


	close(fd);
	free(commitFileName);
	freeManifest(clientCommitManifestEntry, &sizeOfClientManifestEntry);
}

static void createCommitFile(char *projectName)
{

	//1
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);
	char* projectManifestName = generate_manifest_name(projectName, versionNumber);
	int sizeOfProjectManifestEntry = 0;
	manEntry** projectNameManifestEntry = readManifest(projectManifestName, &sizeOfProjectManifestEntry);

	//2
	char* clientCommitManifestName = malloc(strlen(projectName) + 28);  //hello/.git/clientCommit.manifest
	sprintf(clientCommitManifestName, "%s/.git/clientCommit.manifest", projectName);




	int fd_clientCommitManifestName = open(clientCommitManifestName, O_WRONLY | O_CREAT|O_APPEND , mode);
	//生成clientCommit.manifest
	recursive_newManEntry(fd_clientCommitManifestName, projectName, 1);
	close(fd_clientCommitManifestName);
	int sizeOfClientCommitManifestEntry = 0;
	manEntry** clientCommitManifestEntry = readManifest(clientCommitManifestName, &sizeOfClientCommitManifestEntry);

	//3
	generateCommitManifestEntries(projectName ,projectNameManifestEntry, sizeOfProjectManifestEntry, clientCommitManifestEntry, sizeOfClientCommitManifestEntry);

	//4
	remove(clientCommitManifestName);

	printf(" create a commit file\n");
	free(clientCommitManifestName);
	free(projectManifestName);
	freeManifest(projectNameManifestEntry, &sizeOfProjectManifestEntry);
	freeManifest(clientCommitManifestEntry, &sizeOfClientCommitManifestEntry);


}

void clientCommit(char* projectName)
{

    char *updateFile = malloc(strlen(projectName) + 14);
    sprintf(updateFile, "%s/.git/.Update",projectName );

    char *conflictFile = malloc(strlen(projectName) + 16);
    sprintf(conflictFile, "%s/.git/.Conflict",projectName );
	if(judge_file_exists(conflictFile) == 0)
	{
		printf("The %s file exists!\n",conflictFile);
		free(updateFile);
		free(conflictFile);
		return;
	}
	if(judge_file_exists(updateFile) == 0)
	{
		if(return_file_size(updateFile) > 0)
		{
			printf("The %s file  exists!\n",updateFile);
			free(updateFile);
			free(conflictFile);
			return;
		}
	}

	//send command to server
	sendTo(SERVER, "commit");

	//Send Project name to server
	sendTo(SERVER, projectName);

	// If git server has the project, return OK
	char *content_return = getFrom(SERVER);

	printf("content_return = %s\n", content_return);

	if(strcmp(content_return, "ERROR") == 0)
	{
		free(content_return);
		printf("You need to create your project on the server first!\n");
		return;
	}
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);

    char* versionStr = getFrom(SERVER);
    int serverVersionNumber = atoi(versionStr);


	if(versionNumber == serverVersionNumber)
	{

		char *directTreeFile = malloc(strlen(projectName) + 21);
		sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
		int fd_directTreeFile = open(directTreeFile, newFlag, 0644);
		save_direct_index(projectName, fd_directTreeFile);
		createCommitFile(projectName);
	}
	else
	{
		printf("You need to update your project first!\n");
	}


	free(content_return);
	free(versionStr);
}
