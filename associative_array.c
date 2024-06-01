#include "hamt.h"
#include "associative_array.h"

AssociativeArray* createAssociativeArray() {
    AssociativeArray *array = (AssociativeArray *)malloc(sizeof(AssociativeArray));
    array->hamt = createVersionedHAMT(MAX_LEVEL, BIT_SEG);
    return array;
}

void associativeArrayInsert(AssociativeArray *array, int key, int value) {
    insertHAMT(array->hamt, key, value);
}

int associativeArraySearch(AssociativeArray *array, int key, int version) {
    SearchResult result = searchHAMT(array->hamt, key, version);
    if (result.valueCount > 0) {
        return result.values[0];
    }
    return -1; // Key not found
}

void associativeArrayDelete(AssociativeArray *array, int key) {
    deleteHAMT(array->hamt, key, 0); // Assuming the value to be deleted is not needed
}

void freeAssociativeArray(AssociativeArray *array) {
    for (int i = 0; i < array->hamt->versionCount; ++i) {
        freeHAMT(array->hamt->versions[i]);
    }
    free(array->hamt->versions);
    free(array->hamt);
    free(array);
}
