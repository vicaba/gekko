/**
 * @author: Victor Caballero (vicaba)
 */
#include "./cShare.h"

int cshare_new(void **cshare);

void cshare_cpy(void *to, void *from);

int cshare_cmp(void *this, void *to);

void cshare_destroy(void *cshare);

int cshare_setTicker(cShare *cshare, char* ticker);

int cshare_setValue(cShare *cshare, int value);

char* cshare_getTicker(cShare *cshare);

int cshare_getValue(cShare *cshare);


int cshare_new(void **cshare)
{
	cShare **_cshare = (cShare **)cshare;

	*_cshare = (cShare *)malloc(sizeof(cShare) * 1);
	share_new((void **) & (*_cshare)->share);

	pthread_mutex_init(&((*_cshare)->mutex), NULL);

	return 0;
}

void cshare_cpy(void *to, void *from)
{
	cShare *_to = (cShare *)to;
	cShare *_from = (cShare *)from;

	pthread_mutex_lock(&(_to->mutex));
	pthread_mutex_lock(&(_from->mutex));

	share_cpy(_to->share, _from->share);

	pthread_mutex_unlock(&(_to->mutex));
	pthread_mutex_unlock(&(_from->mutex));

}


void cshare_destroy(void *cshare)
{
	cShare *_cshare = (cShare *)cshare;

	pthread_mutex_lock(&(_cshare->mutex));

	share_destroy(_cshare->share);

	pthread_mutex_unlock(&(_cshare->mutex));

	pthread_mutex_destroy(&(_cshare->mutex));

}

int cshare_setTicker(cShare* cshare, char *ticker)
{
	int returnVal;
	pthread_mutex_lock(&(cshare->mutex));

	returnVal = share_setTicker(cshare->share, ticker);

	pthread_mutex_unlock(&(cshare->mutex));

	return returnVal;
}

int cshare_setValue(cShare *cshare, int value)
{

	int returnVal;
	
	pthread_mutex_lock(&(cshare->mutex));

	returnVal = share_setValue(cshare->share, value);

	pthread_mutex_unlock(&(cshare->mutex));

	return returnVal;
}

char* cshare_getTicker(cShare *cshare)
{
	char *returnVal;

	pthread_mutex_lock(&(cshare->mutex));

	returnVal = share_getTicker(cshare->share);

	pthread_mutex_unlock(&(cshare->mutex));

	return returnVal;
}

int cshare_getValue(cShare *cshare)
{
	int returnVal;

	pthread_mutex_lock(&(cshare->mutex));

	returnVal = share_getValue(cshare->share);

	pthread_mutex_unlock(&(cshare->mutex));

	return returnVal;
}
