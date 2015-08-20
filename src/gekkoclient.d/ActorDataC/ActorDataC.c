/**
 * @author: Victor Caballero (vicaba)
 */
#include "ActorDataC.h"

int adatac_new(void **adata);

void adatac_cpy(void *to, void *from);

int adatac_cmp(void *this, void *to);

void adatac_destroy(void *adata);

int adatac_setActorName(ActorDataC *adata, char *actorName);

char* adatac_getActorName(ActorDataC *adata);

stat_thread_t* adatac_getPthread(ActorDataC *adata);

int* adatac_getPipe(ActorDataC *adata);

int* adatac_getSocket(ActorDataC *adata);

int* adatac_getEpoll(ActorDataC *adata);

int adatac_new(void **adata)
{
	ActorDataC **_adata = (ActorDataC **)adata;

	*_adata = (ActorDataC *)malloc(sizeof(ActorDataC));

	(*_adata)->actorName = (char *)malloc(sizeof(char));
	(*_adata)->pipe[0] = -10;
	(*_adata)->pipe[1] = -10;
	(*_adata)->sock = -10;
	(*_adata)->epoll = -10;
	(*_adata)->thread.status = THREAD_NOT_RUNNING;

	return 0;
}

void adatac_cpy(void *to, void *from)
{
	ActorDataC *_to = (ActorDataC *)to;
	ActorDataC *_from = (ActorDataC *)from;

	adatac_setActorName(_to, adatac_getActorName(_from));
	_to->pipe[0] = _from->pipe[0];
	_to->pipe[1] = _from->pipe[1];
	_to->sock = _from->sock;
	_to->epoll = _from->epoll;
	_to->thread.id = _from->thread.id;
	_to->thread.status = _from->thread.status;
}

void adatac_destroy(void *adata)
{
	ActorDataC *_adata = (ActorDataC *)adata;
	if (_adata->thread.status == THREAD_RUNNING) {
		int s;
		void *res;
		s = pthread_join(_adata->thread.id, &res);
		_adata->thread.status = THREAD_NOT_RUNNING;
	}
	free(_adata->actorName);
	if (fdut_fdIsValid(_adata->pipe[0])) close(_adata->pipe[0]);
	if (fdut_fdIsValid(_adata->pipe[1])) close(_adata->pipe[1]);
	if (fdut_fdIsValid(_adata->sock)) close(_adata->sock);
	if (fdut_fdIsValid(_adata->epoll)) close(_adata->epoll);
	free(_adata);
}

int adatac_setActorName(ActorDataC *adata, char *actorName)
{
	free(adata->actorName);
	adata->actorName = strdup(actorName);
	return 0;
}

char* adatac_getActorName(ActorDataC *adata)
{
	return adata->actorName;
}

stat_thread_t* adatac_getPthread(ActorDataC *adata)
{
	return &(adata->thread);
}

int* adatac_getPipe(ActorDataC *adata)
{
	return adata->pipe;
}

int* adatac_getSocket(ActorDataC *adata)
{
	return &(adata->sock);
}

int* adatac_getEpoll(ActorDataC *adata)
{
	return &(adata->epoll);
}

