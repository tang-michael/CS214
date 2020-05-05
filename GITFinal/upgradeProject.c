#include "manifest.h"
#include "utils.h"
#include "upgradeProject.h"

void clientUpgrade(char* projectName)
{

	//2
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
	if(judge_file_exists(updateFile) != 0)
	{
		printf("The %s file dose not exist，please updates your project first!\n",updateFile);
		free(updateFile);
		free(conflictFile);
		return;
	}
	//1
	sendTo(SERVER, "upgrade");
	sendTo(SERVER, projectName);

	//3
	//receive exist
	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "NON-EXIST") == 0)
	{
		printf("The project directory you entered does not exist on the server!\n");
		free(existFlag);
		free(updateFile);
		free(conflictFile);
		return;
	}

	delete_direct_and_file(projectName);
    mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR);
    char* manifestDirectory = malloc(strlen(projectName) + 5 );
    sprintf(manifestDirectory, "%s/.git", projectName);
    mkdir(manifestDirectory, S_IRUSR | S_IWUSR | S_IXUSR);

	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
    receive_file(SERVER, directTreeFile);
    mkdir_direct_tree(projectName, directTreeFile);
	int versionNumber = 0;
    char* versionStr = getFrom(SERVER);
    versionNumber = atoi(versionStr);
    save_version_number(projectName , &versionNumber);
	char* manifestName = generate_manifest_name(projectName, versionNumber);
	receive_file(SERVER, manifestName);

	int i = 0;
	int sizeManifest = 0;
	manEntry** manifestEntry = readManifest(manifestName, &sizeManifest);
	printf("manifestName = %s, sizeManifest %d\n",manifestName,sizeManifest);
	for(i = 0; i < sizeManifest; i++)
	{
		if(strstr(manifestEntry[i]->name, ".manifest") == 0)
		{
			char* filename = truncate_version_number(manifestEntry[i]->name, versionNumber);
			printf("filename = %s\n", filename);
			receive_file(SERVER, filename);
			free(filename);
		}
	}

	free(updateFile);
	free(conflictFile);
	free(existFlag);
	free(manifestDirectory);
	free(directTreeFile);
	free(versionStr);
	free(manifestName);
	freeManifest(manifestEntry, &sizeManifest);

}
void serverUpgrade(int fd)
{

	//1
    char* projectName = getFrom(fd);
    if(!judge_file_exists(projectName))
    {
    	sendTo(fd, "EXIST");
    }
    else
    {
    	sendTo(fd, "NON-EXIST");
    	free(projectName);
    	return;
    }

    //1
	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
	printf("directTreeFile = %s\n ",directTreeFile);
	send_file(fd, directTreeFile);

	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);

	int len = floor(log10(versionNumber)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  versionNumber);
	sendTo(fd, versionStr);

	char* manifestName = generate_manifest_name(projectName, versionNumber);
	send_file(fd, manifestName);

	//2
	int i;
	int sizeManifest = 0;
	manEntry** manifestEntry = readManifest(manifestName, &sizeManifest);

	printf("manifestName = %s, sizeManifest %d\n",manifestName,sizeManifest);
	for(i = 0; i < sizeManifest; i++)
	{
		if(strstr(manifestEntry[i]->name, ".manifest") == 0)
		{
			send_file(fd, manifestEntry[i]->name);
		}
	}

    free(projectName);
    free(directTreeFile);
    free(versionStr);
    free(manifestName);
    freeManifest(manifestEntry, &sizeManifest);

}
