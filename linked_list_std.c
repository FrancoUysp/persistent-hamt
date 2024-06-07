#include "linked_list_std.h"
#include <stdlib.h>

LinkedList* createLinkedListSTD() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->data = 0;
    list->next = NULL;
    return list;
}

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

void updateLinkedListSTD(LinkedList* list, int index, int newData) {
    LinkedList* current = list;
    for (int i = 0; i < index; i++) {
        if (current->next == NULL) return;
        current = current->next;
    }
    current->data = newData;
}

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
