/**
 * @author: Victor Caballero (vicaba)
 */
#include "Gekko.h"

int gek_new(void **gekko);

void gek_cpy(void *to, void *from);

int gek_cmp(void *this, void *to);

void gek_destroy(void *gekko);

int gek_setRefreshPeriod(Gekko *gekko, int refreshPeriod);

int gek_getRefreshPeriod(Gekko *gekko);

int gek_setIbexValues(Gekko *gekko, cLinkedList *ibexValues);

cLinkedList* gek_getIbexValues(Gekko *gekko);

struct sockaddr_in* gek_getTDConfig(Gekko *gekko);

struct sockaddr_in* gek_getServerConfig(Gekko *gekko);

int gek_new(void **gekko)
{

	Gekko **_gekko = (Gekko **)gekko;

	*_gekko = (Gekko *)malloc(sizeof(Gekko));

	(*_gekko)->refreshPeriod = 0;
	cllist_new((void **) & ((*_gekko)->ibexValues));
	cllist_setElemFuncs(
	    (*_gekko)->ibexValues,
	    ibval_new,
	    ibval_cpy,
	    NULL,
	    ibval_destroy
	);
	(*_gekko)->TDaddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	bzero((char *) (*_gekko)->TDaddr, sizeof(struct sockaddr_in));

	(*_gekko)->serveraddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	bzero((char *) (*_gekko)->serveraddr, sizeof(struct sockaddr_in));

	return 0;
}

void gek_destroy(void *gekko)
{
	Gekko *_gekko = (Gekko *)gekko;

	cllist_destroy(_gekko->ibexValues);
	free(_gekko->TDaddr);
	free(_gekko);

}

int gek_setRefreshPeriod(Gekko *gekko, int refreshPeriod)
{
	gekko->refreshPeriod = refreshPeriod;
	return 0;
}

int gek_setIbexValues(Gekko *gekko, cLinkedList *ibexValues)
{
	gekko->ibexValues = ibexValues;
	return 0;
}

int gek_getRefreshPeriod(Gekko *gekko)
{
	return gekko->refreshPeriod;
}

cLinkedList* gek_getIbexValues(Gekko *gekko)
{
	return gekko->ibexValues;
}

struct sockaddr_in* gek_getTDConfig(Gekko *gekko)
{
	return gekko->TDaddr;
}

struct sockaddr_in* gek_getServerConfig(Gekko *gekko)
{
	return gekko->serveraddr;
}


