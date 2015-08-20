/**
 * @author: Victor Caballero (vicaba)
 * @brief Utils to avoid using printf() and scanf()
 */

#ifndef __FDUTL_H__
#define __FDUTL_H__

#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>

/**
 * Sends a message to the specified file descriptor (fd)
 * @param  fd  the file descriptor
 * @param  msg the message
 * @return     the bytes writed
 */
ssize_t fdut_msg(int fd, char *msg);

/**
 * Sends a formated message to the specified file descriptor (fd)
 * @param  fd     	the file descriptor
 * @param  buffer 	a buffer to perform the operations
 * @param  format 	the string format
 * @param  ... 		a list of arguments to match with the format
 * @return        	the bytes writed
 */
ssize_t fdut_msgf(int fd, char *buffer, char *format, ...);

/**
 * Sends a formated message to the specified file descriptor (fd)
 * @param  fd     	the file descriptor
 * @param  buffer 	a buffer to perform the operations
 * @param  format 	the string format
 * @param  args 	a va_list of arguments to match with the format
 * @return        	the bytes writed
 */
ssize_t fdut_vmsgf(int fd, char *buffer, char *format, va_list args);

/**
 * Sends a message to the default stdout
 * @param  msg the message
 * @return     the bytes writed
 */
ssize_t fdut_msgStdo(char *msg);

/**
 * Sends a formated message to the default stdout
 * @param  fd     	the file descriptor
 * @param  buffer 	a buffer to perform the operations
 * @param  format 	the string format
 * @param  ... 		a list of arguments to match ith the format
 * @return        	the bytes writed
 */
ssize_t fdut_msgStdof(char *buffer, char *format, ...);


/**
 * Gets a string from the specified file descriptor (fd).
 * The buffer has no limits until the CPU explodes (Core Dumped).
 * @param  fd  the file descriptor
 * @param  str the buffer, it must be allocated first
 * @return     the bytes read
 */
ssize_t fdut_gets(int fd, char **str);

/**
 * Gets a string from the default stdin.
 * The buffer has no limits until the CPU explodes (Core Dumped).
 * @param  str the buffer it must be allocated first
 * @return     the bytes read
 */
ssize_t fdut_getsStdi(char **str);

/**
 * Writes buf to fd and executes a callback
 * @param  fd    			the file descriptor
 * @param  buf   			the buffer to write
 * @param  count			the size of the buf in bytes
 * @param onResultCallback	the callback to execute after writting
 * @return       			a custom result, returned by the callback
 */
int fdut_writecb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t));

/**
 * Reads from fd to buf and executes a callback
 * @param  fd    			the file descriptor
 * @param  buf   			the buffer to read the data to
 * @param  count 			the size of the buf in bytes
 * @param onResultCallback	the callback to execute after reading
 * @return       			a custom result, returned by the callback
 */
int fdut_readcb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t));

/**
 * Checks if a file descriptor is still valid
 * @param	fd the file descriptor
 * @return	1 if file descriptor is valid, 0 otherwise
 */
int fdut_fdIsValid(int fd);

#endif
