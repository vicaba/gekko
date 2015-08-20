/**
 * @author: Victor Caballero (vicaba)
 */
#include "Dozer.h"

int doz_new(void **dozer);

void doz_cpy(void *to, void *from);

int doz_cmp(void *this, void *to);

void doz_destroy(void *dozer);

int doz_setName(Dozer *dozer, char *name);

char* doz_getName(Dozer *dozer);

cFloat* doz_getMoney(Dozer *dozer);

cLinkedList* doz_getShares(Dozer *dozer);

struct sockaddr_in* doz_getServerConfig(Dozer *dozer);

int doz_new(void **dozer)
{

	Dozer **_dozer = (Dozer **)dozer;

	*_dozer = (Dozer *)malloc(sizeof(Dozer));

	(*_dozer)->name = (char *)malloc(sizeof(char));
	cfloat_new((void **)&(*_dozer)->money);
	cllist_new((void **) & ((*_dozer)->shares));
	cllist_setElemFuncs(
	    (*_dozer)->shares,
	    cshare_new,
	    cshare_cpy,
	    NULL,
	    cshare_destroy
	);
	(*_dozer)->servaddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));

	return 0;
}

void doz_destroy(void *dozer)
{
	Dozer *_dozer = (Dozer *)dozer;

	free(_dozer->name);
	cllist_destroy(_dozer->shares);
	cfloat_destroy(_dozer->money);
	free(_dozer->servaddr);
	free(_dozer);
}

int doz_setName(Dozer *dozer, char *name)
{
	free(dozer->name);
	dozer->name = strdup(name);
	return 0;
}

char* doz_getName(Dozer *dozer)
{
	return dozer->name;
}

cFloat* doz_getMoney(Dozer *dozer)
{
	return dozer->money;
}

cLinkedList* doz_getShares(Dozer *dozer)
{
	return dozer->shares;
}

struct sockaddr_in* doz_getServerConfig(Dozer *dozer)
{
	return dozer->servaddr;
}


