#ifndef LINKED_LIST_HAMT_H
#define LINKED_LIST_HAMT_H

#include "hamt.h"

typedef struct LinkedListHAMT {
    VersionedHAMT* vhamt;
    int version;
    int size;
} LinkedListHAMT;

LinkedListHAMT* createLinkedListHAMT();
void ll_add(LinkedListHAMT* list, int index, int data);
void ll_update(LinkedListHAMT* list, int index, int newData);
void ll_delete(LinkedListHAMT* list, int index);
SearchResult ll_search(LinkedListHAMT* list, int data);

#endif // LINKED_LIST_HAMT_H
