#ifndef LINKED_LIST_HAMT_H
#define LINKED_LIST_HAMT_H

#include "hamt.h"

/**
 * @brief Structure representing a linked list based on HAMT (Hash Array Mapped Trie).
 */
typedef struct LinkedListHAMT {
    VersionedHAMT* vhamt; /**< Pointer to the VersionedHAMT structure */
    int version; /**< Current version of the linked list */
    int size; /**< Size of the linked list */
} LinkedListHAMT;

/**
 * @brief Creates a new linked list based on HAMT (Hash Array Mapped Trie).
 *
 * @return Pointer to the newly created LinkedListHAMT.
 */
LinkedListHAMT* createLinkedListHAMT();

/**
 * @brief Adds a new node with data at the specified index in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index where the new node should be added.
 * @param data Data to be stored in the new node.
 */
void ll_add(LinkedListHAMT* list, int index, int data);

/**
 * @brief Updates the data at the specified index in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index of the node to be updated.
 * @param newData New data to be stored in the node.
 */
void ll_update(LinkedListHAMT* list, int index, int newData);

/**
 * @brief Deletes the node at the specified index from the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index of the node to be deleted.
 */
void ll_delete(LinkedListHAMT* list, int index);

/**
 * @brief Searches for a specific data value in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param data Data value to search for.
 * @return SearchResult containing the indexes where the data is found.
 */
SearchResult ll_search(LinkedListHAMT* list, int data);

#endif // LINKED_LIST_HAMT_H
