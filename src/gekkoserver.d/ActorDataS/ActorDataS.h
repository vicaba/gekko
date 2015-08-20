/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __ACTORDATAS_H__
#define __ACTORDATAS_H__

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>

#include "../../util/thread/thread.h"
#include "../../util/socket/socket.h"
#include "../../util/fdutil/fdutil.h"


typedef struct ActorDataS {
	char *actorName;
	stat_thread_t thread;
	int pipe[2];
	stat_sock_t sock;
	int epoll;
} ActorDataS;

/**
 * Create an actor data.
 * @param  adata [description]
 * @return       [description]
 */
int adatas_new(void **adata);

void adatas_cpy(void *to, void *from);

int adatas_cmp(void *this, void *to);

/**
 * Destroys actor data. Beware that if the file descriptors are valid, they will be closed
 * @param adata the actor data
 */
void adatas_destroy(void *adata);

int adatas_setActorName(ActorDataS *adata, char *actorName);

char* adatas_getActorName(ActorDataS *adata);

stat_thread_t* adatas_getPthread(ActorDataS *adata);

int* adatas_getPipe(ActorDataS *adata);

stat_sock_t* adatas_getSocket(ActorDataS *adata);

int* adatas_getEpoll(ActorDataS *adata);

#endif
