#ifndef HAMT_H
#define HAMT_H

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

VersionedHAMT* createVersionedHAMT(int maxDepth, int blockSize);
HAMT* createHAMT(int maxDepth, int blockSize);
HAMTNode* createBitIndexNode();
HAMTNode* createLeafNode(int key, int value);
unsigned int hashFunction(int key);
HAMTNode* copyHAMTNode(HAMTNode *node);
void insertHAMT(VersionedHAMT *vhamt, int key, int value);
void insertHAMTRec(HAMTNode **node, int key, int value, int depth, int blockSize, int maxDepth);
SearchResult searchHAMT(VersionedHAMT *vhamt, int key, int version);
SearchResult searchHAMTRec(HAMTNode *node, int key, int depth, int blockSize, int maxDepth);
void printHAMT(VersionedHAMT *vhamt, int version);
void printBitmapBinary(int bitmap);
void freeHAMT(HAMT *hamt);
void deleteHAMT(VersionedHAMT *vhamt, int key, int value);
HAMTNode* deleteHAMTRec(HAMTNode *node, int key, int value, int depth, int blockSize, int maxDepth);
void updateHAMT(VersionedHAMT *vhamt, int key, int oldValue, int newValue);
void updateHAMTRec(HAMTNode *node, int key, int oldValue, int newValue, int depth, int blockSize, int maxDepth);
void enqueue(QueueNode **head, HAMTNode *node, int depth);
QueueNode *dequeue(QueueNode **head);
void printVersions(VersionedHAMT *vhamt);
void freeHAMTNode(HAMTNode *node);

void intToStr(int num, char *str);

#endif // HAMT_H
