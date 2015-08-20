/**
 * @author: Victor Caballero (vicaba)
 */
#include "./cInteger.h"

void cint_new(void **cint);

void cint_copy(void *newCint, void *oldCint);

void cint_destroy(void *cint);

void cint_set(cInteger *cint, int val);

int cint_get(cInteger *cint);

void cint_new(void **cint)
{
	cInteger **_cint = (cInteger **)cint;
	*_cint = (cInteger *)malloc(sizeof(cInteger));

	pthread_mutex_init(&((*_cint)->mutex), NULL);

}

void cint_copy(void *newCint, void *oldCint)
{
	cInteger **_newCint = (cInteger **)newCint;
	cInteger **_oldCint = (cInteger **)oldCint;

	pthread_mutex_lock(&((*_newCint)->mutex));
	pthread_mutex_lock(&((*_oldCint)->mutex));

	(*_newCint)->value = (*_oldCint)->value;

	pthread_mutex_unlock(&((*_newCint)->mutex));
	pthread_mutex_unlock(&((*_oldCint)->mutex));

}

void cint_destroy(void *cint)
{
	cInteger *_cint = (cInteger *)cint;

	pthread_mutex_destroy(&(_cint->mutex));

}

void cint_set(cInteger *cint, int val)
{

	pthread_mutex_lock(&(cint->mutex));

	cint->value = val;

	pthread_mutex_unlock(&(cint->mutex));

}

int cint_get(cInteger *cint)
{
	int returnVal;

	pthread_mutex_lock(&(cint->mutex));

	returnVal = cint->value;

	pthread_mutex_unlock(&(cint->mutex));

	return returnVal;
}

