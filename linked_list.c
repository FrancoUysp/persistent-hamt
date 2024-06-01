#include "hamt.h"
#include "linked_list.h"

LinkedList* createLinkedList() {
    LinkedList *list = (LinkedList *)malloc(sizeof(LinkedList));
    list->hamt = createVersionedHAMT(MAX_LEVEL, BIT_SEG);
    return list;
}

void linkedListInsert(LinkedList *list, int value) {
    static int key = 0;
    insertHAMT(list->hamt, key++, value);
}

int linkedListGet(LinkedList *list, int index, int version) {
    SearchResult result = searchHAMT(list->hamt, index, version);
    if (result.valueCount > 0) {
        return result.values[0];
    }
    return -1; // Index not found
}

void linkedListDelete(LinkedList *list, int index) {
    deleteHAMT(list->hamt, index, 0); // Assuming the value to be deleted is not needed
}

void freeLinkedList(LinkedList *list) {
    for (int i = 0; i < list->hamt->versionCount; ++i) {
        freeHAMT(list->hamt->versions[i]);
    }
    free(list->hamt->versions);
    free(list->hamt);
    free(list);
}
