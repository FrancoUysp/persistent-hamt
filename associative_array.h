#ifndef ASSOCIATIVE_ARRAY_H
#define ASSOCIATIVE_ARRAY_H

#include "hamt.h"

typedef struct {
    VersionedHAMT *hamt;
} AssociativeArray;

AssociativeArray* createAssociativeArray();
void associativeArrayInsert(AssociativeArray *array, int key, int value);
int associativeArraySearch(AssociativeArray *array, int key, int version);
void associativeArrayDelete(AssociativeArray *array, int key);
void freeAssociativeArray(AssociativeArray *array);

#endif // ASSOCIATIVE_ARRAY_H
