#include <stdlib.h>

#include "btree.h"


BTree *binaryTree_create(void *item) {
    BTree *tree = malloc(sizeof(BTree));
    if (tree == NULL) {
        return NULL;
    }
    tree->item = item;
    tree->left = NULL;
    tree->right = NULL;
    return tree;
}


void binaryTree_destroy(BTree *tree, void (*destroy)(void *)) {
    if (tree->left != NULL) {
        binaryTree_destroy(tree->left, destroy);
        tree->left = NULL;
    }
    if (tree->right != NULL) {
        binaryTree_destroy(tree->right, destroy);
        tree->right = NULL;
    }
    destroy(tree->item);
    free(tree);
}


int binaryTree_is_leef(BTree *tree) {
    return tree->left == NULL && tree->right == NULL;
}
