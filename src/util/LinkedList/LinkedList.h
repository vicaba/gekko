/**
 * @author: Victor Caballero (vicaba)
 * @brief A Linked List implementation
 */
#ifndef __LINKEDLIST_H__
#define __LINKEDLIST_H__

#include <stdlib.h>
#include <stdio.h>

typedef struct inElemFuncs {
	int (*newFunc)(void **);
	void (*cpyFunc)(void *, void *);
	int (*cmpFunc)(void *, void *);
	void (*freeFunc)(void *);
} inElemFuncs;

/* 	________	
 	|		|
 	|elem*	|
 	|_______|	
 	|next*	|
 	|_______|
 	|prev*	|
 	|_______|*/
typedef struct LinkedListNode {
	void *elem;
	struct LinkedListNode *next;
	struct LinkedListNode *previous;
} LinkedListNode;
/* 	________		
 	|start*	|
 	|_______|
 	|end*	|
 	|_______|*/
typedef struct LinkedList {
	inElemFuncs elemFuncs;
	LinkedListNode *start;
	LinkedListNode *end;
	LinkedListNode *current;	// Not used
	int size;
} LinkedList;

/**
 * Creates a linked list
 * @param  llist the list
 * @return
 */
int llist_new(void **llist);

/**
 * Copies oldList to newList
 * @param  newLlist the new list to copy the elements to
 * @param  oldLlist the old list to copy the elements from
 * @return
 */
int llist_copy(LinkedList *newLlist, LinkedList *oldLlist);

/**
 * Destroys the llist
 * @param  llist the list
 * @return
 */
void llist_destroy(LinkedList *llist);

/**
 * Returns the size of the list
 * @param  llist the list
 * @return
 */
int llist_size(LinkedList *llist);

/**
 * Appends an element to the list
 * @param  llist the list
 * @param  elem  the element
 * @return
 */
int llist_append(LinkedList *llist, void *elem);

/**
 * NOT IMPLEMENTED. Prepends an element to the list
 * @param  llist the list
 * @param  elem  the element
 * @return
 */
int llist_prepend(LinkedList *llist, void *elem);

/**
 * Gets an element by its index on the list
 * @param 	llist the list
 * @param 	index the index
 * @return 	the element or null if the index is out of bounds
 */
void* llist_getElementByIndex(LinkedList *llist, int index);

/**
 * Returns the first occurrence of an element satisfying the condition of the cmpFunc
 * @param	llist the list
 * @param 	cmpFunc the function that compares every element in the list.
 *                 	This function must return 0 if any match is made, -1 otherwise
 * @return 	the element matched or NULL if no element was matched
 */
void* llist_getFirstOcurrenceOfElement(LinkedList *llist, int (*cmpFunc)(void*));

/**
 * Deletes the element on a given index
 * @param llist the list
 * @param index the index
 */
int llist_deleteElementByIndex(LinkedList *llist, int index);

/**
 * Deletes the element elem in the list
 * @param  llist the list
 * @param  elem  the element
 * @return       0 if successful, non-zero otherwise
 */
int llist_deleteElement(LinkedList *llist, void *elem);

/**
 * This function is used to give a template to the list so it is able to perform basic operations
 * with the elements that it contains
 * @param llist 	the list
 * @param newFunc 	function responsible for creating an element
 * @param cpyFunc	function responsible for copying an element
 * @param cmpFunc	DEPRECATED, set to null. function responsible for comparing an element
 * @param freeFunc	function responsible for destroying an element
 */
void llist_setElemFuncs(
    LinkedList *llist,
    int (*newFunc)(void **),
    void (*cpyFunc)(void *, void *),
    int (*cmpFunc)(void *, void *),
    void (*freeFunc)(void *)
);

/**
 * NOT IMPLEMENTED
 */
int llist_toArray(LinkedList *llist);

/**
 * Iterates through the list injecting the elements in the doFunc
 * @param	llist the list
 * @param 	doFunc execute doFunc for each iteration, returning a non-zero value stops foreach iteration
 * @return 	nothing
 */
int llist_foreach(LinkedList *llist, int (*doFunc)(void*));

/**
 * Iterates through both lists injecting the elements and the index in the zipFunc
 * @param	llist1 the list
 * @param	llist2 the list
 * @param 	zipFunc execute doFunc for each iteration, returning a non-zero value stops zip iteration
 * @return 	nothing
 */
int llist_zipWithIndex(LinkedList *llist1, LinkedList *llist2, int (*zipFunc)(void*, void*, int));


#endif
