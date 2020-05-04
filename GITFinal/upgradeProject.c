#include "manifest.h"
#include "utils.h"
#include "upgradeProject.h"

void clientUpgrade(char* projectName)
{

	//1
	sendTo(SERVER, "upgrade");
	sendTo(SERVER, projectName);
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
	//4
	send_file(SERVER, updateFile);
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
    //2
    char *updateFile = malloc(strlen(projectName) + 14);
    sprintf(updateFile, "%s/.git/.Update",projectName );
    receive_file(fd, updateFile);



    free(projectName);


}
