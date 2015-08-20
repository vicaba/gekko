/**
 * @author: Victor Caballero (vicaba)
 * @brief Helper structure to hold the socket/fd and its status
 */
#ifndef __STAT_SOCK_H__
#define __STAT_SOCK_H__

#define SOCK_DISCONNECTED	0x00
#define	SOCK_CONNECTED	0x01

typedef struct stat_sock_t {
	int fd;
	char status;
} stat_sock_t;

#endif
