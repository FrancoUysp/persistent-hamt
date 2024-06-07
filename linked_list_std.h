#ifndef LINKED_LIST_STD_H
#define LINKED_LIST_STD_H

#include <stdio.h>
#include <stdlib.h>

// Define a node structure
typedef struct Node {
    int data;
    struct Node* next;
} Node;

// Define a linked list structure
typedef struct LinkedList {
    Node* head;
    int size;
} LinkedList;

// Function declarations
LinkedList* createLinkedList();
void add(LinkedList* list, int index, int data);
void update(LinkedList* list, int index, int newData);
void delete(LinkedList* list, int index);
Node* search(LinkedList* list, int data);

#endif // LINKED_LIST_STD_H
