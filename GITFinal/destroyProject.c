#include "destroyProject.h"
#include "utils.h"

void serverDestroy(int fd)
{
	//1、receive project name
    char* projectName = getFrom(fd);
    if(!judge_file_exists(projectName))
    {
    	sendTo(fd, "EXIST");
    	delete_direct_and_file(projectName);
    	if(judge_file_exists(projectName) == -1)
    	{
    		sendTo(fd, "SUCESS");
    	}
    	else
    	{
    		sendTo(fd, "FAIL");
    	}

    }
    else
    {
    	sendTo(fd, "NON-EXIST");
    	free(projectName);
    	return;
    }
    free(projectName);
}


void clientDestroy(char* projectName)
{
	//1 send "create" to server, then send projectName to server
	sendTo(SERVER, "destroy");
	sendTo(SERVER, projectName);

	//receive exist
	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "NON-EXIST") == 0)
	{
		printf("The project directory you entered does not exist!\n");
		free(existFlag);
		return;
	}
	char *destroyFlag = getFrom(SERVER);
	if(strcmp(destroyFlag, "SUCESS") == 0)
	{
		printf("The project has been destroyed!\n");
	}
	else if(strcmp(destroyFlag, "FAIL") == 0)
	{
		printf("Error destroying project!\n");
	}
	free(existFlag);
	free(destroyFlag);

}
