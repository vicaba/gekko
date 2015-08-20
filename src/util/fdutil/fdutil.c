/**
 * @author: Victor Caballero (vicaba)
 */
#include "fdutil.h"

/*
 * Prototypes
 */
/*
 * Public
 */
ssize_t fdut_msg(int fd, char *msg);

ssize_t fdut_msgf(int fd, char *buffer, char *format, ...);

ssize_t fdut_vmsgf(int fd, char *buffer, char *format, va_list args);

ssize_t fdut_msgStdo(char *msg);

ssize_t fdut_msgStdof(char *buffer, char *format, ...);

ssize_t fdut_gets(int fd, char **str);

ssize_t fdut_getsStdi(char **str);

int fdut_writecb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t));

int fdut_readcb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t));

int fdut_fdIsValid(int fd);

/*
 * Private
 */
/*
 * Prototypes end
 */



ssize_t fdut_msg(int fd, char *msg)
{
	size_t len;

	len = strlen(msg);
	return write(fd, msg, sizeof(char) * len);
}

ssize_t fdut_msgf(int fd, char *buffer, char *format, ...)
{
	va_list args;

	va_start(args, format);
	vsprintf(buffer, format, args);
	va_end(args);

	return fdut_msg(fd, buffer);

}

ssize_t fdut_vmsgf(int fd, char *buffer, char *format, va_list args)
{

	vsprintf(buffer, format, args);

	return fdut_msg(fd, buffer);

}

ssize_t fdut_msgStdo(char *msg)
{
	return fdut_msg(1, msg);
}

ssize_t fdut_msgStdof(char *buffer, char *format, ...)
{
	va_list args;
	ssize_t rwbytes;

	va_start(args, format);
	rwbytes = fdut_vmsgf(1, buffer, format, args);
	va_end(args);

	return rwbytes;
}


ssize_t fdut_gets(int fd, char **str)
{

	ssize_t rwbytes;
	ssize_t locrwbytes;
	int i;
	int _case = 0; //default, line ending with \0

	locrwbytes = 0;
	rwbytes = 0;
	i = 0;

	do {
		i++;
		rwbytes += locrwbytes;
		*str = (char *)realloc(*str, sizeof(char) * i);
		if (*str == NULL) {
			fdut_msgStdo("Error reallocating memory\n");
			return -1;
		}
		locrwbytes = read(fd, &((*str)[i - 1]), sizeof(char));
		if ((*str)[i - 1] == '\n' || locrwbytes == 0) {
			_case = 1;
			break;
		}
	} while ((*str)[i - 1] != '\0' && locrwbytes != 0);

	switch (_case) {
	case 0:
	{
		return rwbytes;
	}
	case 1:
	{
		i++;
		(*str) = (char *)realloc((*str), sizeof(char) * i);
		if (*str == NULL) {
			fdut_msgStdo("Error reallocating memory\n");
			return -1;
		}
		(*str)[i - 1] = '\0';
		return rwbytes;
	}

	default:
	{
		return rwbytes;
	}

	}

}

ssize_t fdut_getsStdi(char **str)
{
	return fdut_gets(0, str);
}

int fdut_writecb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t))
{
	return onResultCallback(write(fd, buf, count));
}

int fdut_readcb(int fd, void *buf, size_t count, int (*onResultCallback)(ssize_t))
{
	return onResultCallback(read(fd, buf, count));
}

int fdut_fdIsValid(int fd)
{
	return fcntl(fd, F_GETFL) != -1 || errno != EBADF;
}



