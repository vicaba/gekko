/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __DOZER_H__
#define __DOZER_H__

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../util/LinkedList/LinkedList.h"
#include "../util/cLinkedList/cLinkedList.h"
#include "../util/cFloat/cFloat.h"

#include "../cShare/cShare.h"

typedef struct {
	char *name;
	cFloat *money;
	cLinkedList *shares;
	struct sockaddr_in *servaddr;
} Dozer;

// Implemented
int doz_new(void **dozer);

// Not implemented
void doz_cpy(void *to, void *from);

// Not implemented
int doz_cmp(void *this, void *to);

// Implemented
void doz_destroy(void *dozer);

// Implemented
int doz_setName(Dozer *dozer, char *name);

// Implemented
int doz_setServerConfig(Dozer *dozer, struct sockaddr_in *serverConfig);

// Implemented
char* doz_getName(Dozer *dozer);

// Implemented
cFloat* doz_getMoney(Dozer *dozer);

// Implemented
cLinkedList* doz_getShares(Dozer *dozer);

// Implemented
struct sockaddr_in* doz_getServerConfig(Dozer *dozer);

#endif
