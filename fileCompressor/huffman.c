//Does huffman Coding
typedef struct {
    char *token;
    int frequency;
} Node;

Node *BT_get_data(BTree *tree) {
  return (Node *)(tree->item);
}

BTree *huffmanTree(DLL *tokens, FrequencyMap *fm) {
    MinHeap *heap = MH_create(tokens->size, BT_comparator);
    DLL_node *node;
    for (node = tokens->head; node != NULL; node = node->next) {
        Node *data = malloc(sizeof(Node));
        data->token = node->data;
        data->frequency = frequencyMap_frequency(fm, node->data);
        BTree *tree = BT_create(data);
        minHeap_insert(heap, tree);
    }

    // Runs minheap untill only one tree left
    // Pops first 2 in min heap which are the lowest value
    while (minHeap_size(heap) > 1) {
        // Creates tree
        BTree *tree, *left, *right;
        // gets first two
        left = minHeap_pop(heap);
        right = minHeap_pop(heap);


        Node *data = malloc(sizeof(Node));
        data->token = NULL;
        int lfrequency = BT_get_data(left)->frequency;
        int rfrequency = BT_get_data(right)->frequency;
        data->frequency = lfrequency + rfrequency;

        // Creates tree
        tree = binaryTree_create(data);
        tree->left = left;
        tree->right = right;
        // Insert tree back into minHeap to be compared
        minHeap_insert(heap, tree);
    }

    BTree *tree = minHeap_pop(heap);

    // Edge case
    if (binaryTreey_is_leaf(tree)) {
        BTree *dummy = BT_create(malloc(sizeof(Node)));
        dummy->left = tree;
        tree = dummy;
    }

    minHeap_destroy(heap);
    return tree;
}
