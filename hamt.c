//Refrences
//https://stackoverflow.com/questions/664014/what-integer-hash-function-are-good-that-accepts-an-integer-hash-key


// TODO: allow for different types of values and keys ( think about it if we use a linked list to store strings with this thing as the backend then we would like to hash the string value with itself as the value and the key) e.g. linkedlist.add("some string") -> hamt.insert("some string".hash, "some string")
// TODO: force the hash to return a u_int_32 (change the return type) but this will also change some other parts of the code (this should be easier)
// TODO: build a frontend linked list for this hamt and a standart one and then have a main.c run a comparison between the different operations between the two. 

// TODO: think of experiments!!!
#include "hamt.h"

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
    new_hamt->root = createBitIndexNode();
    return new_hamt;
}

HAMTNode* createBitIndexNode() {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = BIT_INDEX_NODE;
    node->node.bitIndexNode.bitmap = 0;
    node->node.bitIndexNode.subnodes = calloc(MAX_CHILD, sizeof(HAMTNode*));
    return node;
}

HAMTNode* createLeafNode(KeyValue keyValue) {
    HAMTNode* node = malloc(sizeof(HAMTNode));
    node->type = LEAF_NODE;
    node->node.leafNode.keyValue = keyValue;
    return node;
}

unsigned int hashFunction(KeyValue keyValue) {
    if (keyValue.type == INT_KEY) {
        int key = keyValue.key.intKey;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = ((key >> 16) ^ key) * 0x45d9f3b;
        key = (key >> 16) ^ key;
        return key;
    } else if (keyValue.type == STR_KEY) {
        const char *key = keyValue.key.strKey;
        unsigned int hash = 5381;
        int c;
        while ((c = *key++)) {
            hash = ((hash << 5) + hash) + c;
        }
        return hash;
    }
    return 0;
}

int compareKeys(KeyValue key1, KeyValue key2) {
    if (key1.type != key2.type) {
        return 0;
    }
    if (key1.type == INT_KEY) {
        return key1.key.intKey == key2.key.intKey;
    } else if (key1.type == STR_KEY) {
        return strcmp(key1.key.strKey, key2.key.strKey) == 0;
    }
    return 0;
}

HAMTNode* insertHAMTRec(HAMTNode *node, KeyValue keyValue, int depth) {
    unsigned int hash = hashFunction(keyValue);
    int index = (hash >> (depth * BIT_SEG)) & (MAX_CHILD - 1);

    if (node == NULL) {
        return createLeafNode(keyValue);
    }

    if (node->type == LEAF_NODE) {
        if (compareKeys(node->node.leafNode.keyValue, keyValue)) {
            // Update the value if the key already exists
            HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
            *newLeaf = *node;
            newLeaf->node.leafNode.keyValue.value = keyValue.value;
            return newLeaf;
        } else {
            HAMTNode *newBitIndexNode = createBitIndexNode();
            unsigned int existingHash = hashFunction(node->node.leafNode.keyValue);
            int existingIndex = (existingHash >> (depth * BIT_SEG)) & (MAX_CHILD - 1);
            newBitIndexNode->node.bitIndexNode.bitmap |= (1 << existingIndex);
            newBitIndexNode->node.bitIndexNode.subnodes[existingIndex] = node;
            newBitIndexNode->node.bitIndexNode.subnodes[index] = insertHAMTRec(NULL, keyValue, depth + 1);
            return newBitIndexNode;
        }
    } else if (node->type == BIT_INDEX_NODE) {
        HAMTNode *newNode = malloc(sizeof(HAMTNode));
        *newNode = *node;
        newNode->node.bitIndexNode.subnodes = calloc(MAX_CHILD, sizeof(HAMTNode*));

        // Copy existing subnodes references
        for (int i = 0; i < MAX_CHILD; i++) {
            if (i != index) {
                newNode->node.bitIndexNode.subnodes[i] = node->node.bitIndexNode.subnodes[i];
            }
        }

        // Recursively insert into the correct subnode
        newNode->node.bitIndexNode.subnodes[index] = insertHAMTRec(node->node.bitIndexNode.subnodes[index], keyValue, depth + 1);
        newNode->node.bitIndexNode.bitmap |= (1 << index);

        return newNode;
    }

    return NULL; // Should never reach here
}

void insertVersion(VersionedHAMT *vhamt, KeyValue keyValue, int version) {
    HAMT *newHamt = createHAMT();
    newHamt->root = insertHAMTRec(vhamt->versions[version-1]->root, keyValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

void insert(VersionedHAMT *vhamt, KeyValue keyValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = insertHAMTRec(vhamt->versions[latestVersion]->root, keyValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

SearchResult searchHAMTRec(HAMTNode *node, KeyValue key, int depth) {
    // printf("Searching at depth %d\n", depth);
    SearchResult result = {NULL};
    if (node == NULL) {
        // printf("Node is NULL at depth %d\n", depth);
        return result;
    }

    unsigned int hash = hashFunction(key);
    int index = (hash >> (depth * BIT_SEG)) & (MAX_CHILD - 1);
    // printf("Hash: %u, Index: %d\n", hash, index);

    if (node->type == LEAF_NODE) {
        // printf("Reached leaf node at depth %d\n", depth);
        if (compareKeys(node->node.leafNode.keyValue, key)) {
            // printf("Keys match at depth %d\n", depth);
            result.value = node->node.leafNode.keyValue.value;
            return result;
        }
    } else if (node->type == BIT_INDEX_NODE) {
        // printf("Reached bit index node at depth %d\n", depth);
        int bit = 1 << index;
        if (node->node.bitIndexNode.bitmap & bit) {
            // printf("Bit is set at index %d\n", index);
            HAMTNode *subnode = node->node.bitIndexNode.subnodes[index];
            return searchHAMTRec(subnode, key, depth + 1);
        } else {
            // printf("Bit is not set at index %d\n", index);
        }
    }

    // printf("Key not found at depth %d\n", depth);
    return result;
}
SearchResult search(VersionedHAMT *vhamt, KeyValue key) {
    int latestVersion = vhamt->versionCount - 1;
    if (latestVersion >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", latestVersion);
        SearchResult result = {NULL};
        return result;
    }
    return searchHAMTRec(vhamt->versions[latestVersion]->root, key, 0);
}

SearchResult searchVersion(VersionedHAMT *vhamt, KeyValue key, int version) {
    printf("Searching for key in version %d\n", version);
    printf("Key type: %d\n", key.type);
    if (key.type == INT_KEY) {
        printf("Integer key: %d\n", key.key.intKey);
    } else if (key.type == STR_KEY) {
        printf("String key: %s\n", key.key.strKey);
    }
    
    if (version >= vhamt->versionCount) {
        printf("Version %d does not exist.\n", version);
        SearchResult result = {NULL};
        return result;
    }

    printf("Accessing HAMT version at index %d\n", version);
    HAMT *hamt = vhamt->versions[version];
    if (hamt == NULL) {
        printf("HAMT at version %d is NULL\n", version);
    } else {
        printf("HAMT at version %d is not NULL\n", version);
    }

    printf("Calling searchHAMTRec with root node of version %d\n", version);
    SearchResult result = searchHAMTRec(hamt->root, key, 0);
    
    if (result.value == NULL) {
        printf("Key not found in version %d\n", version);
    } else {
        printf("Key found in version %d\n", version);
    }
    
    return result;
}

HAMTNode* updateHAMTRec(HAMTNode *node, KeyValue oldKeyValue, KeyValue newKeyValue, int depth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(oldKeyValue);
    int index = (hash >> (depth * BIT_SEG)) & (MAX_CHILD - 1);

    if (node->type == LEAF_NODE) {
        if (compareKeys(node->node.leafNode.keyValue, oldKeyValue)) {
            HAMTNode *newLeaf = malloc(sizeof(HAMTNode));
            *newLeaf = *node;
            newLeaf->node.leafNode.keyValue = newKeyValue;
            return newLeaf;
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        HAMTNode *newNode = malloc(sizeof(HAMTNode));
        *newNode = *node;
        newNode->node.bitIndexNode.subnodes = calloc(MAX_CHILD, sizeof(HAMTNode*));

        // Copy existing subnodes references
        for (int i = 0; i < MAX_CHILD; i++) {
            if (i != index) {
                newNode->node.bitIndexNode.subnodes[i] = node->node.bitIndexNode.subnodes[i];
            }
        }

        // Recursively update the correct subnode
        newNode->node.bitIndexNode.subnodes[index] = updateHAMTRec(node->node.bitIndexNode.subnodes[index], oldKeyValue, newKeyValue, depth + 1);
        newNode->node.bitIndexNode.bitmap |= (1 << index);

        return newNode;
    }

    return NULL; // Should never reach here
}

void update(VersionedHAMT *vhamt, KeyValue oldKeyValue, KeyValue newKeyValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = updateHAMTRec(vhamt->versions[latestVersion]->root, oldKeyValue, newKeyValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
    vhamt->versions[vhamt->versionCount] = newHamt;
    vhamt->versionCount++;
    vhamt->currentVersion = vhamt->versionCount - 1;
}

HAMTNode* deleteHAMTRec(HAMTNode *node, KeyValue keyValue, int depth) {
    if (node == NULL) {
        return NULL;
    }

    unsigned int hash = hashFunction(keyValue);
    int index = (hash >> (depth * BIT_SEG)) & (MAX_CHILD - 1);

    if (node->type == LEAF_NODE) {
        if (compareKeys(node->node.leafNode.keyValue, keyValue)) {
            free(node);
            return NULL;
        }
        return node;
    }

    if (node->type == BIT_INDEX_NODE) {
        node->node.bitIndexNode.subnodes[index] = deleteHAMTRec(node->node.bitIndexNode.subnodes[index], keyValue, depth + 1);

        if (node->node.bitIndexNode.subnodes[index] == NULL) {
            node->node.bitIndexNode.bitmap &= ~(1 << index);
        }

        if (node->node.bitIndexNode.bitmap == 0) {
            free(node->node.bitIndexNode.subnodes);
            free(node);
            return NULL;
        }

        return node;
    }

    return NULL;
}

void delete(VersionedHAMT *vhamt, KeyValue keyValue) {
    int latestVersion = vhamt->versionCount - 1;
    HAMT *newHamt = createHAMT();
    newHamt->root = deleteHAMTRec(vhamt->versions[latestVersion]->root, keyValue, 0);
    vhamt->versions = realloc(vhamt->versions, (vhamt->versionCount + 1) * sizeof(HAMT*));
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
            printf("  LeafNode: ");
            printKeyValue(node->node->node.leafNode.keyValue);
            printf("\n");
        }

        free(node);
    }
}

void printBitmapBinary(int bitmap) {
    for (int i = MAX_CHILD - 1; i >= 0; --i) {
        printf("%d", (bitmap >> i) & 1);
    }
}

void printKeyValue(KeyValue keyValue) {
    if (keyValue.type == INT_KEY) {
        printf("key = %d, value = %d", keyValue.key.intKey, *(int*)keyValue.value);
    } else if (keyValue.type == STR_KEY) {
        printf("key = %s, value = %s", keyValue.key.strKey, (char*)keyValue.value);
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
        if (node->node.leafNode.keyValue.type == STR_KEY) {
            free(node->node.leafNode.keyValue.key.strKey);
        }
        free(node->node.leafNode.keyValue.value);
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

void measurePerformance(int N, int D, int U) {
    VersionedHAMT* vhamt = createVersionedHAMT();
    clock_t start, end;
    double insertionTime = 0.0, deletionTime = 0.0, updateTime = 0.0;

    // Measure insertion time
    for (int i = 0; i < N; i++) {
        KeyValue keyValue;
        keyValue.type = INT_KEY;
        keyValue.key.intKey = rand();
        int value = rand();
        keyValue.value = &value;
        start = clock();
        insert(vhamt, keyValue);
        end = clock();
        insertionTime += (double)(end - start) / CLOCKS_PER_SEC;
    }
    insertionTime /= N;

    // Measure deletion time
    for (int i = 0; i < D; i++) {
        KeyValue keyValue;
        keyValue.type = INT_KEY;
        keyValue.key.intKey = rand() % N; // Assuming the key was inserted
        int value = rand();
        keyValue.value = &value;
        start = clock();
        delete(vhamt, keyValue);
        end = clock();
        deletionTime += (double)(end - start) / CLOCKS_PER_SEC;
    }
    deletionTime /= D;

    // Measure update time
    for (int i = 0; i < U; i++) {
        KeyValue oldKeyValue, newKeyValue;
        oldKeyValue.type = INT_KEY;
        oldKeyValue.key.intKey = rand() % N; // Assuming the key was inserted
        int oldValue = rand();
        oldKeyValue.value = &oldValue;
        newKeyValue.type = INT_KEY;
        newKeyValue.key.intKey = oldKeyValue.key.intKey;
        int newValue = rand();
        newKeyValue.value = &newValue;
        start = clock();
        update(vhamt, oldKeyValue, newKeyValue);
        end = clock();
        updateTime += (double)(end - start) / CLOCKS_PER_SEC;
    }
    updateTime /= U;

    // Print average times
    printf("Average insertion time: %f seconds\n", insertionTime);
    printf("Average deletion time: %f seconds\n", deletionTime);
    printf("Average update time: %f seconds\n", updateTime);
}

int main() {
    VersionedHAMT* vhamt = createVersionedHAMT();

    // Insert string key-value pairs into the HAMT
    KeyValue keyValue1;
    keyValue1.type = STR_KEY;
    keyValue1.key.strKey = strdup("apple");
    char *value1 = strdup("red");
    keyValue1.value = value1;
    insert(vhamt, keyValue1);

    KeyValue keyValue2;
    keyValue2.type = STR_KEY;
    keyValue2.key.strKey = strdup("banana");
    char *value2 = strdup("yellow");
    keyValue2.value = value2;
    insert(vhamt, keyValue2);

    // Print initial version
    printf("Initial state of the HAMT:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n");

    // Search for a key in the initial version
    SearchResult result = search(vhamt, keyValue1);
    if (result.value != NULL) {
        printf("Value for key 'apple': %s\n", (char*)result.value);
    } else {
        printf("Key 'apple' not found.\n");
    }
    printf("\n");

    // Update the value for a key
    KeyValue updateKeyValue;
    updateKeyValue.type = STR_KEY;
    updateKeyValue.key.strKey = strdup("apple");
    char *newValue = strdup("green");
    updateKeyValue.value = newValue;
    update(vhamt, keyValue1, updateKeyValue);

    // Print the updated version
    printf("State of the HAMT after update:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n");

    // Search for the updated key in the latest version
    result = search(vhamt, updateKeyValue);
    if (result.value != NULL) {
        printf("Updated value for key 'apple': %s\n", (char*)result.value);
    } else {
        printf("Key 'apple' not found after update.\n");
    }
    printf("\n");

    // Delete a key-value pair
    delete(vhamt, keyValue2);

    // Print the version after deletion
    printf("State of the HAMT after deletion:\n");
    printHAMT(vhamt, vhamt->versionCount - 1);
    printf("\n");

    // Search for the deleted key in the latest version
    result = search(vhamt, keyValue2);
    if (result.value != NULL) {
        printf("Value for key 'banana' after deletion: %s\n", (char*)result.value);
    } else {
        printf("Key 'banana' not found after deletion.\n");
    }
    printf("\n");

    // Free allocated memory
    freeHAMT(vhamt->versions[0]);
    free(vhamt->versions);
    free(vhamt);
    free(keyValue1.key.strKey);
    free(value1);
    free(keyValue2.key.strKey);
    free(value2);
    free(updateKeyValue.key.strKey);
    free(newValue);

    return 0;
}