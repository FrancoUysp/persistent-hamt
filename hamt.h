#ifndef HAMT_H
#define HAMT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

#define BIT_SEG 4
#define MAX_CHILD (1 << BIT_SEG)
#define MAX_LEVEL (32 / BIT_SEG)

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
HAMTNode* createLeafNode(uint32_t key, int value);
uint32_t hashFunction(uint32_t key);
void insertVersion(VersionedHAMT *vhamt, uint32_t key, int value, int version);
void insert(VersionedHAMT *vhamt, uint32_t key, int value);
HAMTNode* insertHAMTRec(HAMTNode *node, uint32_t key, int value, int depth);
SearchResult searchVersion(VersionedHAMT *vhamt, uint32_t key, int version);
SearchResult search(VersionedHAMT *vhamt, uint32_t key);
SearchResult searchHAMTRec(HAMTNode *node, uint32_t key, int depth);
void printHAMT(VersionedHAMT *vhamt, int version);
void printBitmapBinary(int bitmap);
void freeHAMT(HAMT *hamt);
HAMTNode* deleteHAMTRec(HAMTNode *node, uint32_t key, int value, int depth);
void update(VersionedHAMT *vhamt, uint32_t key, int oldValue, int newValue);
void delete(VersionedHAMT *vhamt, uint32_t key, int value);
HAMTNode* updateHAMTRec(HAMTNode *node, uint32_t key, int oldValue, int newValue, int depth);
void enqueue(QueueNode **head, HAMTNode *node, int depth);
QueueNode *dequeue(QueueNode **head);
void printVersions(VersionedHAMT *vhamt);
void freeHAMTNode(HAMTNode *node);
void freeVersionedHAMT(VersionedHAMT* vhamt);
void measurePerformance(int N, int D, int U);

#endif // HAMT_H