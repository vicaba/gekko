/**
 * @author: Victor Caballero (vicaba)
 * @author: Victor Caballero
 */
#include "LinkedList.h"

int llist_new(void **llist);

int llist_copy(LinkedList *newLlist, LinkedList *oldLlist);

void llist_destroy(LinkedList *llist);

int llist_size(LinkedList *llist);

int llist_append(LinkedList *llist, void *elem);

int llist_prepend(LinkedList *llist, void *elem);

void* llist_getElementByIndex(LinkedList *llist, int index);

void* llist_getFirstOcurrenceOfElement(LinkedList *llist, int (*cmpFunc)(void*));

int llist_deleteElementByIndex(LinkedList *llist, int index);

int llist_deleteElement(LinkedList *llist, void *elem);

void llist_setElemFuncs(
    LinkedList *llist,
    int (*newFunc)(void **),
    void (*cpyFunc)(void *, void *),
    int (*cmpFunc)(void *, void *),
    void (*freeFunc)(void *)
);

int llist_foreach(LinkedList *llist, int (*doFunc)(void*));

int llist_zipWithIndex(LinkedList *llist1, LinkedList *llist2, int (*zipFunc)(void*, void*, int));

static LinkedListNode* _newNode();

static LinkedListNode* _getNodeByIndex(LinkedList *llist, int index);

static LinkedListNode* _getNodeByElem(LinkedList *llist, void *elem);

int llist_new(void **llist)
{
	LinkedList **_llist = (LinkedList **)llist;
	LinkedListNode *newNode = _newNode();
	*_llist = (LinkedList *)malloc(sizeof(LinkedList));

	if (*_llist == NULL) {
		return 1;
	}

	(*_llist)->start = newNode;
	newNode->previous = NULL;
	newNode->next = NULL;

	(*_llist)->size = 0;
	(*_llist)->end = (*_llist)->start;

	(*_llist)->elemFuncs.newFunc = NULL;
	(*_llist)->elemFuncs.cpyFunc = NULL;
	(*_llist)->elemFuncs.cmpFunc = NULL;
	(*_llist)->elemFuncs.freeFunc = NULL;

	return 0;
}

int llist_copy(LinkedList *newList, LinkedList *oldList)
{
	LinkedListNode *auxNode;
	int listSize = oldList->size;
	int i;

	llist_setElemFuncs(
	    newList,
	    oldList->elemFuncs.newFunc,
	    oldList->elemFuncs.cpyFunc,
	    oldList->elemFuncs.cmpFunc,
	    oldList->elemFuncs.freeFunc
	);

	auxNode = oldList->start;
	for (i = 0; i < listSize; i++) {
		auxNode = auxNode->next;
		llist_append(newList, auxNode->elem);

	}
	return 0;
}



void llist_destroy(LinkedList *llist)
{

	LinkedListNode *auxNode, *nextNode;
	int listSize = llist->size;
	int i;
	auxNode = llist->start;
	nextNode = auxNode;
	for (i = 0; (i < listSize) && (nextNode != NULL); i++) {
		auxNode = nextNode;
		nextNode = auxNode->next;
		// The first element of the list is always empty (elem = no memory allocation)
		if (i != 0) {
			llist->elemFuncs.freeFunc(auxNode->elem);
		}
		free(auxNode);
	}
	free(llist);
}


int llist_size(LinkedList *llist)
{
	return llist->size;
}

int llist_append(LinkedList *llist, void *elem)
{

	if (llist->elemFuncs.newFunc == NULL || llist->elemFuncs.cpyFunc == NULL) {
		return -1;
	}

	LinkedListNode *newNode = _newNode();
	LinkedListNode *auxNode;

	// Catch the last node
	auxNode = llist->end;
	// The new node will be appended to the list, set the previous
	// pointer of the newNode to the auxNode
	newNode->previous = auxNode;
	// Set the next pointer of the newNode to NULL as it is now
	// the last node
	newNode->next = NULL;
	// Set the element to the newNode
	llist->elemFuncs.newFunc(&newNode->elem);
	llist->elemFuncs.cpyFunc(newNode->elem, elem);

	llist->end->next = newNode;
	llist->end = llist->end->next;

	// Increment the size of the list
	llist->size++;

	return 0;
}

void* llist_getElementByIndex(LinkedList *llist, int index)
{
	LinkedListNode *auxNode = _getNodeByIndex(llist, index);

	if (auxNode != NULL) {
		return auxNode->elem;
	}

	return NULL;
}

void* llist_getFirstOcurrenceOfElement(LinkedList *llist, int (*cmpFunc)(void*))
{

	LinkedListNode *auxNode;
	int listSize;
	auxNode = llist->start;
	listSize = llist->size;
	void *elem;
	int i;

	for (i = 0; i < listSize; i++) {
		auxNode = auxNode->next;
		elem = auxNode->elem;
		if (cmpFunc(elem) == 0) {
			return elem;
		}
	}
	return NULL;
}

int llist_deleteElementByIndex(LinkedList *llist, int index)
{
	LinkedListNode *nodeToDelete = _getNodeByIndex(llist, index);
	LinkedListNode *prevNode, *nextNode;

	if (nodeToDelete == NULL)
	{
		return -1;
	}

	prevNode = nodeToDelete->previous;
	nextNode = nodeToDelete->next;

	prevNode->next = nextNode;
	nextNode->previous = prevNode;

	llist->elemFuncs.freeFunc(nodeToDelete->elem);

	free(nodeToDelete);

	llist->size--;

	return 0;
}

int llist_deleteElement(LinkedList *llist, void *elem)
{
	LinkedListNode *nodeToDelete = _getNodeByElem(llist, elem);
	LinkedListNode *prevNode, *nextNode;

	if (nodeToDelete == NULL)
	{
		return -1;
	}

	prevNode = nodeToDelete->previous;
	nextNode = nodeToDelete->next;

	prevNode->next = nextNode;
	nextNode->previous = prevNode;

	llist->elemFuncs.freeFunc(nodeToDelete->elem);

	free(nodeToDelete);

	llist->size--;

	return 0;
}

void llist_setElemFuncs(
    LinkedList *llist,
    int (*newFunc)(void **),
    void (*cpyFunc)(void *, void *),
    int (*cmpFunc)(void *, void *),
    void (*freeFunc)(void *)
)
{
	llist->elemFuncs.newFunc = newFunc;
	llist->elemFuncs.cpyFunc = cpyFunc;
	llist->elemFuncs.cmpFunc = cmpFunc;
	llist->elemFuncs.freeFunc = freeFunc;
}

int llist_foreach(LinkedList *llist, int (*doFunc)(void*))
{
	LinkedListNode *auxNode;
	int listSize;
	auxNode = llist->start;
	listSize = llist->size;
	void *elem;
	int i;
	int shouldReturn = 0;

	for (i = 0; i < listSize; i++) {
		auxNode = auxNode->next;
		elem = auxNode->elem;
		shouldReturn = doFunc(elem);
		if (shouldReturn != 0) {
			return shouldReturn;
		}
	}
	return 0;
}

int llist_zipWithIndex(LinkedList *llist1, LinkedList *llist2, int (*zipFunc)(void*, void*, int))
{
	LinkedListNode *auxNode1, *auxNode2;
	int listSize1, listSize2, maxSize;
	void *elem1, *elem2;
	int i;
	int shouldReturn = 0;

	auxNode1 = llist1->start;
	listSize1 = llist1->size;
	auxNode2 = llist2->start;
	listSize2 = llist2->size;

	if (listSize1 >= listSize2) maxSize = listSize1;
	if (listSize2 >= listSize1) maxSize = listSize2;

	for (i = 0; i < maxSize; i++) {

		if (i < listSize1) {
			auxNode1 = auxNode1->next;
			elem1 = auxNode1->elem;
		} else {
			elem1 = NULL;
		}
		if (i < listSize2) {
			auxNode2 = auxNode2->next;
			elem2 = auxNode2->elem;
		} else {
			elem2 = NULL;
		}
		shouldReturn = zipFunc(elem1, elem2, i);

		if (shouldReturn != 0) {
			return shouldReturn;
		}
	}
	return 0;
}


static LinkedListNode* _newNode()
{
	return (LinkedListNode *)malloc(sizeof(LinkedListNode));
}

static LinkedListNode* _getNodeByIndex(LinkedList *llist, int index)
{
	LinkedListNode *auxNode;
	int listSize = llist->size;
	int i;
	auxNode = llist->start;
	for (i = 0; i < listSize; i++) {
		auxNode = auxNode->next;
		if (i == index) {
			return auxNode;
		}
	}

	return NULL;
}

static LinkedListNode* _getNodeByElem(LinkedList *llist, void *elem)
{
	LinkedListNode *auxNode;
	int listSize = llist->size;
	int i;
	auxNode = llist->start;
	for (i = 0; i < listSize; i++) {
		auxNode = auxNode->next;
		if (auxNode->elem == elem) {
			return auxNode;
		}
	}

	return NULL;
}

