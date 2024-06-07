#include <stdio.h>
#include <stdlib.h>
#include "linked_list_hamt.h"

LinkedListHAMT* createLinkedListHAMT() {
    LinkedListHAMT* list = (LinkedListHAMT*)malloc(sizeof(LinkedListHAMT));
    list->vhamt = createVersionedHAMT();
    list->version = 0;
    return list;
}

void addLinkedListHAMT(LinkedListHAMT* list, int index, int data) {
    insertVersion(list->vhamt, index, data, list->version);
    list->version++;
}

void updateLinkedListHAMT(LinkedListHAMT* list, int index, int newData) {
    SearchResult result = searchVersion(list->vhamt, index, list->version - 1);
    if (result.valueCount > 0) {
        updateVersion(list->vhamt, index, result.values[0], newData, list->version);
        list->version++;
    }
}

void deleteLinkedListHAMT(LinkedListHAMT* list, int index) {
    SearchResult result = searchVersion(list->vhamt, index, list->version - 1);
    if (result.valueCount > 0) {
        deleteVersion(list->vhamt, index, result.values[0], list->version);
        list->version++;
    }
}

SearchResult searchLinkedListHAMT(LinkedListHAMT* list, int data) {
    return searchVersion(list->vhamt, data, list->version - 1);
}
