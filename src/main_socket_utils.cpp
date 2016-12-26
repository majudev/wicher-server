#include "main.h"
#include <fcntl.h>

bool set_blocking_socket(int fd, bool blocking){
	if (fd < 0) return false;
	int flags = fcntl(fd, F_GETFL, 0);
	if (flags < 0) return false;
	flags = blocking ? (flags&~O_NONBLOCK) : (flags|O_NONBLOCK);
	return (fcntl(fd, F_SETFL, flags) == 0) ? true : false;
}