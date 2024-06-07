#include "linked_list_std.h"
#include <stdlib.h>

/**
* @brief Creates a new standard linked list.
*
* @return Pointer to the newly created LinkedList.
*/
__attribute__((visibility("default"))) LinkedList* createLinkedListSTD() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->data = 0;
    list->next = NULL;
    return list;
}

/**
* @brief Adds a new node with data at the specified index in the linked list.
*
* @param list Pointer to the linked list.
* @param index Index where the new node should be added.
* @param data Data to be stored in the new node.
*/
void addLinkedListSTD(LinkedList* list, int index, int data) {
    LinkedList* newNode = (LinkedList*)malloc(sizeof(LinkedList));
    newNode->data = data;
    newNode->next = NULL;

    LinkedList* current = list;
    for (int i = 0; i < index - 1; i++) {
        if (current->next == NULL) break;
        current = current->next;
    }
    newNode->next = current->next;
    current->next = newNode;
}

/**
* @brief Updates the data at the specified index in the linked list.
*
* @param list Pointer to the linked list.
* @param index Index of the node to be updated.
* @param newData New data to be stored in the node.
*/
void updateLinkedListSTD(LinkedList* list, int index, int newData) {
    LinkedList* current = list;
    for (int i = 0; i < index; i++) {
        if (current->next == NULL) return;
        current = current->next;
    }
    current->data = newData;
}

/**
* @brief Deletes the node at the specified index from the linked list.
*
* @param list Pointer to the linked list.
* @param index Index of the node to be deleted.
*/
void deleteLinkedListSTD(LinkedList* list, int index) {
    LinkedList* current = list;
    LinkedList* previous = NULL;

    for (int i = 0; i < index; i++) {
        if (current->next == NULL) return;
        previous = current;
        current = current->next;
    }

    if (previous == NULL) {
        LinkedList* temp = list->next;
        list->data = list->next->data;
        list->next = list->next->next;
        free(temp);
    } else {
        previous->next = current->next;
        free(current);
    }
}

/**
* @brief Searches for a specific data value in the linked list.
*
* @param list Pointer to the linked list.
* @param data Data value to search for.
* @return 1 if data is found, 0 otherwise.
*/
int searchLinkedListSTD(LinkedList* list, int data) {
    LinkedList* current = list;
    while (current != NULL) {
        if (current->data == data) {
            return 1; // Found
        }
        current = current->next;
    }
    return 0; // Not found
}
