#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "huffman.h"

HuffmanNode* createHuffmanTree(int* a, int n)
{
    int i, j;
    HuffmanNode **temp, *hufmTree;
    temp = malloc(n*sizeof(HuffmanNode));
    for (i=0; i<n; ++i)
    {
        temp[i] = (HuffmanNode*)malloc(sizeof(HuffmanNode));
        temp[i]->weight = a[i];
        temp[i]->id = i;
        temp[i]->lchild = temp[i]->rchild = NULL;
    }

    for (i=0; i<n-1; ++i)
    {
        int small1=-1, small2;
        for (j=0; j<n; ++j)
        {
            if (temp[j] != NULL && small1==-1)
            {
                small1 = j;
                continue;
            } else if(temp[j] != NULL)
            {
                small2 = j;
                break;
            }
        }

        for (j=small2; j<n; ++j)
        {
            if (temp[j] != NULL)
            {
                if (temp[j]->weight < temp[small1]->weight)
                {
                    small2 = small1;
                    small1 = j;
                } else if (temp[j]->weight < temp[small2]->weight)
                {
                    small2 = j;
                }
            }
        }
        hufmTree = (HuffmanNode*)malloc(sizeof(HuffmanNode));
        hufmTree->weight = temp[small1]->weight + temp[small2]->weight;
        hufmTree->lchild = temp[small1];
        hufmTree->rchild = temp[small2];

        temp[small1] = hufmTree;
        temp[small2] = NULL;
    }
    free(temp);
    return hufmTree;
}

void PrintHuffmanTree(HuffmanNode* hufmTree)
{
    if (hufmTree)
    {
    	printf("%d", hufmTree->weight);
        if (hufmTree->lchild != NULL || hufmTree->rchild != NULL)
        {
            printf("(");
            PrintHuffmanTree(hufmTree->lchild);
            printf(",");
            PrintHuffmanTree(hufmTree->rchild);
            printf(")");
        }
    }
}

void HuffmanCode(HuffmanNode* hufmTree, int depth, char**pwords, int fd)
{
    static int code[10];
    static char buf[4][1] = {"0", "1","\t","\n"};
    if (hufmTree)
    {
        if (hufmTree->lchild==NULL && hufmTree->rchild==NULL)
        {
            int i;
            printf("%s		%d		 %d		",pwords[hufmTree->id],hufmTree->weight, hufmTree->id);
            for (i=0; i<depth; ++i)
            {
                printf("%d", code[i]);
                write(fd, buf[code[i]], 1);
            }
            write(fd, buf[2], 1);
            write(fd, pwords[hufmTree->id], strlen(pwords[hufmTree->id]));
            free(pwords[hufmTree->id]);
            pwords[hufmTree->id] = NULL;
            write(fd, buf[3], 1);
            printf("\n");
        } else
        {
            code[depth] = 0;
            HuffmanCode(hufmTree->lchild, depth+1, pwords, fd);
            code[depth] = 1;
            HuffmanCode(hufmTree->rchild, depth+1, pwords, fd);
        }
    }
}

HuffmanNode* HuffmancodeToHuffmanTree(char **pphuffmancode, int n)
{
    int i, j;
    HuffmanNode *hufmTree;
    hufmTree = malloc(sizeof(HuffmanNode));
    hufmTree->id = -1;
    hufmTree->lchild = NULL;
    hufmTree->rchild = NULL;


    for (i=0; i<n; i++)
    {
    	HuffmanNode * tempHuffmanNode = hufmTree;
    	for(j = 0; j < strlen(pphuffmancode[i]); j++)
    	{
    		if(pphuffmancode[i][j] == '0')  //left
    		{
    			if(tempHuffmanNode->lchild)
    			{
    				tempHuffmanNode = tempHuffmanNode->lchild;
    			}
    			else
    			{
    				if(j == strlen(pphuffmancode[i])-1)
					{
    					tempHuffmanNode->lchild = malloc(sizeof(HuffmanNode));
    					tempHuffmanNode = tempHuffmanNode->lchild;
    					tempHuffmanNode->id = i;
    					tempHuffmanNode->lchild = NULL;
    					tempHuffmanNode->rchild = NULL;
					}
    				else
    				{
    					tempHuffmanNode->lchild = malloc(sizeof(HuffmanNode));
    					tempHuffmanNode = tempHuffmanNode->lchild;
    					tempHuffmanNode->id = -1;
    					tempHuffmanNode->lchild = NULL;
    					tempHuffmanNode->rchild = NULL;
    				}
    			}
    		}
    		else //right
    		{
    			if(tempHuffmanNode->rchild)
    			{
    				tempHuffmanNode = tempHuffmanNode->rchild;
    			}
    			else
    			{
    				if(j == strlen(pphuffmancode[i])-1)
					{
    					tempHuffmanNode->rchild = malloc(sizeof(HuffmanNode));
    					tempHuffmanNode = tempHuffmanNode->rchild;
    					tempHuffmanNode->id = i;
    					tempHuffmanNode->lchild = NULL;
    					tempHuffmanNode->rchild = NULL;
					}
    				else
    				{
    					tempHuffmanNode->rchild = malloc(sizeof(HuffmanNode));
    					tempHuffmanNode = tempHuffmanNode->rchild;
    					tempHuffmanNode->id = -1;
    					tempHuffmanNode->lchild = NULL;
    					tempHuffmanNode->rchild = NULL;
    				}
    			}
    		}
    	}
    }

//    printf("hufmTree->rchild->rchild->lchild->lchild->id = %d\n",hufmTree->rchild->rchild->lchild->lchild->id);
    return hufmTree;
}
// 哈夫曼解码
void HuffmanDecode(char ch[], HuffmanNode* hufmTree, char**pwords, int fd)
{
    int i;
    int num[512] = {};
    HuffmanNode* tempTree = NULL;
    for (i=0; i<strlen(ch); ++i)
    {
        if (ch[i] == '0')
            num[i] = 0;
        else
            num[i] = 1;
    }
    if(hufmTree)
    {
        i = 0;
        while(i<strlen(ch))
        {
            tempTree = hufmTree;
            while(tempTree->lchild!=NULL && tempTree->rchild!=NULL)
            {
                if (num[i] == 0)
                {
                    tempTree = tempTree->lchild;
                } else
                {
                    tempTree = tempTree->rchild;
                }
                i++;
            }
//            printf("%s", pwords[tempTree->id]);
            write(fd, pwords[tempTree->id], strlen(pwords[tempTree->id]));
        }
    }
}
