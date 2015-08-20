/**
 * @author: Victor Caballero (vicaba)
 */
#include "./cLinkedList.h"

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
    void (*freeFunc)(void *)
);

int cllist_toArray(cLinkedList *cllist);

int cllist_foreach(cLinkedList *cllist, int (*doFunc)(void*));

int cllist_zipWithIndex(cLinkedList *cllist1, cLinkedList *cllist2, int (*zipFunc)(void*, void*, int));

int cllist_sync(cLinkedList *cllist, int (*callback)(LinkedList *llist));

int cllist_rsync(cLinkedList *cllist, int (*callback)(LinkedList *llist));

int cllist_new(void **cllist)
{
	cLinkedList **_cllist = (cLinkedList **)cllist;
	*_cllist = (cLinkedList *)malloc(sizeof(cLinkedList));

	llist_new((void **) & ((*_cllist)->llist));
	pthread_mutex_init(&((*_cllist)->mutex), NULL);

	return 0;
}

int cllist_copy(void *newCllist, void *oldCllist)
{
	cLinkedList *_newCllist = (cLinkedList *)newCllist;
	cLinkedList *_oldCllist = (cLinkedList *)oldCllist;
	int returnVal;

	pthread_mutex_lock(&(_newCllist->mutex));
	pthread_mutex_lock(&(_oldCllist->mutex));

	returnVal = llist_copy(_newCllist->llist, _oldCllist->llist);

	pthread_mutex_unlock(&(_newCllist->mutex));
	pthread_mutex_unlock(&(_oldCllist->mutex));

	return returnVal;

}

void cllist_destroy(void *cllist)
{
	cLinkedList *_cllist = (cLinkedList *)cllist;

	pthread_mutex_lock(&(_cllist->mutex));

	llist_destroy(_cllist->llist);

	pthread_mutex_unlock(&(_cllist->mutex));
	pthread_mutex_destroy(&(_cllist->mutex));

}

int cllist_size(cLinkedList *cllist)
{
	int size;

	pthread_mutex_lock(&(cllist->mutex));

	size = llist_size(cllist->llist);

	pthread_mutex_unlock(&(cllist->mutex));

	return size;
}

int cllist_append(cLinkedList *cllist, void *elem)
{
	int returnVal;

	pthread_mutex_lock(&(cllist->mutex));

	returnVal = llist_append(cllist->llist, elem);

	pthread_mutex_unlock(&(cllist->mutex));

	return returnVal;
}

void* cllist_getElementByIndex(cLinkedList *cllist, int index)
{
	void *elem;

	pthread_mutex_lock(&(cllist->mutex));

	elem = llist_getElementByIndex(cllist->llist, index);

	pthread_mutex_unlock(&(cllist->mutex));

	return elem;
}

void* cllist_getFirstOcurrenceOfElement(cLinkedList *cllist, int (*cmpFunc)(void*))
{
	void *elem;

	pthread_mutex_lock(&(cllist->mutex));

	elem = llist_getFirstOcurrenceOfElement(cllist->llist, cmpFunc);

	pthread_mutex_unlock(&(cllist->mutex));

	return elem;
}

int cllist_deleteElementByIndex(cLinkedList *cllist, int index)
{
	int returnVal;

	pthread_mutex_lock(&(cllist->mutex));

	returnVal = llist_deleteElementByIndex(cllist->llist, index);

	pthread_mutex_unlock(&(cllist->mutex));

	return returnVal;
}

void cllist_setElemFuncs(
    cLinkedList *cllist,
    int (*newFunc)(void **),
    void (*cpyFunc)(void *, void *),
    int (*cmpFunc)(void *, void *),
    void (*freeFunc)(void *)
)
{
	pthread_mutex_lock(&(cllist->mutex));

	llist_setElemFuncs(
	    cllist->llist,
	    newFunc,
	    cpyFunc,
	    cmpFunc,
	    freeFunc
	);

	pthread_mutex_unlock(&(cllist->mutex));

}

int cllist_foreach(cLinkedList *cllist, int (*doFunc)(void*))
{
	pthread_mutex_lock(&(cllist->mutex));

	llist_foreach(cllist->llist, doFunc);

	pthread_mutex_unlock(&(cllist->mutex));

	return 0;
}

int cllist_zipWithIndex(cLinkedList *cllist1, cLinkedList *cllist2, int (*zipFunc)(void*, void*, int))
{
	int returnVal;
	
	pthread_mutex_lock(&(cllist1->mutex));
	pthread_mutex_lock(&(cllist2->mutex));

	returnVal = llist_zipWithIndex(cllist1->llist, cllist2->llist, zipFunc);

	pthread_mutex_unlock(&(cllist1->mutex));
	pthread_mutex_unlock(&(cllist2->mutex));

	return returnVal;
}


int cllist_sync(cLinkedList *cllist, int (*callback)(LinkedList *llist))
{

	int returnVal;

	pthread_mutex_lock(&(cllist->mutex));

	returnVal = callback(cllist->llist);

	pthread_mutex_unlock(&(cllist->mutex));

	return returnVal;
}

int cllist_rsync(cLinkedList *cllist, int (*callback)(LinkedList *llist))
{

	int returnVal;

	pthread_mutex_lock(&(cllist->mutex));

	returnVal = callback(cllist->llist);

	pthread_mutex_unlock(&(cllist->mutex));

	return returnVal;
}

