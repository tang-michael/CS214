#include "utils.h"

#define BUFFER_SIZE                   1024

void sendTo(int fd, char *contents)
{
	int size = strlen(contents) + 1;
	write(fd, &size, sizeof(int));
	write(fd, contents, size);
}
char *getFrom(int fd)
{
	int size = 0;
	read(fd,&size,sizeof(int));
	char* contents = malloc(size*sizeof(char) + 1);
	read(fd, contents, size);
	return contents;
}

char* generate_manifest_name(char *projectName, int versionNumber)
{
	int len = floor(log10(versionNumber)) + 1;
	char* manifestName = malloc(strlen(projectName)*2 + 20 + len);  //hello/.git/hello.manifest
	sprintf(manifestName, "%s/.git/%s.manifest.%d", projectName, projectName, versionNumber);
	return manifestName;
}

char* generate_direct_index_name(char *projectName, int versionNumber)
{
	int len = floor(log10(versionNumber)) + 1;
	char* directIndex = malloc(strlen(projectName) + 25 + len);  //hello/.git/hello.manifest
	sprintf(directIndex, "%s/.git/directTree.txt.%d", projectName, versionNumber);
	return directIndex;
}

char* truncate_version_number(char *fileName, int versionNumber)
{
	int len_filename = strlen(fileName);
	int len_version = floor(log10(versionNumber)) + 1;
	char *newFileName = malloc(len_filename-len_version);
	strncpy(newFileName, fileName, len_filename-len_version-1);
	return newFileName;
}


int judge_file_exists(char *fileName)
{
	struct stat s={};
	if(stat(fileName,&s))
		return -1;
	if(S_ISDIR(s.st_mode) || S_ISREG(s.st_mode))
		return 0;
	return -1;
}

void save_version_number(char *projectName , int *version)
{

	char *versionName = malloc(strlen(projectName) + 18);
	sprintf(versionName, "%s/.git/version.txt", projectName); //hello/.git/version.txt
	remove(versionName);
	int fd = open(versionName, newFlag, mode);
	int len = floor(log10(*version)) + 1;
	char* versionStr = malloc(len + 1);
	sprintf(versionStr, "%d",  *version);
	write(fd, versionStr, strlen(versionStr));
	close(fd);
	free(versionName);
	free(versionStr);
}

void get_version_number(char *projectName , int *version)
{
	char *versionName = malloc(strlen(projectName) + 18);
	sprintf(versionName, "%s/.git/version.txt", projectName); //hello/.git/version.txt
	int fd = open(versionName ,O_RDONLY, 0644);
	int len = lseek(fd,0,SEEK_END);

	lseek(fd,0, SEEK_SET);

	char * versionStr = malloc((len)*sizeof(char));
	read(fd, versionStr, len);
	close(fd);
	*version = atoi(versionStr);
	free(versionName);
	free(versionStr);
}

void connect_server (char * Ip, int port)
{
	int sockfd, portno, n, status, message_size, sent_size, total_size;
    struct sockaddr_in serv_addr;
    struct hostent *server;

	char recv_buffer[1024], send_buffer[1024], *tmp;


	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(Ip);
	if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
	bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(port);
    while (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0)
    {
        printf("Error connection: will retry in 3 seconds \n");
		sleep(1);
	}
	printf("the connection is good\n");

	SERVER = sockfd;
}

void send_file(int fd, char * path)
{
    char buffer[BUFFER_SIZE];
	int fd_send = open(path, O_RDONLY);
	if (fd_send<0){
		return ;
	}
	int length = lseek(fd_send,0,SEEK_END);
	lseek(fd_send,0, SEEK_SET);
	//发送数据的总长度
	write(fd, &length ,sizeof(int));
	printf("send_file length = %d\n", length);


    bzero(buffer, BUFFER_SIZE);
    int file_block_length = 0;
    while ((file_block_length = read(fd_send,buffer,BUFFER_SIZE)) > 0)
    {
        printf("file_block_length = %d\n", file_block_length);

        if (send(fd, buffer, file_block_length, 0) < 0)
        {
            printf("Send File Failed!\n");
            break;
        }

        bzero(buffer, sizeof(buffer));
    }
	close(fd_send);
	return;
}
int return_file_size(char *fileName)
{
	struct stat s={};
	if(stat(fileName,&s))
		return -1;
	return s.st_size;
}

void receive_file(int fd ,char * path)
{
    char buffer[BUFFER_SIZE];
	int fd_receive = open(path, newFlag, mode);
	if (fd_receive<0)
	{
		return ;
	}
	int length = 0;
	read(fd,&length,sizeof(int));

	printf("receive_file length = %d\n", length);
    bzero(buffer, sizeof(buffer));

    int receive_length = 0;
    int sum_recv_length = 0;

    while(1)
    {
    	length = length - receive_length;

    	if(length <= 0)
    		break;
    	if(length <= BUFFER_SIZE)
    		receive_length = recv(fd, buffer, length, 0);
    	else
    		receive_length = recv(fd, buffer, BUFFER_SIZE, 0);

        write(fd_receive, buffer, receive_length);

        sum_recv_length += receive_length;

        bzero(buffer, BUFFER_SIZE);
    }
	close(fd_receive);
}


void copy_file(char *path, char * newPath)
{
	int fd_path = open(path, O_RDONLY);
	int fd_newPath = open(newPath, newFlag, mode);
	int length = lseek(fd_path,0,SEEK_END);
	lseek(fd_path,0, SEEK_SET);
	char * buffer = malloc(length*sizeof(char) + 1);
	int bytes_read = 0;
	int size = 0;
	while (bytes_read < length)
	{
		size = read(fd_path,buffer,1024);
		bytes_read += size;
		write(fd_newPath, buffer, size);
	}

	close(fd_path);
	close(fd_newPath);

}

void save_direct_index(char *pathName, int fd)
{
	struct stat s={};
	int res = stat(pathName,&s);
	if(res==-1)
		perror("stat"),exit(-1);
	if(S_ISDIR(s.st_mode))
	{
		write(fd, pathName, strlen(pathName));
		write(fd, "\n", 1);
//		printf("%s\n", pathName);
		DIR *d;
		struct dirent *dir;
		d = opendir(pathName);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
	        {
				if(strcmp(dir->d_name,".") == 0||strcmp(dir->d_name,"..") == 0 || strcmp(dir->d_name,".git") == 0)
				{
					continue;
				}
				else
				{
					char path[512]={};
					strcpy(path, pathName);
					strcat(path, "/");
					strcat(path, dir->d_name);
					save_direct_index(path, fd);

				}
	        }

	        closedir(d);
		}
	}
}


void save_file_index(char *pathName, int fd)
{
	struct stat s={};
	int res = stat(pathName,&s);
	if(res==-1)
		perror("stat"),exit(-1);
	if(S_ISDIR(s.st_mode))
	{
		DIR *d;
		struct dirent *dir;
		d = opendir(pathName);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
	        {
				if(strcmp(dir->d_name,".") == 0||strcmp(dir->d_name,"..") == 0 || strcmp(dir->d_name,".git") == 0)
				{
					continue;
				}
				else
				{
					char path[512]={};
					strcpy(path, pathName);
					strcat(path, "/");
					strcat(path, dir->d_name);
					save_file_index(path, fd);
				}
	        }

	        closedir(d);
		}
	}
	else if(S_ISREG(s.st_mode))
	{
//		printf("pathName = %s\n", pathName);
		write(fd, pathName, strlen(pathName));
		write(fd, "\n", 1);
	}
}

void delete_direct_and_file(char *pathName)
{
	struct stat s={};
	int res = stat(pathName,&s);
	if(res==-1)
		perror("stat"),exit(-1);
	if(S_ISDIR(s.st_mode))
	{
		DIR *d;
		struct dirent *dir;
		d = opendir(pathName);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
	        {
				if(strcmp(dir->d_name,".") == 0||strcmp(dir->d_name,"..") == 0 )
				{
					continue;
				}else
				{
					char path[512]={};
					strcpy(path, pathName);
					strcat(path, "/");
					strcat(path, dir->d_name);
					delete_direct_and_file(path);
				}
	        }
	        closedir(d);
		}
	}
	else if(S_ISREG(s.st_mode))
	{
		remove(pathName);
	}
	remove(pathName);
}

void mkdir_direct_tree(char *projectDir, char *directIndexfile)
{
	int fd = open(directIndexfile ,O_RDONLY, 0644);
	int len = lseek(fd,0,SEEK_END);
	lseek(fd,0, SEEK_SET);
	char * directIndex = malloc((len)*sizeof(char));
	read(fd, directIndex, len);
	close(fd);

	printf("mkdir_direct_tree = %s\n", directIndexfile);

	int leader = 0;
	int index = 0;
	char buffer[512];
	bzero(buffer,512);
	while(leader < len)
	{
		if(directIndex[leader] != '\n')
		{
			buffer[index++] = directIndex[leader++];
		}
		else
		{
			leader++;
			index = 0;
			mkdir(buffer, 0755);
			bzero(buffer,512);
		}
	}
}
