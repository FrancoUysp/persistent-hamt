#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "associative_array.h"
#include "linked_list.h"
#include "assoc_std.h"
#include "linkd_std.h"

#ifdef __APPLE__
#include <malloc/malloc.h>
#else
#include <windows.h>
#include <psapi.h>
#endif

#define DATA_POINTS 10
#define INSERTIONS 10000

void printMemoryUsage() {
    #ifdef __APPLE__
    malloc_statistics_t stats;
    malloc_zone_statistics(NULL, &stats);
    printf("Memory in use: %lu bytes\n", stats.size_in_use);
    #else
    PROCESS_MEMORY_COUNTERS_EX pmc;
    GetProcessMemoryInfo(GetCurrentProcess(), (PROCESS_MEMORY_COUNTERS*)&pmc, sizeof(pmc));
    SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
    printf("Memory in use: %zu bytes\n", physMemUsedByMe);
    #endif
}

void memoryExperiment() {
    printf("Running memory experiment...\n");

    // Measure memory usage for HAMT-based associative array
    AssociativeArray *hamtArray = createAssociativeArray();
    for (int i = 0; i < INSERTIONS; ++i) {
        associativeArrayInsert(hamtArray, i, i);
    }
    printMemoryUsage();
    freeAssociativeArray(hamtArray);

    // Measure memory usage for standard associative array
    StdHashTable *stdArray = createStdHashTable(INSERTIONS);
    for (int i = 0; i < INSERTIONS; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        stdHashTableInsert(stdArray, key, i);
    }
    printMemoryUsage();
    freeStdHashTable(stdArray);

    // Measure memory usage for HAMT-based linked list
    LinkedList *hamtList = createLinkedList();
    for (int i = 0; i < INSERTIONS; ++i) {
        linkedListInsert(hamtList, i);
    }
    printMemoryUsage();
    freeLinkedList(hamtList);

    // Measure memory usage for standard linked list
    StdLinkedList *stdList = createStdLinkedList();
    for (int i = 0; i < INSERTIONS; ++i) {
        stdLinkedListInsert(stdList, i);
    }
    printMemoryUsage();
    freeStdLinkedList(stdList);
}

void speedExperiment() {
    printf("Running speed experiment...\n");
    clock_t start, end;
    double cpu_time_used;

    // Measure speed for HAMT-based associative array
    AssociativeArray *hamtArray = createAssociativeArray();
    start = clock();
    for (int i = 0; i < INSERTIONS; ++i) {
        associativeArrayInsert(hamtArray, i, i);
    }
    for (int i = 0; i < INSERTIONS; ++i) {
        associativeArraySearch(hamtArray, i, hamtArray->hamt->currentVersion);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("HAMT Associative Array speed: %f seconds\n", cpu_time_used);
    freeAssociativeArray(hamtArray);

    // Measure speed for standard associative array
    StdHashTable *stdArray = createStdHashTable(INSERTIONS);
    start = clock();
    for (int i = 0; i < INSERTIONS; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        stdHashTableInsert(stdArray, key, i);
    }
    for (int i = 0; i < INSERTIONS; ++i) {
        char key[20];
        sprintf(key, "key%d", i);
        stdHashTableSearch(stdArray, key);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Standard Associative Array speed: %f seconds\n", cpu_time_used);
    freeStdHashTable(stdArray);

    // Measure speed for HAMT-based linked list
    LinkedList *hamtList = createLinkedList();
    start = clock();
    for (int i = 0; i < INSERTIONS; ++i) {
        linkedListInsert(hamtList, i);
    }
    for (int i = 0; i < INSERTIONS; ++i) {
        linkedListGet(hamtList, i, hamtList->hamt->currentVersion);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("HAMT Linked List speed: %f seconds\n", cpu_time_used);
    freeLinkedList(hamtList);

    // Measure speed for standard linked list
    StdLinkedList *stdList = createStdLinkedList();
    start = clock();
    for (int i = 0; i < INSERTIONS; ++i) {
        stdLinkedListInsert(stdList, i);
    }
    for (int i = 0; i < INSERTIONS; ++i) {
        stdLinkedListGet(stdList, i);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)) / CLOCKS_PER_SEC;
    printf("Standard Linked List speed: %f seconds\n", cpu_time_used);
    freeStdLinkedList(stdList);
}

void thirdExperiment() {
    printf("Running third experiment...\n");
    // Additional experiment implementation
}

int main() {
    // Initialize random seed
    srand(time(NULL));

    // Run experiments
    memoryExperiment();
    speedExperiment();
    thirdExperiment();

    return 0;
}
