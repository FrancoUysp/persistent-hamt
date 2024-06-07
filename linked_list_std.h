#ifndef LINKED_LIST_STD_H
#define LINKED_LIST_STD_H

#include <stddef.h>

/**
 * @brief Structure representing a standard linked list.
 */
typedef struct LinkedList {
    int data; /**< Data stored in the node */
    struct LinkedList* next; /**< Pointer to the next node */
} LinkedList;

/**
 * @brief Creates a new standard linked list.
 *
 * @return Pointer to the newly created LinkedList.
 */
LinkedList* createLinkedListSTD();

/**
 * @brief Adds a new node with data at the specified index in the linked list.
 *
 * @param list Pointer to the linked list.
 * @param index Index where the new node should be added.
 * @param data Data to be stored in the new node.
 */
void addLinkedListSTD(LinkedList* list, int index, int data);

/**
 * @brief Updates the data at the specified index in the linked list.
 *
 * @param list Pointer to the linked list.
 * @param index Index of the node to be updated.
 * @param newData New data to be stored in the node.
 */
void updateLinkedListSTD(LinkedList* list, int index, int newData);

/**
 * @brief Deletes the node at the specified index from the linked list.
 *
 * @param list Pointer to the linked list.
 * @param index Index of the node to be deleted.
 */
void deleteLinkedListSTD(LinkedList* list, int index);

/**
 * @brief Searches for a specific data value in the linked list.
 *
 * @param list Pointer to the linked list.
 * @param data Data value to search for.
 * @return 1 if data is found, 0 otherwise.
 */
int searchLinkedListSTD(LinkedList* list, int data);

#endif // LINKED_LIST_STD_H
