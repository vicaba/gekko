/**
 * @author: Victor Caballero (vicaba)
 * @brief A thread-safe implementation of an integer
 */
#ifndef __CINTEGER_H__
#define __CINTEGER_H__

/**
 * @author: Victor Caballero
 * @brief thread safe integer implementation
 */
#include <pthread.h>
#include <stdlib.h>

typedef struct cInteger {
	pthread_mutex_t mutex;
	int value;
} cInteger;

/**
 * Creates an integer
 * @param cint A pointer to a cFloat structure
 */
void cint_new(void **cint);

/**
 * Copy integers
 * @param to
 * @param from
 */
void cint_copy(void *newCint, void *oldCint);

/**
 * Frees memory
 * @param cint the pointer to the cInteger structure
 */
void cint_destroy(void *cint);

/**
 * Sets a value
 * @param cint
 * @param val the value
 */
void cint_set(cInteger *cint, int val);

/**
 * Gets the integer
 * @param cint
 * @return the value
 */
int cint_get(cInteger *cint);

#endif
