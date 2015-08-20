/** 
 * @author: Victor Caballero (vicaba)
 * @brief Helper functions to set up a timer filedescriptor
 */
#ifndef __TIMERFD_H_H__
#define __TIMERFD_H_H__

#include <stdio.h>
#include <sys/timerfd.h>
#include <stdint.h>

int timfdh_create_and_start(int fd, struct timespec ts);

#endif
