#include <stdio.h>
#include <stdlib.h>
#include "linkd_std.h"

StdLinkedList* createStdLinkedList() {
    StdLinkedList *list = (StdLinkedList *)malloc(sizeof(StdLinkedList));
    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    return list;
}

void stdLinkedListInsert(StdLinkedList *list, int value) {
    StdListNode *newNode = (StdListNode *)malloc(sizeof(StdListNode));
    newNode->value = value;
    newNode->next = NULL;
    if (list->tail != NULL) {
        list->tail->next = newNode;
    } else {
        list->head = newNode;
    }
    list->tail = newNode;
    list->size++;
}

int stdLinkedListGet(StdLinkedList *list, int index) {
    if (index < 0 || index >= list->size) {
        return -1; // Index out of bounds
    }
    StdListNode *current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    return current->value;
}

void stdLinkedListDelete(StdLinkedList *list, int index) {
    if (index < 0 || index >= list->size) {
        return; // Index out of bounds
    }
    StdListNode *current = list->head;
    StdListNode *previous = NULL;
    for (int i = 0; i < index; i++) {
        previous = current;
        current = current->next;
    }
    if (previous != NULL) {
        previous->next = current->next;
    } else {
        list->head = current->next;
    }
    if (current == list->tail) {
        list->tail = previous;
    }
    free(current);
    list->size--;
}

void freeStdLinkedList(StdLinkedList *list) {
    StdListNode *current = list->head;
    while (current != NULL) {
        StdListNode *next = current->next;
        free(current);
        current = next;
    }
    free(list);
}
