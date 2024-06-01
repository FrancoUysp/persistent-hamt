#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "assoc_std.h"

#define INITIAL_CAPACITY 100

unsigned int stdHashFunction(const char *key) {
    unsigned int hash = 0;
    while (*key) {
        hash = (hash << 5) + *key++;
    }
    return hash;
}

StdHashTable* createStdHashTable(int capacity) {
    StdHashTable *table = (StdHashTable *)malloc(sizeof(StdHashTable));
    table->entries = (StdEntry **)malloc(sizeof(StdEntry *) * capacity);
    for (int i = 0; i < capacity; i++) {
        table->entries[i] = NULL;
    }
    table->size = 0;
    table->capacity = capacity;
    return table;
}

void stdHashTableInsert(StdHashTable *table, const char *key, int value) {
    unsigned int index = stdHashFunction(key) % table->capacity;
    while (table->entries[index] != NULL && strcmp(table->entries[index]->key, key) != 0) {
        index = (index + 1) % table->capacity;
    }
    if (table->entries[index] == NULL) {
        table->entries[index] = (StdEntry *)malloc(sizeof(StdEntry));
        table->entries[index]->key = strdup(key);
        table->size++;
    }
    table->entries[index]->value = value;
}

int stdHashTableSearch(StdHashTable *table, const char *key) {
    unsigned int index = stdHashFunction(key) % table->capacity;
    while (table->entries[index] != NULL && strcmp(table->entries[index]->key, key) != 0) {
        index = (index + 1) % table->capacity;
    }
    if (table->entries[index] == NULL) {
        return -1; // Key not found
    }
    return table->entries[index]->value;
}

void stdHashTableDelete(StdHashTable *table, const char *key) {
    unsigned int index = stdHashFunction(key) % table->capacity;
    while (table->entries[index] != NULL && strcmp(table->entries[index]->key, key) != 0) {
        index = (index + 1) % table->capacity;
    }
    if (table->entries[index] != NULL) {
        free(table->entries[index]->key);
        free(table->entries[index]);
        table->entries[index] = NULL;
        table->size--;
    }
}

void freeStdHashTable(StdHashTable *table) {
    for (int i = 0; i < table->capacity; i++) {
        if (table->entries[i] != NULL) {
            free(table->entries[i]->key);
            free(table->entries[i]);
        }
    }
    free(table->entries);
    free(table);
}
