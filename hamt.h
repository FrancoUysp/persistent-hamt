#ifndef HAMT_H
#define HAMT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

extern int BIT_SEG;

typedef enum { BIT_INDEX_NODE, LEAF_NODE } NodeType;

typedef struct BitIndexNode {
    int bitmap;
    struct HAMTNode **subnodes;
} BitIndexNode;

typedef struct LeafNode {
    uint32_t key;
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
void updateVersion(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue, int version);
void delete(VersionedHAMT *vhamt, uint32_t key, int value);
HAMTNode* deleteHAMTRec(HAMTNode *node, uint32_t key, int value, int depth);
HAMTNode* updateHAMTRec(HAMTNode *node, uint32_t key, int oldValue, int newValue, int depth);
HAMTNode* createLeafNode(uint32_t key, int value);
void insert(VersionedHAMT *vhamt, uint32_t key, int value);
SearchResult search(VersionedHAMT *vhamt, uint32_t key);
QueueNode* dequeue(QueueNode **head);
void printHAMT(VersionedHAMT *vhamt, int version);
void insertVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version);
void update(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue);
void printBitmapBinary(int bitmap);
HAMTNode* insertHAMTRec(HAMTNode *node, uint32_t key, int value, int depth);
SearchResult searchHAMTRec(HAMTNode *node, uint32_t key, int depth);
void deleteVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version);
uint32_t hashFunction(uint32_t key);
void enqueue(QueueNode **head, HAMTNode *node, int depth);
void freeHAMT(HAMT *hamt);
void freeHAMTNode(HAMTNode *node);
SearchResult searchVersion(VersionedHAMT *vhamt, uint32_t key, int version);
void printVersions(VersionedHAMT *vhamt);
int getMaxChild();




#endif // HAMT_H