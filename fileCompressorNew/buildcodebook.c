#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include "buildcodebook.h"
#include "minheap.h"
#include "hashmap.h"
#include "huffman.h"

#define EMPTY_STR ""

static int int_comparator(void *a, void *b)
{
	item* pitem1 = (item*)a;
	item* pitem2 = (item*)b;

	if (pitem1->fre == pitem2->fre)
	{
		return strcmp(pitem1->pword, pitem2->pword);
    }
	else if (pitem1->fre > pitem2->fre)
    {
         return 1;
    }
	else
	{
		return -1;
	}
}


static void save_string_to_list(linklist *p_linklist, char * pathname)
{
	char buf[512]={};
	char ch;
	int fd;
	int buf_index = 0;
	if((fd=open(pathname ,O_RDONLY, 0666))==-1)
		perror(pathname),exit(-1);

	while(read(fd, &ch, 1) > 0)
	{
		if(ch == '\n' || ch == ' ' || ch =='\t')
		{
			if(-1 == linklist_update_node(p_linklist, buf))
				linklist_insert_head(p_linklist, 1, buf, strlen(buf));
			memset(buf,'\0',512);
			strcpy(buf,EMPTY_STR);

			if(ch == ' ')
			{
				buf[0] = ch;
				if(-1 == linklist_update_node(p_linklist, buf))
					linklist_insert_head(p_linklist, 1, buf, strlen(buf));
				memset(buf,'\0',512);
				strcpy(buf,EMPTY_STR);
			}

			buf_index = 0;
		}
		else
		{
			buf[buf_index++] = ch;
		}
	}
	close(fd);
}

static void list_to_minheap(linklist *p_linklist, MinHeap *heap)
{

	char pstr[512] ={};
	int fre;
	item *piteminput = NULL;

	while(linklist_pop_head(p_linklist, &fre, pstr) == 0){
		piteminput = malloc(sizeof(item));
		piteminput->fre = fre;
		piteminput->pword = (char*)malloc(strlen(pstr));
		strcpy(piteminput->pword, pstr);
		minHeap_insert(heap, piteminput);
		memset(pstr,'\0',512);
		strcpy(pstr,EMPTY_STR);
	}
}

static void minheap_to_huffman(HuffmanNode *hufmTree , MinHeap *heap)
{
	int fd;
	item *pitemoutput = NULL;
	char *pathnamehcz= "HuffmanCodebook";
	if((fd=open(pathnamehcz,O_WRONLY|O_CREAT|O_TRUNC,0666))==-1)
		perror(pathnamehcz),exit(-1);

	int sizeMH = (int)minHeap_size(heap);
	int *arr= (int*)malloc(sizeMH*sizeof(int));
    char **pwords = (char**)malloc(sizeMH*sizeof(char*));
    int i;
    for(i = 0; i < sizeMH; i++)
    {
    	if((pitemoutput = (item*)minHeap_pop(heap))!=NULL)
    	{
    		printf("%s		%d\n", pitemoutput->pword, pitemoutput->fre);
    		arr[i] = pitemoutput->fre;
    		pwords[i] = (char*)malloc(strlen(pitemoutput->pword));
    		strcpy(pwords[i], pitemoutput->pword);

    		free(pitemoutput->pword);
    		pitemoutput->pword = NULL;
    		free(pitemoutput);
    		pitemoutput = NULL;
    	}
    	else
    		break;
    }
    hufmTree = createHuffmanTree(arr, sizeMH);
    PrintHuffmanTree(hufmTree);
    HuffmanCode(hufmTree, 0, pwords,fd);
    close(fd);
    free(arr);
    free(pwords);
}

void build_codebook(char* pathname)
{
	linklist lnklst;
	lnklst.head.p_next=NULL;
	save_string_to_list(&lnklst, pathname);
	linklist_print(&lnklst);

	MinHeap *heap = minHeap_create(10000, int_comparator);
	list_to_minheap(&lnklst, heap);

    HuffmanNode* hufmTree = NULL;
    minheap_to_huffman(hufmTree ,heap);

    free(hufmTree);
	linklist_deinit(&lnklst);
	minHeap_destroy(heap);
}


void get_all_file_string_to_list(linklist *p_linklist, char* pathname)
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
					get_all_file_string_to_list(p_linklist,path);
				}
	        }
	        closedir(d);
		}
	}
	else if(S_ISREG(s.st_mode))
	{
		save_string_to_list(p_linklist, pathname);
	}
}

void build_codebook_recursive(char* pathname)
{

	linklist lnklst;
	lnklst.head.p_next=NULL;
	get_all_file_string_to_list(&lnklst, pathname);
	linklist_print(&lnklst);

	MinHeap *heap = minHeap_create(10000, int_comparator);
	list_to_minheap(&lnklst, heap);

    HuffmanNode* hufmTree = NULL;
    minheap_to_huffman(hufmTree ,heap);

    free(hufmTree);
	linklist_deinit(&lnklst);
	minHeap_destroy(heap);
}
