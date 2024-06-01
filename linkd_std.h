#ifndef LINKD_STD_H
#define LINKD_STD_H

typedef struct StdListNode {
    int value;
    struct StdListNode *next;
} StdListNode;

typedef struct {
    StdListNode *head;
    StdListNode *tail;
    int size;
} StdLinkedList;

StdLinkedList* createStdLinkedList();
void stdLinkedListInsert(StdLinkedList *list, int value);
int stdLinkedListGet(StdLinkedList *list, int index);
void stdLinkedListDelete(StdLinkedList *list, int index);
void freeStdLinkedList(StdLinkedList *list);

#endif // LINKD_STD_H
