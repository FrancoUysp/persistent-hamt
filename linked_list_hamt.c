#include "linked_list_hamt.h"

/**
 * @brief Creates a new linked list based on HAMT (Hash Array Mapped Trie).
 *
 * @return Pointer to the newly created LinkedListHAMT.
 */
LinkedListHAMT* createLinkedListHAMT() {
    LinkedListHAMT* list = (LinkedListHAMT*)malloc(sizeof(LinkedListHAMT));
    list->vhamt = createVersionedHAMT();
    list->version = 0;
    list->size = 0;
    return list;
}

/**
 * @brief Adds a new node with data at the specified index in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index where the new node should be added.
 * @param data Data to be stored in the new node.
 */
void ll_add(LinkedListHAMT* list, int index, int data) {
    if (index < 0 || index > list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    insertVersion(list->vhamt, index, data, list->version);
    list->version = list->vhamt->versionCount - 1;
    list->size++;
}

/**
 * @brief Updates the data at the specified index in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index of the node to be updated.
 * @param newData New data to be stored in the node.
 */
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

/**
 * @brief Deletes the node at the specified index from the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param index Index of the node to be deleted.
 */
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

/**
 * @brief Searches for a specific data value in the linked list.
 *
 * @param list Pointer to the LinkedListHAMT.
 * @param data Data value to search for.
 * @return SearchResult containing the indexes where the data is found.
 */
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
