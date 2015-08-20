/**
 * @author: Victor Caballero (vicaba)
 */
#include "timer.h"

int timfdh_create_and_start(int fd, struct timespec ts);

int timfdh_createAndStartPeriodicTimer(int fd, struct timespec ts)
{
	struct itimerspec newValue;
	bzero(&newValue, sizeof(struct itimerspec));

	newValue.it_value = ts;
	newValue.it_interval = ts;

	if ((fd = timerfd_create(CLOCK_MONOTONIC, TFD_NONBLOCK)) < 0) {
		return -1;
	}

	if (timerfd_settime(fd, 0, &newValue, NULL) < 0)
	{
		return -1;
	}

}