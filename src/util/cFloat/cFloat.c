/**
 * @author: Victor Caballero (vicaba)
 */
#include "./cFloat.h"

void cfloat_new(void **cfloat);

void cfloat_copy(void *to, void *from);

void cfloat_destroy(void *cfloat);

void cfloat_set(cFloat *cfloat, float val);

float cfloat_get(cFloat *cfloat);

void cfloat_new(void **cfloat)
{
	cFloat **_cfloat = (cFloat **)cfloat;
	*_cfloat = (cFloat *)malloc(sizeof(cFloat));

	pthread_mutex_init(&((*_cfloat)->mutex), NULL);
}

void cfloat_copy(void *to, void *from)
{
	cFloat **_to = (cFloat **)to;
	cFloat **_from = (cFloat **)from;

	pthread_mutex_lock(&((*_to)->mutex));
	pthread_mutex_lock(&((*_from)->mutex));

	(*_to)->value = (*_from)->value;

	pthread_mutex_unlock(&((*_to)->mutex));
	pthread_mutex_unlock(&((*_from)->mutex));

}

void cfloat_destroy(void *cfloat)
{
	cFloat *_cfloat = (cFloat *)cfloat;

	pthread_mutex_destroy(&(_cfloat->mutex));
}

void cfloat_set(cFloat *cfloat, float val)
{
	pthread_mutex_lock(&(cfloat->mutex));

	cfloat->value = val;

	pthread_mutex_unlock(&(cfloat->mutex));
}

float cfloat_get(cFloat *cfloat)
{
	float returnVal;

	pthread_mutex_lock(&(cfloat->mutex));

	returnVal = cfloat->value;

	pthread_mutex_unlock(&(cfloat->mutex));

	return returnVal;
}

