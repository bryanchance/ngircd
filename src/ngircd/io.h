/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Please read the file COPYING, README and AUTHORS for more information.
 */

#ifndef io_H_included
#define io_H_included

/**
 * @file
 * I/O abstraction interface (header)
 */

#include "portab.h"
#include <sys/time.h>
#include <event2/event.h>

#define IO_WANTREAD	EV_READ
#define IO_WANTWRITE	EV_WRITE

/* init library.
   sets up epoll/kqueue descriptors and tries to allocate space for ioevlen
   file descriptors. ioevlen is just the _initial_ size, not a limit. */
bool io_library_init PARAMS((unsigned int ioevlen));

/* shutdown and free all internal data structures */
void io_library_shutdown PARAMS((void));

/* add fd to internal set, enable readability check, set callback */
bool io_event_create PARAMS((int fd, short what, void (*cbfunc)(int, short, void*)));

/* change callback function associated with fd */
bool io_event_setcb PARAMS((int fd, void (*cbfunc)(int, short, void*)));

/* watch fd for event of type what */
bool io_event_add PARAMS((int fd, short what));

/* do not watch fd for event of type what */
bool io_event_del PARAMS((int fd, short what));

/* remove fd from watchlist, close() fd.  */
bool io_close PARAMS((int fd));

/* set O_NONBLOCK */
bool io_setnonblock PARAMS((int fd));

/* set O_CLOEXEC */
bool io_setcloexec PARAMS((int fd));

/* watch fds for activity */
int io_dispatch PARAMS((struct timeval *tv));

#endif /* io_H_included */
