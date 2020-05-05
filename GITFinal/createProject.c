#include "utils.h"
#include "manifest.h"
#include "createProject.h"


/*
 * 1、client send "create" to server, server receive "create", do serverCommit()
 * 2、client send projectName to server, server receive projectName, judge project's manifest
 * 3、server judge server's manifest
 * 4、
 */

void clientCreate(char* projectName)
{

	//1 send "create" to server, then send projectName to server
	sendTo(SERVER, "create");
	sendTo(SERVER, projectName);

	//receive exist
	char *existFlag = getFrom(SERVER);
	if(strcmp(existFlag, "EXIST") == 0)
	{
		printf("You've created this project!\n");
		free(existFlag);
		return;
	}
	if(mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
			printf("Unable to create the project\n");
	}
	//2
    char* manifestDirectory = malloc(strlen(projectName) + 5 );
    sprintf(manifestDirectory, "%s/.git", projectName);
    if(mkdir(manifestDirectory, S_IRUSR | S_IWUSR | S_IXUSR) != 0){
        printf("Unable to create the project\n");
    }

    //3
    char* versionStr = getFrom(SERVER);

	char* manifestName = malloc(strlen(manifestDirectory)+ strlen(projectName) + strlen(versionStr) + 11);
	sprintf(manifestName, "%s/%s.manifest.%s", manifestDirectory, projectName, versionStr);
	receive_file(SERVER ,manifestName);

	int versionNumber = atoi(versionStr);
    save_version_number(projectName , &versionNumber);


	printf("Your project was created successfully!\n");
	free(existFlag);
	free(manifestDirectory);
	free(manifestName);
	free(versionStr);

}

void serverCreate(int fd)
{

	int versionNumber = 1;

	//1、receive project name
    char* projectName = getFrom(fd);
    if(!judge_file_exists(projectName))
    {
    	sendTo(fd, "EXIST");
    	free(projectName);
    	return;
    }
    else
    {
    	sendTo(fd, "NON-EXIST");
    }

    ///3.1
    if(mkdir(projectName, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
    {
        printf("Unable to create the project\n");
    }

    char* manifestDirectory = malloc(strlen(projectName) + 5 );
    sprintf(manifestDirectory, "%s/.git", projectName);
    //3.2
    if(mkdir(manifestDirectory, S_IRUSR | S_IWUSR | S_IXUSR) != 0)
    {
        printf("Unable to create the project\n");
    }

    //2、server保存本工程的版本号
    save_version_number(projectName , &versionNumber);

	//3.3
    char* manifestName = generate_manifest_name(projectName, versionNumber);

	int contents = open(manifestName, newFlag, mode);
	manEntry* newProject = newManEntry(manifestName, versionNumber);
	writeManEntry(newProject, contents);
	close(contents);

	//4
	int len = floor(log10(versionNumber)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  versionNumber);
	sendTo(fd, versionStr);
	send_file(fd, manifestName);

	char* historyName = malloc(strlen(projectName)+9);
	sprintf(historyName,"%s/.history",projectName);
	int history = open(historyName, newFlag, mode);
	char* historyText = malloc(strlen(projectName)+15);
	sprintf(historyText, "0 %s Project Created Created\n", projectName);
	write(history, historyText, strlen(historyText));
	close(history);

	free(projectName);
	free(manifestDirectory);
	free(manifestName);
	free(versionStr);
	freeManEntry(newProject);
}
