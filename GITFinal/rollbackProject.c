#include "utils.h"
#include "rollbackProject.h"
#include "manifest.h"

void clientRollback(char *projectName, int rollbackVersion)
{


	int versionNumber = 0;
	int rollbackNumber = rollbackVersion;
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
	int len = floor(log10(rollbackNumber)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  rollbackNumber);
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
	get_version_number(projectName , &versionNumber);
	save_version_number(projectName , &rollbackNumber);
	//4
	char* manifestName = generate_manifest_name(projectName, rollbackVersion);
	printf("manifestName = %s\n", manifestName);
	receive_file(SERVER, manifestName);
	char* directIndex = generate_direct_index_name(projectName, rollbackVersion);
	printf("directIndex = %s\n", directIndex);
	receive_file(SERVER, directIndex);

	//5


	free(versionStr);
	free(versionReturn);
	free(existFlag);
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

	printf("rollbackVersionNumber = %d, versionNumber = %d\n", rollbackVersionNumber, versionNumber);
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
	char* manifestName = generate_manifest_name(projectName, rollbackVersionNumber);
	printf("manifestName = %s\n", manifestName);
	send_file(fd, manifestName);
	char* directIndex = generate_direct_index_name(projectName, rollbackVersionNumber);
	printf("directIndex = %s\n", directIndex);
	send_file(fd, directIndex);



    free(projectName);
}
