//Refrences
//https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key


// FIXME: shrink the tree for the deletion

// TODO: memory leakage!!!
// TODO: comment code

// TODO: think of experiments!!!
// TODO: build a small experiments file maybe a python file uses it as so file
// TODO: build a frontend linked list for this hamt and a standart one and then have a main.c run a comparison between the different operations between the two. 
// TODO: list resources in comments
// TODO: make code cleaner
// TODO: radme.md

// TODO: write report

#include "hamt.h"
// Global variable for bit segment size
int BIT_SEG = 4; 

/**
* @brief Creates a new versioned HAMT.
*
* @return Pointer to the newly created VersionedHAMT.
*/
VersionedHAMT* createVersionedHAMT() {
    VersionedHAMT* vhamt = (VersionedHAMT*)(calloc(1, sizeof(VersionedHAMT)));
    vhamt->versions = (HAMT**)(calloc(1, sizeof(HAMT*)));
    vhamt->versions[0] = createHAMT();
    vhamt->versionCount = 1;
    vhamt->currentVersion = 0;
    return vhamt;
}

/**
* @brief Creates a new HAMT.
*
* @return Pointer to the newly created HAMT.
*/
HAMT* createHAMT() {
    HAMT* new_hamt = (HAMT*)(calloc(1, sizeof(HAMT)));
    new_hamt->root = createBitIndexNode();
    return new_hamt;
}

/**
* @brief Creates a new bit-index node.
*
* @return Pointer to the newly created HAMTNode.
*/
HAMTNode* createBitIndexNode() {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = BIT_INDEX_NODE;
    node->node.bitIndexNode.bitmap = 0;
    node->node.bitIndexNode.subnodes = calloc(getMaxChild(), sizeof(HAMTNode*));
    return node;
}

/**
* @brief Creates a new leaf node.
*
* @param key The key for the leaf node.
* @param value The value for the leaf node.
* @return Pointer to the newly created HAMTNode.
*/
HAMTNode* createLeafNode(uint32_t key, int value) {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = LEAF_NODE;
    node->node.leafNode.key = key;
    node->node.leafNode.values = malloc(sizeof(int));
    node->node.leafNode.values[0] = value;
    node->node.leafNode.valueCount = 1;
    return node;
}

/**
* @brief Hash function to generate hash for a key.
*
* @param key The key to hash.
* @return The hashed key.
*/
u_int32_t hashFunction(uint32_t key) {
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key;
}

/**
* @brief Recursive function to insert a key-value pair into the HAMT.
*
* @param node The current HAMT node.
* @param key The key to insert.
* @param value The value to insert.
* @param depth The current depth of the tree.
* @return Pointer to the updated HAMTNode.
*/
HAMTNode* insertHAMTRec(HAMTNode *node, uint32_t key, int value, int depth) {
    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node == NULL) {
        return createLeafNode(key, value);
    }

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            // Create a new leaf node with updated values
            HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
            *newLeaf = *node;
            newLeaf->node.leafNode.values = realloc(newLeaf->node.leafNode.values, (newLeaf->node.leafNode.valueCount + 1) * sizeof(int));
            newLeaf->node.leafNode.values[newLeaf->node.leafNode.valueCount++] = value;
            return newLeaf;
        } else {
            // Create a new bit-index node and insert the existing and new leaf nodes
            HAMTNode *newBitIndexNode = createBitIndexNode();
            unsigned int existingHash = hashFunction(node->node.leafNode.key);
            int existingIndex = (existingHash >> (depth * BIT_SEG)) & (getMaxChild() - 1);
            newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
            newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = node;
            newBitIndexNode->node.bitIndexNode.subnodes[index] = insertHAMTRec(NULL, key, value, depth + 1);
            return newBitIndexNode;
        }
    } else if (node->type == BIT_INDEX_NODE) {
        // Create a new bit-index node with updated subnodes
        HAMTNode *newNode = malloc(sizeof(HAMTNode));
        *newNode = *node;
        newNode->node.bitIndexNode.subnodes = calloc(getMaxChild(), sizeof(HAMTNode*));

        // Copy existing subnodes references
        for (int i = 0; i < getMaxChild(); i++) {
            if (i != index) {
                newNode->node.bitIndexNode.subnodes[i] = node->node.bitIndexNode.subnodes[i];
            }
        }

        // Recursively insert into the correct subnode
        newNode->node.bitIndexNode.subnodes[index] = insertHAMTRec(node->node.bitIndexNode.subnodes[index], key, value, depth + 1);
        newNode->node.bitIndexNode.bitmap |= (1 << index);

        return newNode;
    }

    return NULL; // Should never reach here
}

/**
* @brief Inserts a key-value pair into a specific version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to insert.
* @param value The value to insert.
* @param version The version number to insert into.
*/
void insertVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = insertHAMTRec(vhamt->versions[version]->root, key, value, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Inserts a key-value pair into the latest version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to insert.
* @param value The value to insert.
*/
void insert(VersionedHAMT *vhamt, uint32_t key, int value) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = insertHAMTRec(vhamt->versions[latestVersion]->root, key, value, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Recursive function to search for a key in the HAMT.
*
* @param node The current HAMT node.
* @param key The key to search for.
* @param depth The current depth of the tree.
* @return SearchResult containing the values and their count.
*/
SearchResult searchHAMTRec(HAMTNode *node, uint32_t key, int depth) {
    SearchResult result = {NULL, 0};
    if (node == NULL) {
        return result;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            result.values = node->node.leafNode.values;
            result.valueCount = node->node.leafNode.valueCount;
            return result;
        }
    } else if (node->type == BIT_INDEX_NODE) {
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            return searchHAMTRec(subnode, key, depth + 1);
        }
    }

    return result;
}

/**
* @brief Searches for a key in the latest version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to search for.
* @return SearchResult containing the values and their count.
*/
SearchResult search(VersionedHAMT *vhamt, uint32_t key) {
    int latestVersion = vhamt->versionCount - 1;
    if (latestVersion >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", latestVersion);
        SearchResult result = {NULL, 0};
        return result;
    }
    return searchHAMTRec(vhamt->versions[latestVersion]->root, key, 0);
}

/**
* @brief Searches for a key in a specific version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to search for.
* @param version The version number to search in.
* @return SearchResult containing the values and their count.
*/
SearchResult searchVersion(VersionedHAMT *vhamt, uint32_t key, int version) {
    if (version >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", version);
        SearchResult result = {NULL, 0};
        return result;
    }
    return searchHAMTRec(vhamt->versions[version]->root, key, 0);
}

/**
* @brief Recursive function to update a value in the HAMT.
*
* @param node The current HAMT node.
* @param key The key to update.
* @param oldValue The old value to replace.
* @param newValue The new value to set.
* @param depth The current depth of the tree.
* @return Pointer to the updated HAMTNode.
*/
HAMTNode* updateHAMTRec(HAMTNode *node, uint32_t key, int oldValue, int newValue, int depth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            // Create a new leaf node with updated values
            HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
            *newLeaf = *node;
            newLeaf->node.leafNode.values = malloc(newLeaf->node.leafNode.valueCount * sizeof(int));
            for (int i = 0; i < newLeaf->node.leafNode.valueCount; ++i) {
                newLeaf->node.leafNode.values[i] = node->node.leafNode.values[i];
            }
            for (int i = 0; i < newLeaf->node.leafNode.valueCount; ++i) {
                if (newLeaf->node.leafNode.values[i] == oldValue) {
                    newLeaf->node.leafNode.values[i] = newValue;
                    return newLeaf;
                }
            }
            free(newLeaf->node.leafNode.values);
            free(newLeaf);
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        HAMTNode *newNode = malloc(sizeof(HAMTNode));
        *newNode = *node;
        newNode->node.bitIndexNode.subnodes = calloc(getMaxChild(), sizeof(HAMTNode*));

        // Copy existing subnodes references
        for (int i = 0; i < getMaxChild(); i++) {
            if (i != index) {
                newNode->node.bitIndexNode.subnodes[i] = node->node.bitIndexNode.subnodes[i];
            }
        }

        // Recursively update the correct subnode
        newNode->node.bitIndexNode.subnodes[index] = updateHAMTRec(node->node.bitIndexNode.subnodes[index], key, oldValue, newValue, depth + 1);
        newNode->node.bitIndexNode.bitmap |= (1 << index);

        return newNode;
    }

    return NULL; // Should never reach here
}

/**
* @brief Updates a value for a specific key in a specific version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to update.
* @param oldValue The old value to replace.
* @param newValue The new value to set.
* @param version The version number to update.
*/
void updateVersion(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = updateHAMTRec(vhamt->versions[version]->root, key, oldValue, newValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Updates a value for a specific key in the latest version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to update.
* @param oldValue The old value to replace.
* @param newValue The new value to set.
*/
void update(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = updateHAMTRec(vhamt->versions[latestVersion]->root, key, oldValue, newValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Recursive function to delete a key-value pair from the HAMT.
*
* @param node The current HAMT node.
* @param key The key to delete.
* @param value The value to delete.
* @param depth The current depth of the tree.
* @return Pointer to the updated HAMTNode.
*/
HAMTNode* deleteHAMTRec(HAMTNode *node, uint32_t key, int value, int depth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            for (int i = 0; i < node->node.leafNode.valueCount; ++i) {
                if (node->node.leafNode.values[i] == value) {
                    HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
                    *newLeaf = *node;
                    newLeaf->node.leafNode.values = malloc((newLeaf->node.leafNode.valueCount - 1) * sizeof(int));
                    for (int j = 0, k = 0; j < newLeaf->node.leafNode.valueCount; ++j) {
                        if (j != i) {
                            newLeaf->node.leafNode.values[k++] = newLeaf->node.leafNode.values[j];
                        }
                    }
                    newLeaf->node.leafNode.valueCount--;
                    if (newLeaf->node.leafNode.valueCount == 0) {
                        free(newLeaf->node.leafNode.values);
                        free(newLeaf);
                        return NULL;
                    }
                    return newLeaf;
                }
            }
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        HAMTNode *newNode = malloc(sizeof(HAMTNode));
        *newNode = *node;
        newNode->node.bitIndexNode.subnodes = calloc(getMaxChild(), sizeof(HAMTNode*));

        for (int i = 0; i < getMaxChild(); i++) {
            if (i != index) {
                newNode->node.bitIndexNode.subnodes[i] = node->node.bitIndexNode.subnodes[i];
            }
        }

        newNode->node.bitIndexNode.subnodes[index] = deleteHAMTRec(node->node.bitIndexNode.subnodes[index], key, value, depth + 1);
        if (newNode->node.bitIndexNode.subnodes[index] == NULL) {
            newNode->node.bitIndexNode.bitmap &= ~(1 << index);
        }

        if (newNode->node.bitIndexNode.bitmap == 0) {
            free(newNode->node.bitIndexNode.subnodes);
            free(newNode);
            return NULL;
        }

        return newNode;
    }

    return NULL;
}

/**
* @brief Deletes a key-value pair from a specific version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to delete.
* @param value The value to delete.
* @param version The version number to delete from.
*/
void deleteVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = deleteHAMTRec(vhamt->versions[version]->root, key, value, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Deletes a key-value pair from the latest version of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param key The key to delete.
* @param value The value to delete.
*/
void delete(VersionedHAMT *vhamt, uint32_t key, int value) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = deleteHAMTRec(vhamt->versions[latestVersion]->root, key, value, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

/**
* @brief Adds a node to the queue.
*
* @param head Pointer to the head of the queue.
* @param node The HAMT node to enqueue.
* @param depth The depth of the node in the tree.
*/
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

/**
* @brief Removes and returns the front node from the queue.
*
* @param head Pointer to the head of the queue.
* @return Pointer to the dequeued node.
*/
QueueNode *dequeue(QueueNode **head) {
    if (*head == NULL) return NULL;
    QueueNode *dequeuedNode = *head;
    *head = (*head)->next;
    return dequeuedNode;
}

/**
* @brief Prints the HAMT for a specific version.
*
* @param vhamt Pointer to the VersionedHAMT.
* @param version The version number to print.
*/
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
            for (int i = 0; i < getMaxChild(); ++i) {
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
                if (i < getMaxChild() - 1) {
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

/**
* @brief Prints the bitmap in binary form.
*
* @param bitmap The bitmap to print.
*/
void printBitmapBinary(int bitmap) {
    for (int i = getMaxChild() - 1; i >= 0; --i) {
        printf("%d", (bitmap >> i) & 1);
    }
}

/**
* @brief Frees the memory allocated for a HAMT node.
*
* @param node Pointer to the HAMT node to free.
*/
void freeHAMTNode(HAMTNode *node) {
    if (node == NULL) return;

    if (node->type == BIT_INDEX_NODE) {
        for (int i = 0; i < getMaxChild(); ++i) {
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

/**
* @brief Frees the memory allocated for a HAMT.
*
* @param hamt Pointer to the HAMT to free.
*/
void freeHAMT(HAMT *hamt) {
    if (hamt == NULL) return;
    freeHAMTNode(hamt->root);
    free(hamt);
}

/**
* @brief Prints all versions of the HAMT.
*
* @param vhamt Pointer to the VersionedHAMT.
*/
void printVersions(VersionedHAMT *vhamt) {
    printf("Versions:\n");
    for (int i = 0; i < vhamt->versionCount; ++i) {
        printf("Version %d\n", i);
    }
    printf("Current Version: %d\n", vhamt->currentVersion);
}

/**
* @brief Returns the maximum number of children a node can have.
*
* @return The maximum number of children.
*/
int getMaxChild() {
    return 1 << BIT_SEG;
}

int main() {

    VersionedHAMT* vhamt = createVersionedHAMT();
    
    // Insert values into the HAMT
    insert(vhamt, 223, 223);
    insert(vhamt, 239, 239);

    // Print initial versions
    printf("Initial state of latest version:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n\n");

    // Update a value for a specific key
    update(vhamt, 239, 239, 0);

    // Print updated versions
    printf("State of latest version after update:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n\n");

    // Search for the updated key in the latest version
    SearchResult res = searchVersion(vhamt, 239, vhamt->versionCount - 1);
    if (res.values != NULL) {
        printf("Updated values for key 4 in latest version: ");
        for (int i = 0; i < res.valueCount; ++i) {
            printf("%d ", res.values[i]);
        }
        printf("\n");
    } else {
        printf("Key 4 not found in latest version.\n");
    }

    // Search in the previous version to ensure it remains unchanged
    res = searchVersion(vhamt, 239, vhamt->versionCount - 2);
    if (res.values != NULL) {
        printf("Values for key 4 in previous version: ");
        for (int i = 0; i < res.valueCount; ++i) {
            printf("%d ", res.values[i]);
        }
        printf("\n");
    } else {
        printf("Key 4 not found in previous version.\n");
    }

    // Delete the updated value
    delete(vhamt, 239, 0);

    // Print versions after deletion
    printf("State of latest version after deletion:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n\n");

    // Search for the deleted key in the latest version
    res = searchVersion(vhamt, 239, vhamt->versionCount - 1);
    if (res.values != NULL) {
        printf("Values for key 4 in latest version after deletion: ");
        for (int i = 0; i < res.valueCount; ++i) {
            printf("%d ", res.values[i]);
        }
        printf("\n");
    } else {
        printf("Key 4 not found in latest version after deletion.\n");
    }

    // Search in the previous version to ensure it remains unchanged
    res = searchVersion(vhamt, 4, vhamt->versionCount - 2);
    if (res.values != NULL) {
        printf("Values for key 4 in previous version after deletion: ");
        for (int i = 0; i < res.valueCount; ++i) {
            printf("%d ", res.values[i]);
        }
        printf("\n");
    } else {
        printf("Key 4 not found in previous version after deletion.\n");
    }

    return 0;
}



