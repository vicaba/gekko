/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __CSHARE_H__
#define __CSHARE_H__

#include <pthread.h>

#include "../Share/Share.h"

typedef struct {
	pthread_mutex_t mutex;
	Share *share;
} cShare;

// Implemented
int cshare_new(void **cshare);

// Not implemented
void cshare_cpy(void *to, void *from);

// Not implemented
int cshare_cmp(void *this, void *to);

// Implemented
void cshare_destroy(void *cshare);

// Implemented
int cshare_setTicker(cShare *cshare, char* ticker);

// Implemented
int cshare_setValue(cShare *cshare, int value);

// Implemented
char* cshare_getTicker(cShare *cshare);

// Implemented
int cshare_getValue(cShare *cshare);

#endif
