#include "currentversionProject.h"
#include "utils.h"

void clientCurrentVersion(char* projectName)
{
	sendTo(SERVER, "currentversion");
	sendTo(SERVER, projectName);

	char *content_return = getFrom(SERVER);
	printf("clientCurrentVersion content_return = %s\n",content_return);
	if(strcmp(content_return, "ERROR") == 0)
	{
		free(content_return);
		printf("The project you entered does not exist!\n");
		return;
	}
	char *fileTreeFile = malloc(strlen(projectName) + 21);
	sprintf(fileTreeFile, "%s/.git/fileTree.txt", projectName );
	receive_file(SERVER ,fileTreeFile);

	int fd_fileTreeFile = open(fileTreeFile, O_RDONLY);
	int length = lseek(fd_fileTreeFile,0,SEEK_END);
	lseek(fd_fileTreeFile,0, SEEK_SET);
	char * buffer = malloc(length*sizeof(char) + 1);
	int bytes_read = 0;
	int size = 0;
	while (bytes_read < length)
	{
		size = read(fd_fileTreeFile,buffer,1024);
		bytes_read += size;
		printf("%s", buffer);
	}

	free(fileTreeFile);
	free(content_return);
}

void serverCurrentVersion(int fd)
{
    char* projectName = getFrom(fd);

	if(!judge_file_exists(projectName))
	{
		sendTo(fd, "OK");

		char *fileTreeFile = malloc(strlen(projectName) + 21);
		sprintf(fileTreeFile, "%s/.git/fileTree.txt", projectName );
		int fd_fileTreeFile = open(fileTreeFile, newFlag, 0644);
		save_file_index(projectName, fd_fileTreeFile);
		close(fd_fileTreeFile);

		send_file(fd, fileTreeFile);

		free(fileTreeFile);
	}
	else
	{
		sendTo(fd, "ERROR");
	}
	free(projectName);

}

