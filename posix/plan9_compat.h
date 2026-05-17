/*
 * Plan 9 libc shims for POSIX (macOS, Linux).
 * Include instead of <u.h> / <libc.h>.
 */
#ifndef PLAN9_COMPAT_H
#define PLAN9_COMPAT_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

#define nil ((void *)0)
#define print printf
#define cistrcmp strcasecmp

/* Plan 9 fprint(2, ...) → stderr */
#define fprint(fd, ...) fprintf(((fd) == 2 ? stderr : stdout), __VA_ARGS__)

static inline void
exits(const char *msg)
{
	if(msg != nil)
		exit(1);
	exit(0);
}

#endif
