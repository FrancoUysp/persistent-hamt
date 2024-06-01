#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include "hamt.h"

typedef struct {
    VersionedHAMT *hamt;
    int head; // Key of the head element
    int tail; // Key of the tail element
    int size; // Number of elements in the list
} LinkedList;

LinkedList* createLinkedList();
void linkedListInsert(LinkedList *list, int value);
int linkedListGet(LinkedList *list, int index, int version);
void linkedListDelete(LinkedList *list, int index);
void freeLinkedList(LinkedList *list);

#endif // LINKED_LIST_H
