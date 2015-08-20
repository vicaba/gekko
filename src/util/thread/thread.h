/**
 * @author: Victor Caballero (vicaba)
 * @brief Helper structure to hold the thread id and its status
 */
#ifndef __STAT_THREAD_H__
#define __STAT_THREAD_H__

#include <pthread.h>

#define THREAD_NOT_RUNNING	0x00
#define	THREAD_RUNNING	0x01

typedef struct stat_thread_t {
	pthread_t id;
	char status;
} stat_thread_t;

#endif
