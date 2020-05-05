#include "utils.h"
#include "pushProject.h"
#include "manifest.h"

/*
 * client: send "push", send projectName, get result and judge .Commit , send  .Commit
 * server: get  "push", get  projectName, judge project and send result, receive .Commit
 *
 */

static manEntry *extractCommitEntries(char *text, int trailer)
{
	int leader = trailer;
	manEntry *entry = malloc(sizeof(manEntry));
	entry->code = text[leader];
	leader += 2;
	trailer = leader;
	while(text[leader] != ',')
	{
		leader++;
	}
	entry->name = malloc((leader-trailer)*sizeof(char));
	memset(entry->name, 0x0, (leader-trailer)*sizeof(char)+1);
	strncpy(entry->name, (char*)text+trailer,leader-trailer);
	leader++;

	trailer = leader;
	while(text[leader] != '\n')
	{
		leader++;
	}
	entry->hash = malloc(SHA_DIGEST_LENGTH*2+1);
	memset(entry->hash, 0x0, (leader-trailer)*sizeof(char)+1);
	strncpy(entry->hash, (char*)text+trailer,leader-trailer);

	return entry;
}

static manEntry **readCommit(char *commitName, int* size)
{
	struct stat buff;
	stat(commitName, &buff);

	int contents = open(commitName, readFlag, 0644);
	char *text = malloc(buff.st_size*sizeof(char)+1);
	memset(text,0x0,buff.st_size*sizeof(char)+1);
	read(contents, text, buff.st_size);
	close(contents);

	int counter = 0;
	int  leader =0, trailer = 0;
	int maxEntries = 100;

	manEntry** entries = malloc(maxEntries*sizeof(manEntry*));

	while(leader < buff.st_size)
	{
		while(text[leader] != '\n')
		{
			leader++;
		}

		entries[counter] = extractCommitEntries(text, trailer);
		counter++;

		leader++;
		if(text[leader] == '\0' && leader < buff.st_size)
			leader++;

		trailer = leader;
		if(counter == maxEntries)
		{
			maxEntries += 5;
		entries =	realloc(entries, maxEntries*sizeof(manEntry*));
		}
	}

	*size = counter;
	return entries;
}

static void freeCommitManEntry(manEntry* entry)
{
	free(entry->name);
	free(entry->hash);
	free(entry);
}
static void freeReadManifest(manEntry** manifest, int *size)
{
	int i;
	for(i = 0; i < *size; i++){
		freeCommitManEntry(manifest[i]);
	}
}

static void receiveCommitFileAndUpdateManifestFile(int fd, char *commitFileName, char *manifestName, char* projectName, int versionNumber)
{

	int sizeCommit = 0;
	int i, j;
	manEntry** commitEntry = readCommit(commitFileName, &sizeCommit);
	int sizeManifest = 0;
	manEntry** manifestEntry = readManifest(manifestName, &sizeManifest);
	for(i = 0; i < sizeCommit; i++)
	{
		printf("%c,%s,%s\n", commitEntry[i]->code, commitEntry[i]->name, commitEntry[i]->hash);
		if(commitEntry[i]->code == 'A' || commitEntry[i]->code == 'M')
		{
			char *newName = malloc(strlen(commitEntry[i]->name)+ floor(log10(versionNumber)) + 1 + 1);
			sprintf(newName, "%s.%d", commitEntry[i]->name, versionNumber);
			receive_file(fd, newName);
			free(newName);
		}
		else if(commitEntry[i]->code == 'D')
		{
			char *newName = malloc(strlen(commitEntry[i]->name)+ floor(log10(versionNumber)) + 1 + 1);
			sprintf(newName, "%s.%d", commitEntry[i]->name, versionNumber);
			free(newName);
		}
	}
	char *newManifestName = generate_manifest_name(projectName, versionNumber);
	int fd_newManifestName = open(newManifestName, newFlag, mode);
	int M = 0, D = 0;
	for(i = 0; i < sizeManifest; i++)
	{
		M = 0;
		D = 0;
		manifestEntry[i]->version = versionNumber;
		for(j = 0; j < sizeCommit; j++)
		{
			if(strstr(manifestEntry[i]->name, commitEntry[j]->name) != 0)
			{
				if(commitEntry[j]->code == 'M')
				{
					strcpy(manifestEntry[i]->hash, commitEntry[j]->hash);
					M = 1;
				}
				else
				{
					D = 1;
				}
				if(M == 1)
				{

					commitEntry[j]->version = versionNumber;
					char *newName = malloc(strlen(commitEntry[j]->name)+ floor(log10(versionNumber)) + 1 + 1);
					sprintf(newName, "%s.%d", commitEntry[j]->name, versionNumber);
					commitEntry[j]->name = realloc(commitEntry[j]->name, strlen(newName));
					strcpy(commitEntry[j]->name, newName);
					writeManEntry(commitEntry[j], fd_newManifestName);
					free(newName);
				}
			}
		}
		if(M == 0 && D == 0)
			writeManEntry(manifestEntry[i], fd_newManifestName);

	}
	for(j = 0; j < sizeCommit; j++)
	{
		if(commitEntry[j]->code == 'A')
		{
			commitEntry[j]->version = versionNumber;
			char *newName = malloc(strlen(commitEntry[j]->name)+ floor(log10(versionNumber)) + 1 + 1);
			sprintf(newName, "%s.%d", commitEntry[j]->name, versionNumber);
			commitEntry[j]->name = realloc(commitEntry[j]->name, strlen(newName));
			strcpy(commitEntry[j]->name, newName);
			writeManEntry(commitEntry[j], fd_newManifestName);
			free(newName);
		}
	}
	close(fd_newManifestName);

	freeReadManifest(commitEntry, &sizeCommit);
	freeManifest(manifestEntry, &sizeManifest);
	free(commitEntry);
	free(newManifestName);
}

void serverPush(int fd)
{
	//Gets project name from client
	char *projectName = getFrom(fd);

	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);
	char* manifestName = generate_manifest_name(projectName, versionNumber);

	char *commitFileName = malloc(strlen(projectName) + 14);
	sprintf(commitFileName, "%s/.git/.Commit", projectName);

	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );

	// If git server has the project
	if(!judge_file_exists(projectName))
	{
		sendTo(fd, "OK");

		printf("now, begin receive %s from client\n", commitFileName);
		//1
		receive_file(fd, commitFileName);
		usleep(100);
		receive_file(fd, directTreeFile);

		mkdir_direct_tree(projectName, directTreeFile);
		//3
		versionNumber++;
		receiveCommitFileAndUpdateManifestFile(fd, commitFileName, manifestName, projectName, versionNumber);

		//4
	    save_version_number(projectName , &versionNumber);
		int len = floor(log10(versionNumber)) + 1;
		char* versionStr = malloc(len + 1);
		sprintf(versionStr, "%d",  versionNumber);
		sendTo(fd, versionStr);
		char* newManifestName = generate_manifest_name(projectName, versionNumber);
		send_file(fd, newManifestName);


		char* historyName = malloc(strlen(projectName)+9);
		sprintf(historyName,"%s/.history",projectName);
		int history = open(historyName, addFlag, mode);
		char* historyText = malloc(len+strlen(projectName)+17);
		sprintf(historyText, "%i %s Files Pushed\n", versionNumber, projectName);
		write(history, historyText, strlen(historyText));
		close(history);

		//5
		remove(commitFileName);
		free(newManifestName);

	}
	else
	{
		sendTo(fd, "ERROR");
	}
	free(projectName);
	free(manifestName);
	free(commitFileName);
	free(directTreeFile);
}



void clientPush(char* projectName)
{
	//send command to server
	sendTo(SERVER, "push");

	//Send Project name to server
	sendTo(SERVER, projectName);

	// If git server has the project, return OK
	char *content_return = getFrom(SERVER);

	if(strcmp(content_return, "ERROR") == 0)
	{
		free(content_return);
		printf("The project name you entered does not exist on the server!\n");
		return;
	}
	char *commitFileName = malloc(strlen(projectName) + 14);
	sprintf(commitFileName, "%s/.git/.Commit", projectName);
	if(judge_file_exists(commitFileName) != 0)
	{
		free(content_return);
		free(commitFileName);
		printf("The client has no .Commit file!\n");
		return;
	}


	//1
	printf("now, begin send %s to server\n", commitFileName);
	send_file(SERVER, commitFileName);
	usleep(100);
	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
	send_file(SERVER, directTreeFile);


	int size = 0;
	int i;
	//2
	manEntry** commitEntry = readCommit(commitFileName, &size);
	for(i = 0; i < size; i++)
	{

		if(commitEntry[i]->code == 'A'||commitEntry[i]->code == 'M')
		{
			send_file(SERVER, commitEntry[i]->name);
		}
	}

	//3
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);
	char* manifestName = generate_manifest_name(projectName, versionNumber);
	remove(manifestName);

    char* versionStr = getFrom(SERVER);
    versionNumber = atoi(versionStr);
    save_version_number(projectName , &versionNumber);
	char* newManifestName = generate_manifest_name(projectName, versionNumber);
	receive_file(SERVER ,newManifestName);


	//4
	remove(commitFileName);

	freeReadManifest(commitEntry, &size);
	free(commitEntry);
	free(commitFileName);
	free(directTreeFile);
	free(content_return);
	free(manifestName);
	free(newManifestName);
}
