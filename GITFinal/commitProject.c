#include "utils.h"
#include "manifest.h"
#include "commitProject.h"

/*
 * 1、客户端发送commit命令和工程名字到服务端
 * 2、服务端发送“OK”，然后发送版本号到客户端
 */

void serverCommit(int fd)
{
	//Gets project name from client
	char *projectName = getFrom(fd);

	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);

	char* manifestName = generate_manifest_name(projectName, versionNumber);

	// If git server has the project

	if(!judge_file_exists(projectName))
	{
		sendTo(fd, "OK");

		int len = floor(log10(versionNumber)) + 1;
		char* versionStr = malloc(len + 1);
		sprintf(versionStr, "%d",  versionNumber);
		sendTo(fd, versionStr);
		free(versionStr);
	}
	else
	{
		sendTo(fd, "ERROR");
	}
	free(projectName);
	free(manifestName);
}

//projectName.manifest 与 clientCommit.manifest进行比较，生成.commit文件
//hello.manifest  clientCommit.manifest
static void generateCommitManifestEntries(char *projectName, manEntry** clientManifestEntry, int manifestNumEntries, manEntry** clientCommitEntry, int commitNumEntries)
{
	int i, j;

	char *commitFileName = malloc(strlen(projectName) + 14);
	sprintf(commitFileName, "%s/.git/.Commit", projectName);
	int fd = open(commitFileName, newFlag, 0644);

	int exist = 0;
	for(i = 0; i < manifestNumEntries; i++)  //1
	{
		exist = 0;
		if(strstr(clientManifestEntry[i]->name, ".manifest") == 0)
		{
			for(j = 0; j < commitNumEntries; j++)
			{
				if(strstr(clientManifestEntry[i]->name, clientCommitEntry[j]->name) != NULL)
				{
					exist = 1;
					//M
					if(strcmp(clientManifestEntry[i]->hash, clientCommitEntry[j]->hash) != 0)
					{
						char *commitStr = malloc(strlen(clientCommitEntry[j]->name) + strlen(clientCommitEntry[j]->hash) + 5);
						sprintf(commitStr, "M,%s,%s\n", clientCommitEntry[j]->name, clientCommitEntry[j]->hash);
						write(fd, commitStr, strlen(commitStr));
						free(commitStr);
					}
					else
					{

					}
				}
			}
			//In commitManifest but not clientManifest
			if(!exist)
			{
				char *commitStr = malloc(strlen(clientManifestEntry[i]->name) + strlen(clientManifestEntry[i]->hash) + 5);
				sprintf(commitStr, "D,%s,%s\n", clientManifestEntry[i]->name, clientManifestEntry[i]->hash);
				write(fd, commitStr, strlen(commitStr));
				free(commitStr);
			}
		}
	}
	for(j = 0; j < commitNumEntries; j++)  //6
	{
		exist = 0;
		for(i = 0; i < manifestNumEntries; i++) //1
		{

			if(strstr(clientManifestEntry[i]->name, clientCommitEntry[j]->name) != 0)
			{
				exist = 1;
			}
		}
		if(!exist)
		{
			char *commitStr = malloc(strlen(clientCommitEntry[j]->name) + strlen(clientCommitEntry[j]->hash) + 10);
			sprintf(commitStr, "A,%s,%s\n", clientCommitEntry[j]->name, clientCommitEntry[j]->hash);
			write(fd, commitStr, strlen(commitStr));
			free(commitStr);
			commitStr = NULL;
		}

	}
	close(fd);
}

static void createCommitFile(char *projectName)
{
	/*
	 * 1、读取本地的projectName.manifest文件
	 * 2、根据本地文件生成新的clientCommit.manifest
	 * 3、projectName.manifest 与 clientCommit.manifest进行比较，生成.commit文件
	 * 4、删除clientCommit.manifest
	 */
	//1
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);
	char* projectManifestName = generate_manifest_name(projectName, versionNumber);
	int sizeOfProjectManifestEntry = 0;
	manEntry** projectNameManifestEntry = readManifest(projectManifestName, &sizeOfProjectManifestEntry);

	//2
	char* clientCommitManifestName = malloc(strlen(projectName) + 28);  //hello/.git/clientCommit.manifest
	sprintf(clientCommitManifestName, "%s/.git/clientCommit.manifest", projectName);
	int fd_clientCommitManifestName = open(clientCommitManifestName, newFlag, mode);
	//生成clientCommit.manifest
	recursive_newManEntry(fd_clientCommitManifestName, projectName, 1);
	close(fd_clientCommitManifestName);
	int sizeOfClientCommitManifestEntry = 0;
	manEntry** clientCommitManifestEntry = readManifest(clientCommitManifestName, &sizeOfClientCommitManifestEntry);

	//3
	generateCommitManifestEntries(projectName ,projectNameManifestEntry, sizeOfProjectManifestEntry, clientCommitManifestEntry, sizeOfClientCommitManifestEntry);

	//4
	remove(clientCommitManifestName);

	printf(" create a commit file\n");
	free(clientCommitManifestName);
	free(projectManifestName);
	freeManifest(projectNameManifestEntry, &sizeOfProjectManifestEntry);
	freeManifest(clientCommitManifestEntry, &sizeOfClientCommitManifestEntry);


}

void clientCommit(char* projectName)
{
	/*
	 * 1、发送commit命令，projectName到服务端
	 * 2、获取服务器最高版本号，与本地版本号进行比较
	 * 3、生成目录树文件 .git/directTree.txt
	 * 4、生成.commit文件
	 */
	//send command to server
	sendTo(SERVER, "commit");

	//Send Project name to server
	sendTo(SERVER, projectName);

	// If git server has the project, return OK
	char *content_return = getFrom(SERVER);

	if(strcmp(content_return, "ERROR") == 0)
	{
		free(content_return);
		printf("You need to create your project on the server first!\n");
		return;
	}
	int versionNumber = 0;
	get_version_number(projectName , &versionNumber);

    char* versionStr = getFrom(SERVER);
    int serverVersionNumber = atoi(versionStr);

	//比较client与server的版本号文件是否一样？

	if(versionNumber == serverVersionNumber)
	{
		//1、读取本地的manifest
		//2、遍历本地所有文件，生成本地的hash
		//3、将本地的hash与manifest进行比较，将结果写入commit
		char *directTreeFile = malloc(strlen(projectName) + 21);
		sprintf(directTreeFile, "%s/.git/directTree.txt", projectName );
		int fd_directTreeFile = open(directTreeFile, newFlag, 0644);
		save_direct_index(projectName, fd_directTreeFile);
		createCommitFile(projectName);
	}
	else
	{
		printf("You need to update your project first!\n");
	}


	free(content_return);
}
