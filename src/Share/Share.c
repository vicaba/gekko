/**
 * @author: Victor Caballero (vicaba)
 */
#include "Share.h"

int share_new(void **share);

void share_cpy(void *to, void *from);

int share_cmp(void *this, void *to);

void share_destroy(void *share);

int share_setTicker(Share *share, char* ticker);

int share_setValue(Share *share, int value);

char* share_getTicker(Share *share);

int share_getValue(Share *share);


int share_new(void **share)
{
	Share **_share = (Share **)share;

	*_share = (Share *)malloc(sizeof(Share) * 1);

	(*_share)->ticker = (char *)malloc(sizeof(char *));
	(*_share)->value = 0;

	return 0;
}

void share_cpy(void *to, void *from)
{
	Share *_to = (Share *)to;
	Share *_from = (Share *)from;

	share_setTicker(_to, share_getTicker(_from));
	share_setValue(_to, share_getValue(_from));

}


void share_destroy(void *share)
{
	Share *_share = (Share *)share;
	free(_share->ticker);
	free(_share);
}

int share_setTicker(Share* share, char *ticker)
{
	free(share->ticker);
	share->ticker = strdup(ticker);
	return 0;
}

int share_setValue(Share *share, int value)
{
	share->value = value;
	return 0;
}

char* share_getTicker(Share *share)
{
	return share->ticker;
}

int share_getValue(Share *share)
{
	return share->value;
}
