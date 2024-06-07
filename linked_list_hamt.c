#include "linked_list_hamt.h"

// Create a new linked list using HAMT
LinkedListHAMT* createLinkedListHAMT() {
    LinkedListHAMT* list = (LinkedListHAMT*)malloc(sizeof(LinkedListHAMT));
    list->vhamt = createVersionedHAMT();
    list->version = 0;
    list->size = 0;
    return list;
}

// Add a node at a specific index
void ll_add(LinkedListHAMT* list, int index, int data) {
    if (index < 0 || index > list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    insertVersion(list->vhamt, index, data, list->version);
    list->version = list->vhamt->versionCount - 1;
    list->size++;
}

// Update data at a specific index
void ll_update(LinkedListHAMT* list, int index, int newData) {
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    SearchResult result = searchVersion(list->vhamt, index, list->version);
    if (result.valueCount > 0) {
        updateVersion(list->vhamt, index, result.values[0], newData, list->version);
        list->version = list->vhamt->versionCount - 1;
    }
}

// Delete a node at a specific index
void ll_delete(LinkedListHAMT* list, int index) {
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    SearchResult result = searchVersion(list->vhamt, index, list->version);
    if (result.valueCount > 0) {
        deleteVersion(list->vhamt, index, result.values[0], list->version);
        list->version = list->vhamt->versionCount - 1;
        list->size--;
    }
}

// Search for a node with the given data
SearchResult ll_search(LinkedListHAMT* list, int data) {
    for (int i = 0; i < list->size; i++) {
        SearchResult result = searchVersion(list->vhamt, i, list->version);
        if (result.valueCount > 0 && result.values[0] == data) {
            return result;
        }
    }
    SearchResult emptyResult = {NULL, 0};
    return emptyResult; // Node not found
}
