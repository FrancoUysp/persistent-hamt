#include "hamt.h"

VersionedHAMT* createVersionedHAMT(int maxDepth, int blockSize) {
    VersionedHAMT* vhamt = (VersionedHAMT*)(calloc(1, sizeof(VersionedHAMT)));
    vhamt->versions = (HAMT**)(calloc(1, sizeof(HAMT*)));
    vhamt->versions[0] = createHAMT(maxDepth, blockSize);
    vhamt->versionCount = 1;
    vhamt->currentVersion = 0;
    return vhamt;
}

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

HAMTNode* copyHAMTNode(HAMTNode *node) {
    if (node == NULL) return NULL;
    HAMTNode *newNode = malloc(sizeof(HAMTNode));
    newNode->type = node->type;
    if (node->type == BIT_INDEX_NODE) {
        newNode->node.bitIndexNode.bitmap = node->node.bitIndexNode.bitmap;
        newNode->node.bitIndexNode.subnodes = malloc(MAX_CHILD * sizeof(HAMTNode*));
        for (int i = 0; i < MAX_CHILD; ++i) {
            if (node->node.bitIndexNode.subnodes[i] != NULL) {
                newNode->node.bitIndexNode.subnodes[i] = copyHAMTNode(node->node.bitIndexNode.subnodes[i]);
            } else {
                newNode->node.bitIndexNode.subnodes[i] = NULL;
            }
        }
    } else if (node->type == LEAF_NODE) {
        newNode->node.leafNode.key = node->node.leafNode.key;
        newNode->node.leafNode.valueCount = node->node.leafNode.valueCount;
        newNode->node.leafNode.values = malloc(node->node.leafNode.valueCount * sizeof(int));
        memcpy(newNode->node.leafNode.values, node->node.leafNode.values, node->node.leafNode.valueCount * sizeof(int));
    }
    return newNode;
}


void deleteHAMT(VersionedHAMT *vhamt, int key, int value) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT(vhamt->versions[latestVersion]->maxDepth, vhamt->versions[latestVersion]->blockSize);
    newHamt->root = copyHAMTNode(vhamt->versions[latestVersion]->root);
    newHamt->root = deleteHAMTRec(newHamt->root, key, value, 0, newHamt->blockSize, newHamt->maxDepth);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
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
                        // If the leaf node becomes empty, remove it
                        free(node->node.leafNode.values);
                        free(node);
                        return NULL;
                    }
                    return node;
                }
            }
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            HAMTNode *newSubnode = deleteHAMTRec(subnode, key, value, depth + 1, blockSize, maxDepth);
            if (newSubnode == NULL) {
                node->node.bitIndexNode.bitmap &= ~bit;
                node->node.bitIndexNode.subnodes[index] = NULL;
            }
            return node;
        }
    }
    return node;
}

void updateHAMT(VersionedHAMT *vhamt, int key, int oldValue, int newValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT(vhamt->versions[latestVersion]->maxDepth, vhamt->versions[latestVersion]->blockSize);
    newHamt->root = copyHAMTNode(vhamt->versions[latestVersion]->root);
    updateHAMTRec(newHamt->root, key, oldValue, newValue, 0, newHamt->blockSize, newHamt->maxDepth);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}
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
        }
        return;
    }

    if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            updateHAMTRec(subnode, key, oldValue, newValue, depth + 1, blockSize, maxDepth);
        }
    }
}

void insertHAMTRec(HAMTNode **node, int key, int value, int depth, int blockSize, int maxDepth) {
    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * blockSize)) & (MAX_CHILD - 1);

    if (*node == NULL) {
        *node = createLeafNode(key, value);
        return;
    }

    if (depth >= maxDepth - 1) {
        if ((*node)->type == LEAF_NODE) {
            if ((*node)->node.leafNode.key == key) {
                (*node)->node.leafNode.values = realloc((*node)->node.leafNode.values, ((*node)->node.leafNode.valueCount + 1) * sizeof(int));
                (*node)->node.leafNode.values[(*node)->node.leafNode.valueCount++] = value;
            } else {
                HAMTNode *newBitIndexNode = createBitIndexNode();
                unsigned int existingHash = hashFunction((*node)->node.leafNode.key);
                int existingIndex = (existingHash >> (depth * blockSize)) & (MAX_CHILD - 1);
                newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
                newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = *node;
                *node = newBitIndexNode;
                insertHAMTRec(node, key, value, depth, blockSize, maxDepth);
            }
        } else {
            int bit = 1 << index;
            if (!((*node)->node.bitIndexNode.bitmap & bit)) {
                (*node)->node.bitIndexNode.bitmap |= bit;
                (*node)->node.bitIndexNode.subnodes[index] = createLeafNode(key, value);
            } else {
                insertHAMTRec(&((*node)->node.bitIndexNode.subnodes[index]), key, value, depth + 1, blockSize, maxDepth);
            }
        }
    } else {
        if ((*node)->type == BIT_INDEX_NODE) {
            int bit = 1 << index;
            if (!((*node)->node.bitIndexNode.bitmap & bit)) {
                (*node)->node.bitIndexNode.bitmap |= bit;
                (*node)->node.bitIndexNode.subnodes[index] = createLeafNode(key, value);
            } else {
                insertHAMTRec(&((*node)->node.bitIndexNode.subnodes[index]), key, value, depth + 1, blockSize, maxDepth);
            }
        } else {
            HAMTNode *newBitIndexNode = createBitIndexNode();
            unsigned int existingHash = hashFunction((*node)->node.leafNode.key);
            int existingIndex = (existingHash >> (depth * blockSize)) & (MAX_CHILD - 1);
            newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
            newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = *node;
            *node = newBitIndexNode;
            insertHAMTRec(node, key, value, depth, blockSize, maxDepth);
        }
    }
}

void insertHAMT(VersionedHAMT *vhamt, int key, int value) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT(vhamt->versions[latestVersion]->maxDepth, vhamt->versions[latestVersion]->blockSize);
    newHamt->root = copyHAMTNode(vhamt->versions[latestVersion]->root);
    insertHAMTRec(&(newHamt->root), key, value, 0, newHamt->blockSize, newHamt->maxDepth);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
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

SearchResult searchHAMT(VersionedHAMT *vhamt, int key, int version) {
    if (version >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", version);
        SearchResult result = {NULL, 0};
        return result;
    }
    return searchHAMTRec(vhamt->versions[version]->root, key, 0, vhamt->versions[version]->blockSize, vhamt->versions[version]->maxDepth);
}

void printBitmapBinary(int bitmap) {
    for (int i = MAX_CHILD - 1; i >= 0; --i) {
        printf("%d", (bitmap >> i) & 1);
    }
}

void enqueue(QueueNode **head, HAMTNode *node, int depth) {
    QueueNode *newNode = malloc(sizeof(QueueNode));
    newNode->node = node;
    newNode->depth = depth;
    newNode->next = NULL;

    if (*head == NULL) {
        *head = newNode;
    } else {
        QueueNode *current = *head;
        while (current->next != NULL) {
            current = current->next;
        }
        current->next = newNode;
    }
}

QueueNode *dequeue(QueueNode **head) {
    if (*head == NULL) return NULL;
    QueueNode *dequeuedNode = *head;
    *head = (*head)->next;
    return dequeuedNode;
}

void printHAMT(VersionedHAMT *vhamt, int version) {
    if (version >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", version);
        return;
    }
    HAMT *hamt = vhamt->versions[version];
    if (hamt == NULL || hamt->root == NULL) {
        printf("HAMT is empty.\n");
        return;
    }

    QueueNode *queue = NULL;
    enqueue(&queue, hamt->root, 0);

    int currentDepth = 0;
    printf("Level %d:\n", currentDepth);

    while (queue != NULL) {
        QueueNode *node = dequeue(&queue);
        if (node->depth > currentDepth) {
            currentDepth = node->depth;
            printf("\nLevel %d:\n", currentDepth);
        }

        if (node->node->type == BIT_INDEX_NODE) {
            printf("  --HAMT NODE--\n ");
            printf("  Bitmap: ");
            printBitmapBinary(node->node->node.bitIndexNode.bitmap);
            printf("\n  Nodes: [");
            for (int i = 0; i < MAX_CHILD; ++i) {
                if (node->node->node.bitIndexNode.subnodes[i] != NULL) {
                    if (node->node->node.bitIndexNode.subnodes[i]->type == LEAF_NODE) {
                        printf("Leaf");
                    } else {
                        printf("BitIndex");
                        enqueue(&queue, node->node->node.bitIndexNode.subnodes[i], currentDepth + 1);
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

        if (node->node->type == LEAF_NODE) {
            printf("  LeafNode: key = %d, values = [", node->node->node.leafNode.key);
            for (int i = 0; i < node->node->node.leafNode.valueCount; ++i) {
                printf("%d", node->node->node.leafNode.values[i]);
                if (i < node->node->node.leafNode.valueCount - 1) {
                    printf(", ");
                }
            }
            printf("]\n");
        }

        free(node);
    }
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

void printVersions(VersionedHAMT *vhamt) {
    printf("Versions:\n");
    for (int i = 0; i < vhamt->versionCount; ++i) {
        printf("Version %d\n", i);
    }
    printf("Current Version: %d\n", vhamt->currentVersion);
}


// int main() {
//     VersionedHAMT* vhamt = createVersionedHAMT(MAX_LEVEL, BIT_SEG);
//     char command[256];

//     while (fgets(command, sizeof(command), stdin) != NULL) {
//         char operation[20];
//         int key, value, version, oldValue, newValue;
//         sscanf(command, "%s", operation);

//         if (strcmp(operation, "insert") == 0) {
//             sscanf(command, "%*s %d %d", &key, &value);
//             insertHAMT(vhamt, key, value);
//         } else if (strcmp(operation, "search") == 0) {
//             sscanf(command, "%*s %d %d", &key, &version);
//             SearchResult result = searchHAMT(vhamt, key, version);
//             if (result.values != NULL) {
//                 printf("Values for key %d: ", key);
//                 for (int i = 0; i < result.valueCount; ++i) {
//                     printf("%d ", result.values[i]);
//                 }
//                 printf("\n");
//             } else {
//                 printf("Key %d not found.\n", key);
//             }
//         } else if (strcmp(operation, "print") == 0) {
//             sscanf(command, "%*s %d", &version);
//             printHAMT(vhamt, version);
//         } else if (strcmp(operation, "update") == 0) {
//             sscanf(command, "%*s %d %d %d", &key, &oldValue, &newValue);
//             updateHAMT(vhamt, key, oldValue, newValue);
//         } else if (strcmp(operation, "delete") == 0) {
//             sscanf(command, "%*s %d %d", &key, &value);
//             deleteHAMT(vhamt, key, value);
//         } else if (strcmp(operation, "print_versions") == 0) {
//             printVersions(vhamt);
//         }
//     }

//     for (int i = 0; i < vhamt->versionCount; ++i) {
//         freeHAMT(vhamt->versions[i]);
//     }
//     free(vhamt->versions);
//     free(vhamt);
//     return 0;
// }


