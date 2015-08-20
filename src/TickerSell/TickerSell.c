/**
 * @author: Victor Caballero (vicaba)
 */
#include "./TickerSell.h"

int ts_new(void **ts);

void ts_cpy(void *to, void *from);

int ts_cmp(void *this, void *to);

void ts_destroy(void *ts);

int ts_setTicker(TickerSell *ts, char* ticker);

char* ts_getTicker(TickerSell *ts);

cLinkedList* ts_getDozerList(TickerSell *ts);

int ts_new(void **ts)
{
	TickerSell **_ts = (TickerSell **)ts;

	*_ts = (TickerSell *)malloc(sizeof(TickerSell) * 1);

	(*_ts)->ticker = (char *)malloc(sizeof(char *));
	cllist_new((void **) & ((*_ts)->dozerList));
	cllist_setElemFuncs(
	    (*_ts)->dozerList,
	    share_new,
	    share_cpy,
	    NULL,
	    share_destroy
	);

	return 0;
}

void ts_cpy(void *to, void *from)
{
	TickerSell *_to = (TickerSell *)to;
	TickerSell *_from = (TickerSell *)from;

	ts_setTicker(_to, ts_getTicker(from));
	cllist_copy(_to->dozerList, _from->dozerList);
}

void ts_destroy(void *ts)
{
	TickerSell *_ts = (TickerSell *)ts;
	free(_ts->ticker);
	cllist_destroy(_ts->dozerList);
	free(_ts);
}

int ts_setTicker(TickerSell* ts, char *ticker)
{
	free(ts->ticker);
	ts->ticker = strdup(ticker);
	return 0;
}


char* ts_getTicker(TickerSell *ts)
{
	return ts->ticker;
}

cLinkedList* ts_getDozerList(TickerSell *ts)
{
	return ts->dozerList;
}


