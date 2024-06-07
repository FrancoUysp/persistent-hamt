#include "linked_list_std.h"

// Create a new linked list
LinkedList* createLinkedList() {
    LinkedList* list = (LinkedList*)malloc(sizeof(LinkedList));
    list->head = NULL;
    list->size = 0;
    return list;
}

// Add a node at a specific index
void add(LinkedList* list, int index, int data) {
    if (index < 0 || index > list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->data = data;
    newNode->next = NULL;

    if (index == 0) {
        newNode->next = list->head;
        list->head = newNode;
    } else {
        Node* current = list->head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }

    list->size++;
}

// Update data at a specific index
void update(LinkedList* list, int index, int newData) {
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    Node* current = list->head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    current->data = newData;
}

// Delete a node at a specific index
void delete(LinkedList* list, int index) {
    if (index < 0 || index >= list->size) {
        fprintf(stderr, "Index out of bounds\n");
        return;
    }

    Node* current = list->head;
    if (index == 0) {
        list->head = current->next;
        free(current);
    } else {
        Node* previous = NULL;
        for (int i = 0; i < index; i++) {
            previous = current;
            current = current->next;
        }
        previous->next = current->next;
        free(current);
    }

    list->size--;
}

// Search for a node with the given data
Node* search(LinkedList* list, int data) {
    Node* current = list->head;
    while (current) {
        if (current->data == data) {
            return current;
        }
        current = current->next;
    }
    return NULL; // Node not found
}
