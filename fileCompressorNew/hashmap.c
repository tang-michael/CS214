#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "hashmap.h"

void linklist_insert_head(linklist *p_linklist,int fre, char *str, int sizeofstr)//从头插入
{
	node *p_node=(node*)malloc(sizeof(node));
	if(!p_node)
	return ;
	p_node->fre=fre;
	p_node->str = malloc(sizeofstr);
	strcpy(p_node->str, str);
	p_node->p_next=NULL;
	p_node->p_next=p_linklist->head.p_next;
	p_linklist->head.p_next=p_node;
}

int linklist_pop_head(linklist *p_linklist, int *pfre, char*pstr)//把头移除
{
	if(p_linklist->head.p_next)//p_linklist->head.p_next所指的就是第一个节点
	{
		node *p_node=p_linklist->head.p_next;
		strcpy(pstr, p_node->str);
		*pfre = p_node->fre;
		p_linklist->head.p_next=p_node->p_next;
		free(p_node->str);
		p_node->str = NULL;
		free(p_node);
		p_node=NULL;
		return 0;
	}
	return -1;
}

void for_each(linklist *p_linklist, int(*p_func)(node *, void *),void *p_data)
{
	node *p_node=NULL;
	for(p_node=&(p_linklist->head);p_node;p_node=p_node->p_next)
	{
		if(p_func(p_node ,p_data))//如果回调函数返回值为1，则for_each函数结束
		{
			break;
		}
	}
}
int linklist_update_node(linklist *p_linklist, char * pstr)
{
	node *p_node=NULL;
	for(p_node=&(p_linklist->head);p_node;p_node=p_node->p_next)
	{
		if(p_node->p_next)
		{
			if(strcmp(p_node->p_next->str, pstr) == 0)
			{
				p_node->p_next->fre = p_node->p_next->fre + 1;
				return 0;
			}
		}else
			return -1;
	}
	return -1;
}

static int print_callback(node *p_node, void *p_data)
{
	if(p_node->p_next)
	{
		printf("\"%s\"	%d\n",p_node->p_next->str, p_node->p_next->fre);
	}
	return 0;
}

void linklist_print(linklist *p_linklist)//
{
	for_each(p_linklist,print_callback, NULL);
}

void linklist_deinit(linklist *p_linklist)
{
	char *pstr = NULL;
	int fre;
	while(p_linklist->head.p_next)
	{
		linklist_pop_head(p_linklist,&fre, pstr);
	}
}
