/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __TICKERSELL_H__
#define __TICKERSELL_H__

#include "../util/cLinkedList/cLinkedList.h"
#include "../Share/Share.h"

typedef struct TickerSell {
	char *ticker;
	cLinkedList *dozerList;
} TickerSell;

// Implemented
int ts_new(void **ts);

// Not implemented
void ts_cpy(void *to, void *from);

// Not implemented
int ts_cmp(void *this, void *to);

// Implemented
void ts_destroy(void *ts);

// Implemented
int ts_setTicker(TickerSell *ts, char* ticker);

// Implemented
char* ts_getTicker(TickerSell *ts);

// Implemented
cLinkedList* ts_getDozerList(TickerSell *ts);

#endif
