#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "associative_array.h"
#include "linked_list.h"

void measureInsertionTimeAssociativeArray(int n, AssociativeArray *assocArray);
void measureSearchTimeAssociativeArray(int n, AssociativeArray *assocArray);
void measureDeletionTimeAssociativeArray(int n, AssociativeArray *assocArray);

void measureInsertionTimeLinkedList(int n, LinkedList *list);
void measureSearchTimeLinkedList(int n, LinkedList *list);
void measureDeletionTimeLinkedList(int n, LinkedList *list);

void speedExperiment();

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Run experiments
    speedExperiment();

    return 0;
}

void measureInsertionTimeAssociativeArray(int n, AssociativeArray *assocArray) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        associativeArrayInsert(assocArray, i, i);
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Insert time for %d elements in Associative Array: %f seconds\n", n, timeSpent);
}

void measureSearchTimeAssociativeArray(int n, AssociativeArray *assocArray) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        associativeArraySearch(assocArray, i, assocArray->hamt->currentVersion);
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Search time for %d elements in Associative Array: %f seconds\n", n, timeSpent);
}

void measureDeletionTimeAssociativeArray(int n, AssociativeArray *assocArray) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        associativeArrayDelete(assocArray, i);
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Delete time for %d elements in Associative Array: %f seconds\n", n, timeSpent);
}

void measureInsertionTimeLinkedList(int n, LinkedList *list) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        linkedListInsert(list, i);
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Insert time for %d elements in Linked List: %f seconds\n", n, timeSpent);
}

void measureSearchTimeLinkedList(int n, LinkedList *list) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        linkedListGet(list, i, list->hamt->currentVersion);
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Search time for %d elements in Linked List: %f seconds\n", n, timeSpent);
}

void measureDeletionTimeLinkedList(int n, LinkedList *list) {
    clock_t start = clock();
    for (int i = 0; i < n; i++) {
        linkedListDelete(list, 0); // always delete the first element for simplicity
    }
    clock_t end = clock();
    double timeSpent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Delete time for %d elements in Linked List: %f seconds\n", n, timeSpent);
}

void speedExperiment() {
    printf("Running speed experiment for Associative Array...\n");

    // Create associative array
    AssociativeArray *assocArray = createAssociativeArray();

    // Measure times for various sizes
    int sizes[] = {10, 50, 100, 500, 1000, 5000, 10000};
    int numSizes = sizeof(sizes) / sizeof(sizes[0]);
    for (int i = 0; i < numSizes; i++) {
        measureInsertionTimeAssociativeArray(sizes[i], assocArray);
        measureSearchTimeAssociativeArray(sizes[i], assocArray);
        measureDeletionTimeAssociativeArray(sizes[i], assocArray);
    }

    // Free associative array
    freeAssociativeArray(assocArray);

    printf("Running speed experiment for Linked List...\n");

    // Create linked list
    LinkedList *list = createLinkedList();

    // Measure times for various sizes
    for (int i = 0; i < numSizes; i++) {
        measureInsertionTimeLinkedList(sizes[i], list);
        measureSearchTimeLinkedList(sizes[i], list);
        measureDeletionTimeLinkedList(sizes[i], list);
    }

    // Free linked list
    freeLinkedList(list);
}
