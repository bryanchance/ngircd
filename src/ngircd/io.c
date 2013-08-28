/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * Please read the file COPYING, README and AUTHORS for more information.
 *
 * Copyright (c) 2005 Florian Westphal (westphal@foo.fh-furtwangen.de)
 */

#include "portab.h"

/**
 * @file
 * I/O abstraction interface.
 */

#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>

#include "array.h"
#include "io.h"
#include "log.h"

/* Enables extra debug messages in event add/delete/callback code. */
/* #define DEBUG_IO */

struct event_base *base;

/**
 * This code base seems to associate a single event handler with a file descriptor
 * at any given time. In order to track that association, I am using an array
 * indexed by file descriptor. A refactor could probably eliminate this hack.
 **/
/* I am using 100 since it appeared the the existing code */
#define MAX_EVENT_FDS 100
/* An array to associate a file descriptor with its singular event handler */
static void *io_events[MAX_EVENT_FDS] = {0} ;

#ifdef DEBUG_IO
static void
io_debug(const char *s, int fd, int what)
{
	Log(LOG_DEBUG, "%s: %d, %d\n", s, fd, what);
}
#else
static inline void
io_debug(const char UNUSED *s,int UNUSED a, int UNUSED b)
{ /* NOTHING */ }
#endif

bool
io_library_init(unsigned int UNUSED eventsize)
{
#ifdef DEBUG_IO
	event_enable_debug_mode();
#endif
	base = event_base_new();
	return base;
}

void
io_library_shutdown(void)
{
	event_base_free(base);
}

bool io_event_change(int fd, short what, void (*cbfunc) (int, short, void*))
{
	void *i = io_events[fd];
	event_del(i);
	event_free(i);
	io_debug("io_event_change", fd, what);
	i = event_new(base, fd, what, cbfunc, NULL);
	io_events[fd] = i;
	return event_add(i, NULL) == 0;
}

bool
io_event_setcb(int fd, void (*cbfunc) (int, short, void*))
{
	void *i = io_events[fd];
	if (!i)
		return false;
	short what = event_get_events(i);
	io_debug("io_event_setcb", fd, what);
	return io_event_change(fd, what, cbfunc);
}

bool
io_event_create(int fd, short what, void (*cbfunc) (int, short, void*))
{
	void *i;
	assert(fd >= 0);
	assert(io_events[fd] == NULL);
	io_debug("io_event_create", fd, what);
	/* EV_PERSIST flag needed based on this io library expected behavior */
	/* EV_ET flag will try to use edge triggered events if available */
	i = event_new(base, fd, what|EV_PERSIST|EV_ET, cbfunc, NULL);
	io_events[fd] = i;
	return event_add(i, NULL) == 0;
}

bool
io_event_add(int fd, short what)
{
	void *i = io_events[fd];
	if (!i) return false;

	short oldwhat = event_get_events(i);
	if ((oldwhat & what) == what) /* event type is already registered */
		return true;

	io_debug("io_event_add: fd, what", fd, what);
	event_callback_fn cbfunc = event_get_callback(i);
	return io_event_change(fd, oldwhat | what, cbfunc);
}


bool
io_setnonblock(int fd)
{
	int flags = fcntl(fd, F_GETFL);
	if (flags == -1)
		return false;
#ifndef O_NONBLOCK
#define O_NONBLOCK O_NDELAY
#endif
	flags |= O_NONBLOCK;

	return fcntl(fd, F_SETFL, flags) == 0;
}

bool
io_setcloexec(int fd)
{
	int flags = fcntl(fd, F_GETFD);
	if (flags == -1)
		return false;
#ifdef FD_CLOEXEC
	flags |= FD_CLOEXEC;
#endif

	return fcntl(fd, F_SETFD, flags) == 0;
}

bool
io_close(int fd)
{
	void *i = io_events[fd];
	event_del(i);
	event_free(i);
	io_events[fd] = NULL;
	return close(fd) == 0;
}


bool
io_event_del(int fd, short what)
{
	void *i = io_events[fd];
	if (!i) return false;

	short oldwhat = event_get_events(i);
	if (!(oldwhat & what)) /* event type is already disabled */
		return true;

	io_debug("io_event_del: fd, what", fd, what);
	event_callback_fn cbfunc = event_get_callback(i);
	return io_event_change(fd, oldwhat & ~what, cbfunc);
}

int
io_dispatch(struct timeval *tv)
{
	event_base_loopexit(base, tv);
	return event_base_dispatch(base) == 0;
}
