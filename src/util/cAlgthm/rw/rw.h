/**
 * @author: Victor Caballero (vicaba)
 * @brief This library provides a read-writers lock. A read callback can be executed
 * at the same time by many threads. A write callback can only be executed
 * by one thread at a time with no readers executing a read callback.
 *
 */

#ifndef __RW_ALGTHM_H__
#define __RW_ALGTHM_H__

#include <pthread.h>

typedef struct rw_mutex_t {
	pthread_mutex_t mutex_r;
	pthread_mutex_t mutex_w;
	pthread_mutex_t readers;
	pthread_mutex_t writers;
	int nr;
	int nw;
} rw_mutex_t;

/**
 * Initializes a rw mutex
 * @param  rw_mutex the mutex
 * @return        	0
 */
int rw_mutex_init(rw_mutex_t *rw_mutex);


/**
 * Executes a callback in a read mode
 * @param arg		the argument of the callback
 * @param callback	a callback to execute in a read mode
 * @return 			the callback result
 */
void* rwa_reader_lock(rw_mutex_t *rw_mutex, void* arg, void* (*callback)(void*));

/**
 * Executes a callback in a write mode
 * @param arg		the argument of the callback
 * @param callback	a callback to execute in a write mode
 * @return 			the callback result
 */
void* rwa_writer_lock(rw_mutex_t *rw_mutex, void *arg, void* (*callback)(void*));

#endif
