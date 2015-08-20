/**
 * @author: Victor Caballero (vicaba)
 * @brief A thread-safe implementation of a float
 */
#ifndef __CFLOAT_H__
#define __CFLOAT_H__

/**
 * @author: Victor Caballero
 * @brief thread safe float implementation
 */
#include <pthread.h>
#include <stdlib.h>

typedef struct cFloat {
	pthread_mutex_t mutex;
	float value;
} cFloat;

/**
 * Creates a float
 * @param cfloat A pointer to a cFloat structure
 */
void cfloat_new(void **cfloat);

/**
 * Copy floats
 * @param to
 * @param from
 */
void cfloat_copy(void *to, void *from);

/**
 * Frees memory
 * @param cfloat the pointer to the cFloat structure
 */
void cfloat_destroy(void *cfloat);

/**
 * Sets a value
 * @param cfloat
 * @param val the value
 */
void cfloat_set(cFloat *cfloat, float val);

/**
 * Gets the value
 * @param cfloat
 * @return the value
 */
float cfloat_get(cFloat *cfloat);

#endif
