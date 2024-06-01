#ifndef ASSOC_STD_H
#define ASSOC_STD_H

typedef struct {
    char *key;
    int value;
} StdEntry;

typedef struct {
    StdEntry **entries;
    int size;
    int capacity;
} StdHashTable;

StdHashTable* createStdHashTable(int capacity);
void stdHashTableInsert(StdHashTable *table, const char *key, int value);
int stdHashTableSearch(StdHashTable *table, const char *key);
void stdHashTableDelete(StdHashTable *table, const char *key);
void freeStdHashTable(StdHashTable *table);

#endif // ASSOC_STD_H
