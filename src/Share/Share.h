/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __SHARE_H__
#define __SHARE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
	char *ticker;
	int value;
} Share;

// Implemented
int share_new(void **share);

// Not implemented
void share_cpy(void *to, void *from);

// Not implemented
int share_cmp(void *this, void *to);

// Implemented
void share_destroy(void *share);

// Implemented
int share_setTicker(Share *share, char* ticker);

// Implemented
int share_setValue(Share *share, int value);

// Implemented
char* share_getTicker(Share *share);

// Implemented
int share_getValue(Share *share);

#endif
