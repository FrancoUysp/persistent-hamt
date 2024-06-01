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
typedef struct SearchResult {
    int *values;
    int valueCount;
} SearchResult;

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
SearchResult searchHAMT(HAMT *hamt, int key);
void freeHAMT(HAMT *hamt);
SearchResult searchHAMTRec(HAMTNode *node, int key, int depth, int blockSize, int maxDepth) ;

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
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = BIT_INDEX_NODE;
    node->node.bitIndexNode.bitmap = 0;
    node->node.bitIndexNode.subnodes = calloc(MAX_CHILD, sizeof(HAMTNode*));
    return node;
}

HAMTNode* createLeafNode(int key, int value) {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = LEAF_NODE;
    node->node.leafNode.key = key;
    node->node.leafNode.values = malloc(sizeof(int));
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


SearchResult searchHAMTRec(HAMTNode *node, int key, int depth, int blockSize, int maxDepth) {
    SearchResult result = {NULL, 0};
    if (node == NULL) {
        return result;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * blockSize)) & (MAX_CHILD - 1);

    if (depth >= maxDepth - 1) {
        int bit = 1 << index;
        if (!(node->node.bitIndexNode.bitmap & bit)) {
            return result;
        }
        HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
        if (subnode != NULL && subnode->type == LEAF_NODE && subnode->node.leafNode.key == key) {
            result.values = subnode->node.leafNode.values;
            result.valueCount = subnode->node.leafNode.valueCount;
            return result;
        } else {
            return result;
        }
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (!(node->node.bitIndexNode.bitmap & bit)) {
            return result;
        } else {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            if (subnode != NULL) {
                if (subnode->type == LEAF_NODE) {
                    if (subnode->node.leafNode.key == key) {
                        result.values = subnode->node.leafNode.values;
                        result.valueCount = subnode->node.leafNode.valueCount;
                        return result;
                    } else {
                        return result;
                    }
                } else if (subnode->type == BIT_INDEX_NODE) {
                    return searchHAMTRec(subnode, key, depth + 1, blockSize, maxDepth);
                }
            }
        }
    }

    return result;
}



SearchResult searchHAMT(HAMT *hamt, int key) {
    return searchHAMTRec(hamt->root, key, 0, hamt->blockSize, hamt->maxDepth);
}



// Update operation
void updateHAMTRec(HAMTNode *node, int key, int oldValue, int newValue, int depth, int blockSize, int maxDepth) {
    if (node == NULL) {
        return;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * blockSize)) & (MAX_CHILD - 1);


    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            for (int i = 0; i < node->node.leafNode.valueCount; ++i) {
                if (node->node.leafNode.values[i] == oldValue) {
                    node->node.leafNode.values[i] = newValue;
                    return;
                }
            }
        } else {
        }
        return;
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            if (subnode != NULL) {
                updateHAMTRec(subnode, key, oldValue, newValue, depth + 1, blockSize, maxDepth);
            } else {
            }
        } else {
        }
    }
}

void updateHAMT(HAMT *hamt, int key, int oldValue, int newValue) {
    updateHAMTRec(hamt->root, key, oldValue, newValue, 0, hamt->blockSize, hamt->maxDepth);
}

HAMTNode* deleteHAMTRec(HAMTNode *node, int key, int value, int depth, int blockSize, int maxDepth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * blockSize)) & (MAX_CHILD - 1);


    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            for (int i = 0; i < node->node.leafNode.valueCount; ++i) {
                if (node->node.leafNode.values[i] == value) {
                    // Shift values to the left to overwrite the deleted value
                    for (int j = i; j < node->node.leafNode.valueCount - 1; ++j) {
                        node->node.leafNode.values[j] = node->node.leafNode.values[j + 1];
                    }
                    node->node.leafNode.valueCount--;
                    if (node->node.leafNode.valueCount == 0) {
                        // If the leaf node becomes empty, remove it from the bit index node
                        free(node->node.leafNode.values);
                        free(node);
                        return NULL;
                    }
                    return node;
                }
            }
        } else {
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            if (subnode != NULL) {
                HAMTNode *newSubnode = deleteHAMTRec(subnode, key, value, depth + 1, blockSize, maxDepth);
                if (newSubnode == NULL) {
                    node->node.bitIndexNode.bitmap &= ~bit;
                    node->node.bitIndexNode.subnodes[index] = NULL;
                }
            } else {
            }
        } else {
        }
    }
    return node;
}

void deleteHAMT(HAMT *hamt, int key, int value) {
    hamt->root = deleteHAMTRec(hamt->root, key, value, 0, hamt->blockSize, hamt->maxDepth);
}

void freeHAMTNode(HAMTNode *node) {
    if (node == NULL) return;

    if (node->type == BIT_INDEX_NODE) {
        for (int i = 0; i < MAX_CHILD; ++i) {
            if (node->node.bitIndexNode.subnodes[i] != NULL) {
                freeHAMTNode(node->node.bitIndexNode.subnodes[i]);
            }
        }
        free(node->node.bitIndexNode.subnodes);
    } else if (node->type == LEAF_NODE) {
        free(node->node.leafNode.values);
    }
    free(node);
}

void freeHAMT(HAMT *hamt) {
    if (hamt == NULL) return;
    freeHAMTNode(hamt->root);
    free(hamt);
}

int main() {
    HAMT* ham = createHAMT(MAX_LEVEL, BIT_SEG);
    char command[256];
    
    while (fgets(command, sizeof(command), stdin) != NULL) {
        char operation[10];
        int key, value, oldValue, newValue;
        sscanf(command, "%s", operation);

        if (strcmp(operation, "insert") == 0) {
            sscanf(command, "%*s %d %d", &key, &value);
            insertHAMT(ham, key, value);
        } else if (strcmp(operation, "update") == 0) {
            sscanf(command, "%*s %d %d %d", &key, &oldValue, &newValue);
            updateHAMT(ham, key, oldValue, newValue);
        } else if (strcmp(operation, "delete") == 0) {
            sscanf(command, "%*s %d %d", &key, &value);
            deleteHAMT(ham, key, value);
        } else if (strcmp(operation, "search") == 0) {
            sscanf(command, "%*s %d", &key);
            SearchResult result = searchHAMT(ham, key);
            if (result.values != NULL) {
                printf("Values for key %d: ", key);
                for (int i = 0; i < result.valueCount; ++i) {
                    printf("%d ", result.values[i]);
                }
                printf("\n");
            } else {
                printf("Key %d not found.\n", key);
            }
        } else if (strcmp(operation, "print") == 0) {
            printHAMT(ham);
        }
    }

    freeHAMT(ham);
    return 0;
}


