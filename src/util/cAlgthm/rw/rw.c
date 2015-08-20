/**
 * @author: Victor Caballero (vicaba)
 */
#include "rw.h"

void* rwa_reader_lock(rw_mutex_t *rw_mutex, void* arg, void* (*callback)(void*));

void* rwa_writer_lock(rw_mutex_t *rw_mutex, void *arg, void* (*callback)(void*));

int rw_mutex_init(rw_mutex_t *rw_mutex);

int rw_mutex_init(rw_mutex_t *rw_mutex)
{
	rw_mutex->nr = 0;
	rw_mutex->nw = 0;
	pthread_mutex_init(&(rw_mutex->readers), NULL);
	pthread_mutex_init(&(rw_mutex->writers), NULL);
	pthread_mutex_init(&(rw_mutex->mutex_r), NULL);
	pthread_mutex_init(&(rw_mutex->mutex_w), NULL);
	return 0;
}

void* reader_lock(rw_mutex_t *rw_mutex, void *arg, void* (*callback)(void*))
{
	void *res;

	pthread_mutex_lock(&(rw_mutex->readers));
	pthread_mutex_lock(&(rw_mutex->mutex_r));
	rw_mutex->nr = rw_mutex->nr + 1;
	if (rw_mutex->nr == 1) {
		pthread_mutex_lock(&(rw_mutex->writers));
	}
	pthread_mutex_unlock(&(rw_mutex->mutex_r));
	pthread_mutex_unlock(&(rw_mutex->readers));

	res = callback(arg);

	pthread_mutex_lock(&(rw_mutex->mutex_r));
	rw_mutex->nr = rw_mutex->nr - 1;
	if (rw_mutex->nr == 0) {
		pthread_mutex_unlock(&(rw_mutex->writers));
	}
	pthread_mutex_unlock(&(rw_mutex->mutex_r));
	return res;
}

void* writer_lock(rw_mutex_t *rw_mutex, void *arg, void* (*callback)(void*))
{
	void *res;

	pthread_mutex_lock(&(rw_mutex->mutex_w));
	rw_mutex->nw = rw_mutex->nw + 1;
	if (rw_mutex->nw == 1) {
		pthread_mutex_lock(&(rw_mutex->readers));
	}
	pthread_mutex_unlock(&(rw_mutex->mutex_w));

	pthread_mutex_lock(&(rw_mutex->writers));
	res = callback(arg);
	pthread_mutex_unlock(&(rw_mutex->writers));

	pthread_mutex_lock(&(rw_mutex->mutex_w));
	rw_mutex->nw = rw_mutex->nw - 1;
	if (rw_mutex->nw == 0) {
		pthread_mutex_unlock(&(rw_mutex->readers));
	}
	pthread_mutex_unlock(&(rw_mutex->mutex_w));
	return res;
}
