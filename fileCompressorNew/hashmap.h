#ifndef HASHMAP_H_
#define HASHMAP_H_


typedef struct node
{
	int fre;
	char *str;
	int sizeofstr;
	struct node *p_next;
}node;

typedef struct nodecontent
{
	int fre;
	char *str;
}nonodecontentde;

typedef struct
{
	node head;
}linklist;

void linklist_insert_head(linklist *,int , char *, int);
int linklist_pop_head(linklist *, int *,char*);
int linklist_update_node(linklist * , char *);
void linklist_print(linklist *);
void linklist_deinit(linklist *);

#endif /* HASHMAP_H_ */
