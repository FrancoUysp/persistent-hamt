#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define BIT_SEG 4
#define MAX_CHILD (1 << BIT_SEG)
#define MAX_LEVEL (32 / BIT_SEG)

typedef enum { BIT_INDEX_NODE, LEAF_NODE } NodeType;

typedef struct BitIndexNode {
    int bitmap;
    struct HAMTNode **subnodes;
} BitIndexNode;

typedef struct LeafNode {
    int key;
    int *values;
    int valueCount;
} LeafNode;

typedef struct HAMTNode {
    NodeType type;
    union {
        BitIndexNode bitIndexNode;
        LeafNode leafNode;
    } node;
} HAMTNode;

typedef struct HAMT {
    HAMTNode *root;
    int maxDepth;
    int blockSize;
} HAMT;

HAMT* createHAMT(int maxDepth, int blockSize);
void insertHAMT(HAMT *hamt, int key, int value);
int searchHAMT(HAMT *hamt, int key);
void freeHAMT(HAMT *hamt);

HAMTNode* createBitIndexNode();
HAMTNode* createLeafNode(int key, int value);
void insertIntoBitIndexNode(BitIndexNode *bitIndexNode, HAMTNode *subnode, int index);
void printHAMT(HAMT *hamt);
void printHAMTRec(HAMTNode *node, int depth);
void printBitmapBinary(int bitmap);


HAMT* createHAMT(int maxDepth, int blockSize) {
    HAMT* new_hamt = (HAMT*)(calloc(1, sizeof(HAMT)));
    new_hamt->root = createBitIndexNode();
    new_hamt->maxDepth = maxDepth;
    new_hamt->blockSize = blockSize;
    return new_hamt;
}

HAMTNode* createBitIndexNode() {
    HAMTNode *node = (HAMTNode*)malloc(sizeof(HAMTNode));
    node->type = BIT_INDEX_NODE;
    node->node.bitIndexNode.bitmap = 0;
    node->node.bitIndexNode.subnodes = (HAMTNode**)calloc(MAX_CHILD, sizeof(HAMTNode*));
    return node;
}

HAMTNode* createLeafNode(int key, int value) {
    HAMTNode *node = (HAMTNode*)malloc(sizeof(HAMTNode));
    node->type = LEAF_NODE;
    node->node.leafNode.key = key;
    node->node.leafNode.values = (int*)malloc(sizeof(int));
    node->node.leafNode.values[0] = value;
    node->node.leafNode.valueCount = 1;
    return node;
}

unsigned int hashFunction(int key) {
    // Simple hash function for demonstration purposes
    return key;
}

void insertHAMTRec(HAMTNode *node, int key, int value, int depth, int blockSize, int maxDepth) {
    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * blockSize)) & (MAX_CHILD - 1);

    if (depth >= maxDepth-1) {
        // Check the subnode at index
        int bit = 1 << index;

        // If the bitmap indicates the position is empty, create a new leaf node
        if (!(node->node.bitIndexNode.bitmap & bit)) {
            node->node.bitIndexNode.bitmap |= bit;
            node->node.bitIndexNode.subnodes[index] = createLeafNode(key, value);
        } else {
            // If the position is not empty, append the value to the existing leaf node
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            if (subnode->type == LEAF_NODE) {
                subnode->node.leafNode.values = (int*)realloc(subnode->node.leafNode.values, (subnode->node.leafNode.valueCount + 1) * sizeof(int));
                subnode->node.leafNode.values[subnode->node.leafNode.valueCount++] = value;
            } else {
                // This case should not happen
                printf("Unexpected bit index node at max depth.\n");
            }
        }
        return;
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (!(node->node.bitIndexNode.bitmap & bit)) {
            // Case 1: Subnode is empty, create a leaf node
            node->node.bitIndexNode.bitmap |= bit;
            node->node.bitIndexNode.subnodes[index] = createLeafNode(key, value);
        } else {
            // Subnode is not empty
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            if (subnode->type == LEAF_NODE) {
                // Transform the leaf node into a bit index node
                HAMTNode *newBitIndexNode = createBitIndexNode();

                unsigned int existingHash = hashFunction(subnode->node.leafNode.key);
                int existingIndex = (existingHash >> ((depth + 1) * blockSize)) & (MAX_CHILD - 1);

                // Move the existing leaf node into the new bit index node
                newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
                newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = subnode;

                // Update the parent node to point to the new bit index node
                node->node.bitIndexNode.subnodes[index] = newBitIndexNode;

                // Reinsert the existing values into the new bit index node
                for (int i = 1; i < subnode->node.leafNode.valueCount; ++i) {
                    insertHAMTRec(newBitIndexNode->node.bitIndexNode.subnodes[existingIndex], subnode->node.leafNode.key, subnode->node.leafNode.values[i], depth + 1, blockSize, maxDepth);
                }

                // Insert the new key-value pair into the new bit index node
                insertHAMTRec(newBitIndexNode, key, value, depth + 1, blockSize, maxDepth);
            } else if (subnode->type == BIT_INDEX_NODE) {
                // Case 2: Subnode is already a bit index node, recurse down
                insertHAMTRec(subnode, key, value, depth + 1, blockSize, maxDepth);
            }
        }
    }   
}

void insertHAMT(HAMT *hamt, int key, int value) {
    insertHAMTRec(hamt->root, key, value, 0, hamt->blockSize, hamt->maxDepth);
}

void printBitmapBinary(int bitmap) {
    for (int i = MAX_CHILD - 1; i >= 0; --i) {
        printf("%d", (bitmap >> i) & 1);
    }
}

void printHAMTRec(HAMTNode *node, int depth) {
    if (node == NULL) {
        return;
    }

    printf("Level %d:\n", depth);
    if (node->type == BIT_INDEX_NODE) {
        printf("  Bitmap: ");
        printBitmapBinary(node->node.bitIndexNode.bitmap);
        printf("\n  Nodes: [");
        for (int i = 0; i < MAX_CHILD; ++i) {
            if (node->node.bitIndexNode.subnodes[i] != NULL) {
                if (node->node.bitIndexNode.subnodes[i]->type == LEAF_NODE) {
                    printf("Leaf");
                } else {
                    printf("BitIndex");
                }
            } else {
                printf("empty");
            }
            if (i < MAX_CHILD - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }

    if (node->type == LEAF_NODE) {
        printf("  LeafNode: key = %d, values = [", node->node.leafNode.key);
        for (int i = 0; i < node->node.leafNode.valueCount; ++i) {
            printf("%d", node->node.leafNode.values[i]);
            if (i < node->node.leafNode.valueCount - 1) {
                printf(", ");
            }
        }
        printf("]\n");
    }

    if (node->type == BIT_INDEX_NODE) {
        for (int i = 0; i < MAX_CHILD; ++i) {
            if (node->node.bitIndexNode.subnodes[i] != NULL) {
                printHAMTRec(node->node.bitIndexNode.subnodes[i], depth + 1);
            }
        }
    }
}

void printHAMT(HAMT *hamt) {
    if (hamt == NULL || hamt->root == NULL) {
        printf("HAMT is empty.\n");
        return;
    }
    printHAMTRec(hamt->root, 0);
}


int main() {
    HAMT* ham = createHAMT(MAX_LEVEL, BIT_SEG);
    insertHAMT(ham, 24, 100);
    insertHAMT(ham, 21, 200);
    insertHAMT(ham, 234, 150); // Insert with the same key to demonstrate leaf node transformation
    insertHAMT(ham, 12, 250); // Insert additional value with the same key at max depth
    insertHAMT(ham, 234, 150); // Insert with the same key to demonstrate leaf node transformation
    insertHAMT(ham, 351, 250); // Insert additional value with the same key at max depth
    insertHAMT(ham, 607, 250); // Insert additional value with the same key at max depth
    printHAMT(ham);
    return 0;
}
