#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

#define EMPTY_STR ""

//Authors: Michael Tang, Stephen Scott

typedef struct node
{
	int index;
	char str[512];
	struct node *next;
}node;

typedef struct
{
	node head;
}linklist;

void linklist_insert_head(linklist *p_linklist,int index, char *str)
{
	node *p_node=(node*)malloc(sizeof(node));
	if(!p_node){
		printf("ERROR: Could not allocate space with malloc.");
		return ;}
	p_node->index=index;
	strcpy(p_node->str, str);
	p_node->next=NULL;
	p_node->next=p_linklist->head.next;
	p_linklist->head.next=p_node;
}

void linklist_remove_head(linklist *p_linklist)
{
	if(p_linklist->head.next)
	{
		node *p_node=p_linklist->head.next;
		p_linklist->head.next=p_node->next;
		free(p_node);
		p_node=NULL;
	}
}

void for_each(linklist *p_linklist, int(*p_func)(node *, void *),void *p_data)
{
	node *p_node=NULL;
	for(p_node=&(p_linklist->head);p_node;p_node=p_node->next)
	{
		if(p_func(p_node ,p_data))
		{
			break;
		}
	}
}

int linklist_get_node(linklist *p_linklist, int index, char* pstr)
{
	node *p_node=NULL;
	for(p_node=&(p_linklist->head);p_node;p_node=p_node->next)
	{
		if(p_node->next->index == index)
		{
			strcpy(pstr, p_node->next->str);
			return 0;
		}
	}
	return -1;
}

int linklist_modify_node(linklist *p_linklist, int index, char * pstr)
{
	node *p_node=NULL;
	for(p_node=&(p_linklist->head);p_node;p_node=p_node->next)
	{
		if(p_node->next->index == index)
		{
			strcpy(p_node->next->str, pstr);
			return 0;
		}
	}
	return -1;
}

static int print_callback(node *p_node, void *p_data)
{
	if(p_node->next)
	{
		printf("%s\n",p_node->next->str);
	}
	return 0;
}

void linklist_print(linklist *p_linklist)//
{
	for_each(p_linklist,print_callback, NULL);
	printf("\n");
}

void linklist_deinit(linklist *p_linklist)
{
	while(p_linklist->head.next)
	{
		linklist_remove_head(p_linklist);
	}
}

//Int comparaator
int numericSort(void *str1, void *str2)
{
	if(atoi((char*)str1) > atoi((char*)str2))
		return 1;
	else if(atoi((char*)str1) < atoi((char*)str2))
		return -1;
	else
		return 0;

}
//String comparator
int charSort(void* strptr1, void* strptr2)
{	
	char str1[512];
	char str2[512];
	int i = 0;
	strcpy(str1, (char*)strptr1);
	strcpy(str2, (char*)strptr2);
	for (i = 0; i<512; i++){
	if (str1[i] > str2[i]){
		return 1;}
	else if (str1[i] < str2[i]){
		return -1;}}
	return 0;

}

int insertionSort( void* toSort, int (*comparator)(void*, void*))
{
	linklist* p_linklist = toSort;
	int i,j;
	char temp[512];
	char pstr[512];
	node *p_node=NULL;
	p_node=&(p_linklist->head);

	for(i=1; i < p_node->next->index; i++)
	{
		memset(temp,'\0',512);
		strcpy(temp,EMPTY_STR);
		linklist_get_node(p_linklist, (p_node->next->index - i),temp);

		for(j=i; j>0 ; j--)
		{
			memset(pstr,'\0',512);
			strcpy(pstr,EMPTY_STR);
			linklist_get_node(p_linklist, (p_node->next->index - (j - 1)),pstr);
			if(comparator((void*)temp,(void*)pstr)<0)
			{
				linklist_modify_node(p_linklist, p_node->next->index - j, pstr);
			}
			else
				break;
		}
		linklist_modify_node(p_linklist, p_node->next->index - j, temp);
	}

	return 0;
}

int quickSortHelp( void* toSort,int l,int r, int (*comparator)(void*, void*))
{
	linklist* p_linklist = toSort;
	    if (l < r)
	    {
	        int i = l, j = r;
	        char temp[512];
	        char pstr[512];
	    	node *p_node=NULL;
	    	p_node=&(p_linklist->head);

			memset(temp,'\0',512);
			strcpy(temp,EMPTY_STR);
	    	linklist_get_node(p_linklist, (p_node->next->index - l), temp);

	        while (i < j)
	        {
	            while(i < j)
	            {
	    			memset(pstr,'\0',512);
	    			strcpy(pstr,EMPTY_STR);
	    	    	linklist_get_node(p_linklist, (p_node->next->index - j), pstr);
	            	if(comparator(pstr, temp) >= 0)
	            		j--;
	            	else
	            		break;
	            }

	            if(i < j)
	            {
	                linklist_modify_node(p_linklist, p_node->next->index - i++, pstr);
	            }
	            while(i < j)
	            {
	            	memset(pstr,'\0',512);
	            	strcpy(pstr,EMPTY_STR);
	            	linklist_get_node(p_linklist, (p_node->next->index - i), pstr);
	            	if(comparator(pstr,temp)<0)
	            	{
	            		i++;
	            	}
	            	else
	            		break;
	            }
	            if(i < j)
	            {
	                linklist_modify_node(p_linklist, p_node->next->index - j--, pstr);
	            }
	        }
	        linklist_modify_node(p_linklist, p_node->next->index - i, temp);
	        quickSortHelp(p_linklist, l, i - 1,comparator);
	        quickSortHelp(p_linklist, i + 1, r,comparator);
	    }

	return 0;
}
int quickSort( void* toSort, int (*comparator)(void*, void*))
{
	linklist* p_linklist = toSort;
	node *p_node=NULL;
	p_node=&(p_linklist->head);
	int l = 0;
	int r = p_node->next->index - 1;
	return quickSortHelp(p_linklist,l,r, comparator);

}

int main(int argc, char* argv[])
{
	int fd;
	char ch;
	int flag[2]= {0};
	int buf_index = 0;
	linklist lnklst;
	lnklst.head.next=NULL;
	int str_index = 0;
	
	if(argc < 3)
	{
		printf("Fatal Error: expected two arguments, had one\r\n");
		exit(-1);
	}
	if((access(argv[2],F_OK)) == -1)
	{
		printf("Fatal Error: file \"%s\" does not exist\r\n",argv[2]);
		exit(-1);
	}
	
	char sortFlag1 = argv[1][0];
	char sortFlag2 = argv[1][1];
	int flagLength = strlen(argv[1]);

//open  read close +++
	if((fd=open(argv[2] ,O_RDONLY, 0666))==-1){
		perror(argv[2]),exit(-1);}
	
	int sizeOfFile = lseek(fd, 0, SEEK_END);
	lseek(fd, 0, SEEK_SET);
	char buf[sizeOfFile];
	
	if(sizeOfFile == 0){
	printf("WARNING: Empty input file");
	return 0;}

	while(read(fd, &ch, 1) > 0)
	{	
		
	
		if((ch <= 'z' && ch >= 'a') || (ch <= '9'&&ch >='0') || ch == ',')
		{
			if(ch <= 'z' && ch >= 'a')
				flag[0] = 1;
			if(ch <= '9'&&ch >='0')
				flag[1] = 2;
			if(ch == ',')
			{
				linklist_insert_head(&lnklst, ++str_index, buf);
				memset(buf,'\0',512);
				strcpy(buf,EMPTY_STR);
				buf_index = 0;
			}else
			{
				buf[buf_index++] = ch;
			}
		}
	}
	if(strlen(buf) != 0)
	{
		linklist_insert_head(&lnklst, ++str_index, buf);
		memset(buf,'\0',512);
		strcpy(buf,EMPTY_STR);
		buf_index = 0;
	}
	close(fd);
	//open  read close ---
	

	if(sortFlag1 == '-' && sortFlag2 == 'i' && flagLength == 2)
	{
		if(flag[0] + flag[1] == 1)
			insertionSort(&lnklst, charSort);
		else if(flag[0] + flag[1] == 2)
			insertionSort(&lnklst, numericSort);
		else
		{
			printf("Fatal Error: The input string contains numbers and characters\r\n");
			exit(-1);
		}

	}
	else if(sortFlag1 == '-' && sortFlag2 == 'q' && flagLength == 2)
	{
		if(flag[0] + flag[1] == 1)
			quickSort(&lnklst, charSort);
		else if(flag[0] + flag[1] == 2)
			quickSort(&lnklst, numericSort);
		else
		{	
			printf("Fatal Error: The input string contains numbers and characters\r\n");
			exit(-1);
		}
	}else
	{
		printf("Fatal Error: You input the wrong option\r\n");
		exit(-1);
	}

	linklist_print(&lnklst);
	linklist_deinit(&lnklst);

	return 0;
}