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
int BIT_SEG = 4; 

VersionedHAMT* createVersionedHAMT() {
    VersionedHAMT* vhamt = (VersionedHAMT*)(calloc(1, sizeof(VersionedHAMT)));
    vhamt->versions = (HAMT**)(calloc(1, sizeof(HAMT*)));
    vhamt->versions[0] = createHAMT();
    vhamt->versionCount = 1;
    vhamt->currentVersion = 0;
    return vhamt;
}

HAMT* createHAMT() {
    HAMT* new_hamt = (HAMT*)(calloc(1, sizeof(HAMT)));
    if (new_hamt == NULL) {
        return NULL;
    }
    new_hamt->root = createBitIndexNode();
    if (new_hamt->root == NULL) {
        free(new_hamt);
        return NULL;
    }
    return new_hamt;
}

HAMTNode* createBitIndexNode() {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    if (node == NULL) {
        return NULL;
    }
    node->type = BIT_INDEX_NODE;
    node->node.bitIndexNode.bitmap = 0;
    node->node.bitIndexNode.subnodes = calloc(getMaxChild(), sizeof(HAMTNode*));
    if (node->node.bitIndexNode.subnodes == NULL) {
        free(node);
        return NULL;
    }
    return node;
}

HAMTNode* createLeafNode(uint32_t key, int value) {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = LEAF_NODE;
    node->node.leafNode.key = key;
    node->node.leafNode.values = malloc(sizeof(int));
    node->node.leafNode.values[0] = value;
    node->node.leafNode.valueCount = 1;
    return node;
}

uint32_t hashFunction(uint32_t key) {
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = ((key >> 16) ^ key) * 0x45d9f3b;
    key = (key >> 16) ^ key;
    return key;
}


HAMTNode* insertHAMTRec(HAMTNode *node, uint32_t key, int value, int depth) {
    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node == NULL) {
        return createLeafNode(key, value);
    }

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
            HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
            *newLeaf = *node;
            newLeaf->node.leafNode.values = realloc(newLeaf->node.leafNode.values, (newLeaf->node.leafNode.valueCount + 1) * sizeof(int));
            newLeaf->node.leafNode.values[newLeaf->node.leafNode.valueCount++] = value;
            // freeLeafNode(node);  // Free the old leaf node
            return newLeaf;
        } else {
            HAMTNode *newBitIndexNode = createBitIndexNode();
            unsigned int existingHash = hashFunction(node->node.leafNode.key);
            int existingIndex = (existingHash >> (depth * BIT_SEG)) & (getMaxChild() - 1);
            newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
            newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = node;
            newBitIndexNode->node.bitIndexNode.subnodes[index] = insertHAMTRec(NULL, key, value, depth + 1);
            return newBitIndexNode;
        }
    } else if (node->type == BIT_INDEX_NODE) {
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


void insertVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = insertHAMTRec(vhamt->versions[version]->root, key, value, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

void insert(VersionedHAMT *vhamt, uint32_t key, int value) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    if (newHamt == NULL) {
        // Handle allocation failure
        freeHAMT(newHamt);
        return;
    }
    newHamt->root = insertHAMTRec(vhamt->versions[latestVersion]->root, key, value, 0);
    if (newHamt->root == NULL) {
        // Handle allocation failure
        freeHAMT(newHamt);
        return;
    }
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    if (vhamt->versions == NULL) {
        // Handle allocation failure
        freeHAMT(newHamt);
        return;
    }
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}


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

SearchResult search(VersionedHAMT *vhamt, uint32_t key) {
    int latestVersion = vhamt->versionCount - 1;
    if (latestVersion >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", latestVersion);
        SearchResult result = {NULL, 0};
        return result;
    }
    return searchHAMTRec(vhamt->versions[latestVersion]->root, key, 0);
}
SearchResult searchVersion(VersionedHAMT *vhamt, uint32_t key, int version) {
    if (version >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", version);
        SearchResult result = {NULL, 0};
        return result;
    }
    return searchHAMTRec(vhamt->versions[version]->root, key, 0);
}

HAMTNode* updateHAMTRec(HAMTNode *node, uint32_t key, int oldValue, int newValue, int depth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (getMaxChild() - 1);

    if (node->type == LEAF_NODE) {
        if (node->node.leafNode.key == key) {
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




void updateVersion(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = updateHAMTRec(vhamt->versions[version]->root, key, oldValue, newValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

void update(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    if (newHamt == NULL) {
        // Handle allocation failure
        return;
    }
    newHamt->root = updateHAMTRec(vhamt->versions[latestVersion]->root, key, oldValue, newValue, 0);
    if (newHamt->root == NULL) {
        // Handle allocation failure
        freeHAMT(newHamt);
        return;
    }
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    if (vhamt->versions == NULL) {
        // Handle allocation failure
        freeHAMT(newHamt);
        return;
    }
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}


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




    void deleteVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version) {
        HAMT *newHamt = createHAMT();
        newHamt->root = deleteHAMTRec(vhamt->versions[version]->root, key, value, 0);
        vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
        vhamt->versions[vhamt->versionCount] = newHamt;
        vhamt->versionCount++;
        vhamt->currentVersion = vhamt->versionCount - 1;
    }

    void delete(VersionedHAMT *vhamt, uint32_t key, int value) {
        int latestVersion = vhamt->versionCount - 1;
        HAMT *newHamt = createHAMT();
        if (newHamt == NULL) {
            return;
        }
        newHamt->root = deleteHAMTRec(vhamt->versions[latestVersion]->root, key, value, 0);
        if (newHamt->root == NULL) {
            freeHAMT(newHamt);
            return;
        }
        vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
        if (vhamt->versions == NULL) {
            freeHAMT(newHamt);
            return;
        }
        vhamt->versions[vhamt->versionCount] = newHamt;
        vhamt->versionCount++;
        vhamt->currentVersion = vhamt->versionCount - 1;
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

    void printBitmapBinary(int bitmap) {
        for (int i = getMaxChild() - 1; i >= 0; --i) {
            printf("%d", (bitmap >> i) & 1);
        }
    }

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