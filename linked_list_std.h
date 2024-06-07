#ifndef LINKED_LIST_STD_H
#define LINKED_LIST_STD_H

#include <stddef.h>

typedef struct LinkedList {
    int data;
    struct LinkedList* next;
} LinkedList;

LinkedList* createLinkedListSTD();
void addLinkedListSTD(LinkedList* list, int index, int data);
void updateLinkedListSTD(LinkedList* list, int index, int newData);
void deleteLinkedListSTD(LinkedList* list, int index);
int searchLinkedListSTD(LinkedList* list, int data);

#endif // LINKED_LIST_STD_H
