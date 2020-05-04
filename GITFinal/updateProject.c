#include "updateProject.h"
#include "utils.h"
#include "manifest.h"

void serverupdate(int fd)
{

	//1
    char* projectName = getFrom(fd);
    if(judge_file_exists(projectName) == 0)
    {
    	sendTo(fd, "EXIST");
    }
    else
    {
    	sendTo(fd, "NON-EXIST");
    	free(projectName);
    	return;
    }
    //2
	int versionNumberServer = 0;
	get_version_number(projectName , &versionNumberServer);

	int len = floor(log10(versionNumberServer)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  versionNumberServer);
	sendTo(fd, versionStr);
	free(versionStr);

	//3
	char *equalFlag = getFrom(fd);
	if(strcmp(equalFlag, "EQUAL") == 0)
	{
		printf("Now it's the latest version\n");
		free(equalFlag);
		free(projectName);
		return;
	}
	//4
	char* manifestName = generate_manifest_name(projectName, versionNumberServer);
	send_file(fd, manifestName);


	free(manifestName);
	free(projectName);
	free(equalFlag);

}

void clientUpdate(char *projectName)
{


	//1
	sendTo(SERVER, "update");
	sendTo(SERVER, projectName);

	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "NON-EXIST") == 0)
	{
		printf("The project you entered does not exist on the server!\n");
		free(existFlag);
		return;
	}
	int versionNumberClient = 0;
	get_version_number(projectName , &versionNumberClient);

	//2
    char* versionStr = getFrom(SERVER);
    int versionNumberServer = atoi(versionStr);

    char *updateFile = malloc(strlen(projectName) + 14);
    sprintf(updateFile, "%s/.git/.Update",projectName );

    char *conflictFile = malloc(strlen(projectName) + 16);
    sprintf(conflictFile, "%s/.git/.Conflict",projectName );

    if(versionNumberClient == versionNumberServer)
    {
    	sendTo(SERVER, "EQUAL");

    	int fd_updateFile = open(updateFile, newFlag, mode);
    	remove(conflictFile);
    	free(updateFile);
    	free(conflictFile);
    	free(existFlag);
    	free(versionStr);
    	printf("Now it's the latest version\n");
    	return;
    }
    else
    {
    	sendTo(SERVER, "UNEQUAL");
    }
    //3
    char *serverManifestName = malloc(strlen(projectName) + 25);
    sprintf(serverManifestName, "%s/.git/server.manifest", projectName);
    receive_file(SERVER ,serverManifestName);

	char* manifestName = malloc(strlen(projectName)*2 + strlen(versionStr) + 20);
	sprintf(manifestName, "%s/.git/%s.manifest.%d", projectName, projectName, versionNumberClient);

    char *clientManifestName = malloc(strlen(projectName) + 25);
    sprintf(clientManifestName, "%s/.git/client.manifest", projectName);
	int fd_clientCommitManifestName = open(clientManifestName, newFlag, mode);
	recursive_newManEntry(fd_clientCommitManifestName, projectName, versionNumberClient);
	close(fd_clientCommitManifestName);

	int size_serverManifestName = 0;
	manEntry** serverManifestEntry = readManifest(serverManifestName, &size_serverManifestName);
	int size_clientManifestName = 0;
	manEntry** clientManifestEntry = readManifest(clientManifestName, &size_clientManifestName);
	int size_manifestNamae = 0;
	printf("manifestName = %s\n", manifestName);
	manEntry** projectNameManifestEntry = readManifest(manifestName, &size_manifestNamae);
	int fd_update = open(updateFile, newFlag, mode);
	int fd_conflict = open(conflictFile, newFlag, mode);

	int i, j, w;
	int exist = 0;
	for(i = 0; i <size_serverManifestName; i++)
	{
		exist = 0;
		if(strstr(serverManifestEntry[i]->name, ".manifest") != 0)
			continue;
		for(j = 0; j < size_clientManifestName; j++)
		{
			if(strstr(serverManifestEntry[i]->name, clientManifestEntry[j]->name) != 0)
			{
				exist = 1;
				if(strcmp(serverManifestEntry[i]->hash, clientManifestEntry[j]->hash) == 0)
					continue;

				for(w = 0; w < size_manifestNamae; w++)
				{
					if(strstr(projectNameManifestEntry[w]->name, ".manifest") != 0)
						continue;
					if(strstr(projectNameManifestEntry[w]->name, clientManifestEntry[j]->name) != 0)
					{
						if(strcmp(projectNameManifestEntry[w]->hash, clientManifestEntry[j]->hash) == 0)
						{
							//update
							char *updateStr = malloc(strlen(clientManifestEntry[j]->name) + strlen(serverManifestEntry[i]->hash) + 5);
							sprintf(updateStr, "M,%s,%s\n", clientManifestEntry[j]->name, serverManifestEntry[i]->hash);
							printf("%s",updateStr);
							write(fd_update, updateStr, strlen(updateStr));
							free(updateStr);
						}
						else
						{
							//conflict
							char *conflictStr = malloc(strlen(clientManifestEntry[j]->name) + strlen(serverManifestEntry[i]->hash) + 5);
							sprintf(conflictStr, "M,%s,%s\n", clientManifestEntry[j]->name, serverManifestEntry[i]->hash);
							printf("%s",conflictStr);
							write(fd_conflict, conflictStr, strlen(conflictStr));
							free(conflictStr);

						}
					}

				}
			}
		}

		//D
		if(exist == 0)
		{
			int C = 0;
			for(w = 0; w < size_manifestNamae; w++)
			{
				if(strcmp(serverManifestEntry[i]->name, projectNameManifestEntry[w]->name) != 0)
				{
					continue;
				}
				C = 1;
				//conflict
				char *conflictStr = malloc(strlen(serverManifestEntry[i]->name) + strlen(serverManifestEntry[i]->hash) + 5);
				sprintf(conflictStr, "D,%s,%s\n", serverManifestEntry[i]->name, serverManifestEntry[i]->hash);
				printf("%s",conflictStr);
				write(fd_conflict, conflictStr, strlen(conflictStr));
				free(conflictStr);
			}
			if(C == 0)
			{
				//update
				char *updateStr = malloc(strlen(serverManifestEntry[i]->name) + strlen(serverManifestEntry[i]->hash) + 5);
				sprintf(updateStr, "D,%s,%s\n", serverManifestEntry[i]->name, serverManifestEntry[i]->hash);
				printf("%s",updateStr);
				write(fd_update, updateStr, strlen(updateStr));
				free(updateStr);
			}
		}
	}

	//A
	for(j = 0; j < size_clientManifestName; j++)
	{
		exist = 0;
		for(i = 0; i <size_serverManifestName; i++)
		{
			if(strstr(serverManifestEntry[i]->name, clientManifestEntry[j]->name) != 0)
			{
				exist = 1;
			}
		}
		if(!exist) //A
		{
			int U = 0;
			for(w = 0; w < size_manifestNamae; w++)
			{
				if(strstr(projectNameManifestEntry[w]->name, clientManifestEntry[j]->name) != 0)
				{
					U = 1;
					//update
					char *updateStr = malloc(strlen(clientManifestEntry[j]->name) + strlen(clientManifestEntry[j]->hash) + 5);
					sprintf(updateStr, "A,%s,%s\n", clientManifestEntry[j]->name, clientManifestEntry[j]->hash);
					printf("%s",updateStr);
					write(fd_update, updateStr, strlen(updateStr));
					free(updateStr);
				}
			}

			if(U == 0)
			{
				//conflict
				char *conflictStr = malloc(strlen(clientManifestEntry[j]->name) + strlen(clientManifestEntry[j]->hash) + 5);
				sprintf(conflictStr, "A,%s,%s\n", clientManifestEntry[j]->name, clientManifestEntry[j]->hash);
				printf("%s",conflictStr);
				write(fd_conflict, conflictStr, strlen(conflictStr));
				free(conflictStr);
			}
		}
	}
	close(fd_conflict);
	close(fd_update);
	if(return_file_size(conflictFile) == 0)
		remove(conflictFile);
	else
		remove(updateFile);

	free(existFlag);
	free(versionStr);
	free(updateFile);
	free(conflictFile);
	free(serverManifestName);
	free(manifestName);
	free(clientManifestName);
	freeManifest(serverManifestEntry, &size_serverManifestName);
	freeManifest(clientManifestEntry, &size_clientManifestName);
	freeManifest(projectNameManifestEntry, &size_manifestNamae);
}

void writeUpdateFile(manEntry* entry, int fdUpdate, int fdConflict)
{
	if(entry->code != '-')
	{
		char path[1090];
		memset(path, 0x0, 1090);
		char *filePath = realpath(entry->name, path);

		int filePathLength = strlen(filePath);
		int hashLength = strlen(entry->hash);
		char *output = malloc((strlen(filePath) + 4 + hashLength )*sizeof(char) + 1);

		sprintf(output, "%c,%d,%s,%s\n", entry->code, entry->version, filePath, entry->hash);
		printf("%s", output);

		if(entry->code == 'M')
		{
			write(fdUpdate, output, strlen(output));
		}
		else if(entry->code == 'A')
		{
			write(fdUpdate, output, strlen(output));
		}
		else if(entry->code == 'D'){
			write(fdUpdate, output, strlen(output));
		}
		else if(entry->code == 'C')
		{
			write(fdConflict, output, strlen(output));
		}
	}
}
