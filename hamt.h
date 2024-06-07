#ifndef HAMT_H
#define HAMT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdint.h>

extern int BIT_SEG; /**< Number of bits per segment in the bitmap */

/**
 * @brief Enum representing the type of a HAMT node.
 */
typedef enum { BIT_INDEX_NODE, LEAF_NODE } NodeType;

/**
 * @brief Structure representing a bit index node in the HAMT.
 */
typedef struct BitIndexNode {
    int bitmap; /**< Bitmap representing the existence of subnodes */
    struct HAMTNode **subnodes; /**< Array of pointers to subnodes */
} BitIndexNode;

/**
 * @brief Structure representing a leaf node in the HAMT.
 */
typedef struct LeafNode {
    uint32_t key; /**< Key stored in the leaf node */
    int *values; /**< Array of values associated with the key */
    int valueCount; /**< Number of values associated with the key */
} LeafNode;

/**
 * @brief Structure representing the result of a search operation in the HAMT.
 */
typedef struct SearchResult {
    int *values; /**< Array of values found */
    int valueCount; /**< Number of values found */
} SearchResult;

/**
 * @brief Structure representing a node in the HAMT.
 */
typedef struct HAMTNode {
    NodeType type; /**< Type of the HAMT node */
    union {
        BitIndexNode bitIndexNode; /**< Bit index node */
        LeafNode leafNode; /**< Leaf node */
    } node;
} HAMTNode;

/**
 * @brief Structure representing a Hash Array Mapped Trie (HAMT).
 */
typedef struct HAMT {
    HAMTNode *root; /**< Root node of the HAMT */
} HAMT;

/**
 * @brief Structure representing a versioned HAMT.
 */
typedef struct VersionedHAMT {
    HAMT **versions; /**< Array of HAMT versions */
    int versionCount; /**< Number of versions */
    int currentVersion; /**< Index of the current version */
} VersionedHAMT;

/**
 * @brief Structure representing a node in a queue used for breadth-first traversal.
 */
typedef struct QueueNode {
    HAMTNode *node; /**< HAMT node */
    int depth; /**< Depth of the node */
    struct QueueNode *next; /**< Pointer to the next node in the queue */
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
