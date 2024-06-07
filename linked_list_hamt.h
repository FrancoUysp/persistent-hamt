#ifndef LINKED_LIST_HAMT_H
#define LINKED_LIST_HAMT_H

#include "hamt.h"

typedef struct LinkedListHAMT {
    VersionedHAMT* vhamt;
    int version;
} LinkedListHAMT;

LinkedListHAMT* createLinkedListHAMT();
void addLinkedListHAMT(LinkedListHAMT* list, int index, int data);
void updateLinkedListHAMT(LinkedListHAMT* list, int index, int newData);
void deleteLinkedListHAMT(LinkedListHAMT* list, int index);
SearchResult searchLinkedListHAMT(LinkedListHAMT* list, int data);

#endif // LINKED_LIST_HAMT_H
