#include "checkoutProject.h"
#include "utils.h"
#include "manifest.h"

void clientCheckout(char *projectName)
{
	printf("clientCheckout\n");
    if(!judge_file_exists(projectName))
    {
    	printf("File already on client side");
    }
    else
    {
    	if(-1 == judge_file_exists(".configure"))
    		printf("Configure file not found");
    }
	sendTo(SERVER, "checkout");
	sendTo(SERVER, projectName);
	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "NON-EXIST") == 0)
	{
		printf("The project you entered does not exist on the server!\n");
		free(existFlag);
		return;
	}

	//1
    if(mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
    {
        printf("Unable to create the project\n");
    }

    char* manifestDirectory = malloc(strlen(projectName) + 5 );
    sprintf(manifestDirectory, "%s/.git", projectName);
    mkdir(manifestDirectory, S_IRUSR | S_IWUSR | S_IXUSR);
    //2
	char *directTreeFile = malloc(strlen(projectName) + 21);
	sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
    receive_file(SERVER, directTreeFile);
    mkdir_direct_tree(projectName, directTreeFile);

    //3
	int versionNumber = 0;
    char* versionStr = getFrom(SERVER);
    versionNumber = atoi(versionStr);
    save_version_number(projectName , &versionNumber);
	char* manifestName = generate_manifest_name(projectName, versionNumber);
	receive_file(SERVER, manifestName);

	//4
	int i = 0;
	int sizeManifest = 0;
	manEntry** manifestEntry = readManifest(manifestName, &sizeManifest);
	printf("manifestName = %s, sizeManifest %d\n",manifestName,sizeManifest);
	for(i = 0; i < sizeManifest; i++)
	{
		printf("i = %d\n", i);
		char* filename = truncate_version_number(manifestEntry[i]->name, versionNumber);
		printf("filename = %s\n", filename);
		receive_file(SERVER, filename);
		free(filename);
	}

}
void serverCheckout(int fd)
{
	printf("serverCheckout\n");
	//1、receive project name
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
		send_file(fd, manifestEntry[i]->name);
	}

	free(manifestName);
	free(directTreeFile);
    free(projectName);

}
