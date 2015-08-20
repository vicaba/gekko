/**
 * @author: Victor Caballero (vicaba)
 * @brief A thread-safe implementation of a linked list.
 * Refer to LinkedList docblocks for information about the purpose of the basic functions
 */
#ifndef __CLINKEDLIST_H__
#define __CLINKEDLIST_H__

#include <pthread.h>
#include "../LinkedList/LinkedList.h"

typedef struct cLinkedList {
	pthread_mutex_t mutex;
	LinkedList *llist;
} cLinkedList;


int cllist_new(void **cllist);

int cllist_copy(void *newCllist, void *oldCllist);

void cllist_destroy(void *cllist);

int cllist_size(cLinkedList *cllist);

int cllist_append(cLinkedList *cllist, void *elem);

int cllist_prepend(cLinkedList *cllist, void *elem);

void* cllist_getElementByIndex(cLinkedList *cllist, int index);

void* cllist_getFirstOcurrenceOfElement(cLinkedList *cllist, int (*cmpFunc)(void*));

int cllist_deleteElementByIndex(cLinkedList *cllist, int index);

void cllist_setElemFuncs(
    cLinkedList *cllist,
    int (*newFunc)(void **),
    void (*cpyFunc)(void *, void *),
    int (*cmpFunc)(void *, void *),
    void (*freeFunc)(void *));

int cllist_toArray(cLinkedList *cllist);

int cllist_foreach(cLinkedList *cllist, int (*doFunc)(void*));

int cllist_zipWithIndex(cLinkedList *cllist1, cLinkedList *cllist2, int (*zipFunc)(void*, void*, int));

/**
 * Uses the cllist synchronization mutex to execute the callback
 * @param clist
 * @param callback
 */
int cllist_sync(cLinkedList *cllist, int (*callback)(LinkedList *llist));

/**
 * The same as cllist_sync. This function is here for interfacing purposes.
 * The goal is to wrap the callbacks with the cAlgthm.rw in the sync and rsync functions respectively
 */
int cllist_rsync(cLinkedList *cllist, int (*callback)(LinkedList *llist));

#endif
