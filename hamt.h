#ifndef HAMT_H
#define HAMT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define BIT_SEG 4
#define MAX_CHILD (1 << BIT_SEG)
#define MAX_LEVEL (32 / BIT_SEG)

typedef enum { BIT_INDEX_NODE, LEAF_NODE } NodeType;
typedef enum { INT_KEY, STR_KEY } KeyType;

typedef struct BitIndexNode {
    int bitmap;
    struct HAMTNode **subnodes;
} BitIndexNode;

typedef struct KeyValue {
    KeyType type;
    union {
        int intKey;
        char *strKey;
    } key;
    void *value;
} KeyValue;

typedef struct LeafNode {
    KeyValue keyValue;
} LeafNode;

typedef struct SearchResult {
    void *value;
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
} HAMT;

typedef struct VersionedHAMT {
    HAMT **versions;
    int versionCount;
    int currentVersion;
} VersionedHAMT;

typedef struct QueueNode {
    HAMTNode *node;
    int depth;
    struct QueueNode *next;
} QueueNode;

VersionedHAMT* createVersionedHAMT();
HAMT* createHAMT();
HAMTNode* createBitIndexNode();
HAMTNode* createLeafNode(KeyValue keyValue);
unsigned int hashFunction(KeyValue keyValue);
int compareKeys(KeyValue key1, KeyValue key2);
void insertVersion(VersionedHAMT *vhamt, KeyValue keyValue, int version);
void insert(VersionedHAMT *vhamt, KeyValue keyValue);
HAMTNode* insertHAMTRec(HAMTNode *node, KeyValue keyValue, int depth);
SearchResult searchVersion(VersionedHAMT *vhamt, KeyValue key, int version);
SearchResult search(VersionedHAMT *vhamt, KeyValue key);
SearchResult searchHAMTRec(HAMTNode *node, KeyValue key, int depth);
void printHAMT(VersionedHAMT *vhamt, int version);
void printBitmapBinary(int bitmap);
void freeHAMT(HAMT *hamt);
HAMTNode* deleteHAMTRec(HAMTNode *node, KeyValue keyValue, int depth);
void update(VersionedHAMT *vhamt, KeyValue oldKeyValue, KeyValue newKeyValue);
void delete(VersionedHAMT *vhamt, KeyValue keyValue);
HAMTNode* updateHAMTRec(HAMTNode *node, KeyValue oldKeyValue, KeyValue newKeyValue, int depth);
void enqueue(QueueNode **head, HAMTNode *node, int depth);
QueueNode *dequeue(QueueNode **head);
void printVersions(VersionedHAMT *vhamt);
void freeHAMTNode(HAMTNode *node);
void measurePerformance(int N, int D, int U);
void printKeyValue(KeyValue keyValue);

#endif // HAMT_H