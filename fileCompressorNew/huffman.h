#ifndef HUFFMAN_H_
#define HUFFMAN_H_

typedef struct HuffmanTree
{
	int weight;
	int id;
    struct HuffmanTree* lchild;
    struct HuffmanTree* rchild;
}HuffmanNode;

int mymain();

HuffmanNode* HuffmancodeToHuffmanTree(char **, int );
HuffmanNode* createHuffmanTree(int* , int );
void PrintHuffmanTree(HuffmanNode* );
void HuffmanCode(HuffmanNode* , int ,char**,int );
void HuffmanDecode(char*, HuffmanNode* , char**, int);
#endif /* HUFFMAN_H_ */
