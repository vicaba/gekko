/**
 * @author: Victor Caballero (vicaba)
 */
#include "ActorDataS.h"

int adatas_new(void **adata);

void adatas_cpy(void *to, void *from);

int adatas_cmp(void *this, void *to);

void adatas_destroy(void *adata);

int adatas_setActorName(ActorDataS *adata, char *actorName);

char* adatas_getActorName(ActorDataS *adata);

stat_thread_t* adatas_getPthreadId(ActorDataS *adata);

int* adatas_getPipe(ActorDataS *adata);

stat_sock_t* adatas_getSocket(ActorDataS *adata);

int* adatas_getEpoll(ActorDataS *adata);

int adatas_new(void **adata)
{
	ActorDataS **_adata = (ActorDataS **)adata;

	*_adata = (ActorDataS *)malloc(sizeof(ActorDataS));

	(*_adata)->actorName = (char *)malloc(sizeof(char));
	(*_adata)->pipe[0] = -10;
	(*_adata)->pipe[1] = -10;
	(*_adata)->sock.fd = -10;
	(*_adata)->sock.status = SOCK_DISCONNECTED;
	(*_adata)->epoll = -10;
	(*_adata)->thread.id = 0;
	(*_adata)->thread.status = THREAD_NOT_RUNNING;

	return 0;
}

void adatas_cpy(void *to, void *from)
{
	ActorDataS *_to = (ActorDataS *)to;
	ActorDataS *_from = (ActorDataS *)from;

	adatas_setActorName(_to, adatas_getActorName(_from));
	_to->pipe[0] = _from->pipe[0];
	_to->pipe[1] = _from->pipe[1];
	_to->sock.fd = _from->sock.fd;
	_to->epoll = _from->epoll;
	_to->thread.id = _from->thread.id;
	_to->thread.status = _from->thread.status;
}

void adatas_destroy(void *adata)
{
	ActorDataS *_adata = (ActorDataS *)adata;

	if (_adata->thread.status == THREAD_RUNNING) {
		int s;
		void *res;
		s = pthread_join(_adata->thread.id, &res);
		_adata->thread.status = THREAD_NOT_RUNNING;
	}
	free(_adata->actorName);
	if (fdut_fdIsValid(_adata->pipe[0])) close(_adata->pipe[0]);
	if (fdut_fdIsValid(_adata->pipe[1])) close(_adata->pipe[1]);
	if (fdut_fdIsValid(_adata->sock.fd)) close(_adata->sock.fd);
	if (fdut_fdIsValid(_adata->epoll)) close(_adata->epoll);
	free(_adata);
}

int adatas_setActorName(ActorDataS *adata, char *actorName)
{
	free(adata->actorName);
	adata->actorName = strdup(actorName);
	return 0;
}

char* adatas_getActorName(ActorDataS *adata)
{
	return adata->actorName;
}

stat_thread_t* adatas_getPthread(ActorDataS *adata)
{
	return &(adata->thread);
}

int* adatas_getPipe(ActorDataS *adata)
{
	return adata->pipe;
}

stat_sock_t* adatas_getSocket(ActorDataS *adata)
{
	return &(adata->sock);
}

int* adatas_getEpoll(ActorDataS *adata)
{
	return &(adata->epoll);
}

