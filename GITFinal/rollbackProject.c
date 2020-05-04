#include "utils.h"
#include "rollbackProject.h"
#include "manifest.h"

void clientRollback(char *projectName, int rollbackVersion)
{

	//1
	sendTo(SERVER, "rollback");
	sendTo(SERVER, projectName);

	//receive exist
	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "NON-EXIST") == 0)
	{
		printf("The project directory you entered does not exist!\n");
		free(existFlag);
		return;
	}

	//2
	int len = floor(log10(rollbackVersion)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  rollbackVersion);
	sendTo(SERVER, versionStr);
	char* versionReturn = getFrom(SERVER);
	if(strcmp(versionReturn, "WRONG") == 0)
	{
		printf("The version number you entered is higher than the maximum version number!\n");
		free(existFlag);
		free(versionStr);
		free(versionReturn);
		return;
	}
	//3
	delete_direct_and_file(projectName);
    mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR);
    char* manifestDirectory = malloc(strlen(projectName) + 5 );
    sprintf(manifestDirectory, "%s/.git", projectName);
    mkdir(manifestDirectory, S_IRUSR | S_IWUSR | S_IXUSR);

	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
    receive_file(SERVER, directTreeFile);
    mkdir_direct_tree(projectName, directTreeFile);
    int versionNumber = rollbackVersion;
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
	free(existFlag);
	free(versionStr);
	free(manifestDirectory);
	free(directTreeFile);
	free(manifestName);
	freeManifest(manifestEntry, &sizeManifest);
}

void serverRollback(int fd)
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
    //2
    char *versionStr = getFrom(fd);
    int rollbackVersionNumber = atoi(versionStr);
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);

	if(rollbackVersionNumber > versionNumber)
	{
		sendTo(fd, "WRONG");
		free(projectName);
		free(versionStr);
		return;
	}
	else
	{
		sendTo(fd, "CORRECT");
	}

    //3
	char* directIndex = generate_direct_index_name(projectName, rollbackVersionNumber);
	printf("directIndex = %s\n", directIndex);
	send_file(fd, directIndex);
	char* manifestName = generate_manifest_name(projectName, rollbackVersionNumber);
	printf("manifestName = %s\n", manifestName);
	send_file(fd, manifestName);

	//4
	int i;
	int sizeManifest = 0;
	manEntry** manifestEntry = readManifest(manifestName, &sizeManifest);

	for(i = 0; i < sizeManifest; i++)
	{
		if(strstr(manifestEntry[i]->name, ".manifest") == 0)
		{
			send_file(fd, manifestEntry[i]->name);
		}
	}

    free(projectName);
    free(versionStr);
    free(directIndex);
    free(manifestName);
    freeManifest(manifestEntry, &sizeManifest);
}
