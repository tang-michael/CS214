#include "decompress.h"
#include "huffman.h"
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

static int get_line_count(char* name)
{
	int fd;
	char ch;
	int count = 0;
	if((fd=open(name ,O_RDONLY, 0666))==-1)
		perror(name),exit(-1);
	while(read(fd, &ch, 1) > 0)
	{
		if(ch == '\n')
			count++;
	}
	close(fd);
	return count;
}

static void get_codebook_content(char **ppword, char **pphuffman,char *codebookname)
{
	int fd;
	char ch;
	int index = 0;
	int count = 0;
	char buf[512] = {};
	char buf1[512] = {};
	char buf2[512] = {};
	if((fd=open(codebookname ,O_RDONLY, 0666))==-1)
		perror(codebookname),exit(-1);
	while(read(fd, &ch, 1) > 0)
	{
		if(ch == '\n'){
			sscanf(buf,"%s	%s",buf1, buf2);
			if(strcmp(buf2, "") == 0)
				strcpy(buf2, " ");
//			printf("%s %s\n",buf1, buf2);
			ppword[count] = malloc(strlen(buf2));
			strcpy(ppword[count],buf2);
			pphuffman[count] = malloc(strlen(buf1));
			strcpy(pphuffman[count++],buf1);
			index = 0;
			memset(buf,'\0',512);
			strcpy(buf,"");
		}
		else
		{
			buf[index++] = ch;
		}
	}
	close(fd);
}

void decompress_file(char *pathname, char *codebookname)
{

	char buf[512]={};
	char ch;
	int fd_pathname;
	int fd_decompress;
	int buf_index = 0;
	char decompressname[512]={};
	HuffmanNode* hufmTree = NULL;

	strncpy(decompressname, pathname, strlen(pathname)-4);

	int sizeofcodebook = get_line_count(codebookname);

	char **ppwords = (char**)malloc(sizeofcodebook*sizeof(char*));
	char **pphuffman = (char**)malloc(sizeofcodebook*sizeof(char*));
	get_codebook_content(ppwords, pphuffman, codebookname);


	hufmTree = HuffmancodeToHuffmanTree(pphuffman, sizeofcodebook);

	if((fd_pathname = open(pathname ,O_RDONLY, 0666))==-1)
		perror(pathname),exit(-1);

	if((fd_decompress = open(decompressname ,O_WRONLY|O_CREAT|O_TRUNC, 0666))==-1)
		perror(decompressname),exit(-1);

	while(read(fd_pathname, &ch, 1) > 0)
	{
		if(ch == '\n'  || ch == '\t')
		{
			HuffmanDecode(buf, hufmTree, ppwords, fd_decompress);

			write(fd_decompress, &ch, 1);
			buf_index = 0;
			memset(buf,'\0',512);
			strcpy(buf,"");
		}
		else
		{
			buf[buf_index++] = ch;
		}

	}


	int i;
	for(i = 0; i < sizeofcodebook; i++)
	{
		free(ppwords[i]);
		free(pphuffman[i]);
	}
	free(ppwords);
	free(pphuffman);
	close(fd_decompress);
	printf("%s file created\n",decompressname);
	close(fd_pathname);
}

void recursive_decompress_file(char *pathname, char *codebookname)
{
	struct stat s={};
	int res = stat(pathname,&s);
	if(res==-1)
		perror("stat"),exit(-1);
	if(S_ISDIR(s.st_mode))
	{
		DIR *d;
		struct dirent *dir;
		d = opendir(pathname);
		if (d)
		{
			while ((dir = readdir(d)) != NULL)
	        {
				if(strcmp(dir->d_name,".") == 0||strcmp(dir->d_name,"..") == 0)
				{
					continue;
				}else
				{
					char path[512]={};
					strcpy(path, pathname);
					strcat(path, "/");
					strcat(path, dir->d_name);
					recursive_decompress_file(path,codebookname);

				}
	        }
	        closedir(d);
		}
	}
	else if(S_ISREG(s.st_mode))
	{
		decompress_file(pathname, codebookname);
	}
}

void decompress(char *pathname, char *codebookname)
{
	decompress_file(pathname, codebookname);
}
void decompress_recursive(char *pathname, char *codebookname)
{

	recursive_decompress_file(pathname ,codebookname);

}
