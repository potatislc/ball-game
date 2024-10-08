#ifndef BALL_GAME_LINKED_LIST_H
#define BALL_GAME_LINKED_LIST_H

#include <stdlib.h>

#define LIST_MAP_DATA(headNode, function, type) \
do                                              \
{                                               \
    ListNode* currentNode = headNode;           \
    while (currentNode != NULL)                 \
    {                                           \
        if (currentNode->data != NULL) function( type currentNode->data);      \
        currentNode = currentNode->next;        \
    }                                           \
} while (0)                                     \

typedef struct ListNode
{
    void *next;
    void *data;
} ListNode;

extern ListNode *head;

ListNode* ListNodeCreate(void* data);
void ListNodePush(ListNode** headRef, void* newData); // Pushes that node to the head of the list
void ListNodeRemove(ListNode** headRef, void* key);
void ListNodeRemoveActual(ListNode** headRef, void* key);
void ListRemoveAllNodes(ListNode** headRef);
int ListLength(ListNode** headRef);
void ListNodePrint(ListNode* node);

//
// Freeing Queue
//
void FreeingQueuePush(ListNode* node);
void FreeingQueueFreeAll();

#endif //BALL_GAME_LINKED_LIST_H
