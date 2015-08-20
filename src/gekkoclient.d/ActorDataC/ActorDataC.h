/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __ACTORDATAC_H__
#define __ACTORDATAC_H__

#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

#include <pthread.h>

#include "../../util/thread/thread.h"
#include "../../util/fdutil/fdutil.h"

typedef struct ActorDataC {
	char *actorName;
	stat_thread_t thread;
	int pipe[2];
	int sock;
	int epoll;
} ActorDataC;

/**
 * 
 * @param  adata [description]
 * @return       [description]
 */
int adatac_new(void **adata);

void adatac_cpy(void *to, void *from);

int adatac_cmp(void *this, void *to);

/**
 * Set file descriptor variables to -10 if they are closed! (pipe, sock, epoll)
 */
void adatac_destroy(void *adata);

int adatac_setActorName(ActorDataC *adata, char *actorName);

char* adatac_getActorName(ActorDataC *adata);

stat_thread_t* adatac_getPthread(ActorDataC *adata);

int* adatac_getPipe(ActorDataC *adata);

int* adatac_getSocket(ActorDataC *adata);

int* adatac_getEpoll(ActorDataC *adata);

#endif
