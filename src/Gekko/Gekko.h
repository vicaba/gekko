/**
 * @author: Victor Caballero (vicaba)
 */
#ifndef __GEKKO_H__
#define __GEKKO_H__

#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "../util/LinkedList/LinkedList.h"
#include "../util/cLinkedList/cLinkedList.h"
#include "../IbexValue/IbexValue.h"

typedef struct {
	int refreshPeriod;
	cLinkedList *ibexValues;
	struct sockaddr_in *TDaddr;
	struct sockaddr_in *serveraddr;
} Gekko;

// Implemented
int gek_new(void **gekko);

// Not implemented
void gek_cpy(void *to, void *from);

// Not implemented
int gek_cmp(void *this, void *to);

// Implemented
void gek_destroy(void *gekko);

// Implemented
int gek_setRefreshPeriod(Gekko *gekko, int refreshPeriod);

// Implemented
int gek_setIbexValues(Gekko *gekko, cLinkedList *shares);

// Not Implemented
int gek_setTDConfig(Gekko *gekko, struct sockaddr_in *serverConfig);

// Implemented
int gek_getRefreshPeriod(Gekko *gekko);

// Implemented
cLinkedList* gek_getIbexValues(Gekko *gekko);

// Implemented
struct sockaddr_in* gek_getTDConfig(Gekko *gekko);

// Implemented
struct sockaddr_in* gek_getServerConfig(Gekko *gekko);

#endif
