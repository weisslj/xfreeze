/* 
 * signals.c - catch signals
 *
 * Copyright (C) 2004-2017 Johannes Weißl
 *
 * This file is part of xfreeze.
 *
 * xfreeze is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * xfreeze is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with xfreeze.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "signals.h"
#include "lock.h"
#include "opt.h"

#include <stdio.h> /* remove */
#include <stdlib.h> /* exit */
#include <signal.h> /* signal */
#include <X11/Xlib.h> /* Display */

#if HAVE_CONFIG_H
#include <config.h>
#endif

static Display *curdpy;

static RETSIGTYPE xfreeze_quit(int sig)
{
    xunlock(curdpy, 1);
    XCloseDisplay(curdpy);
    exit(sig);
}

void init_signals(Display *display)
{
    curdpy = display;
    signal(SIGINT, SIG_IGN);
    signal(SIGABRT, xfreeze_quit);
    signal(SIGFPE, xfreeze_quit);
    signal(SIGILL, xfreeze_quit);
    signal(SIGSEGV, xfreeze_quit);
    signal(SIGTERM, xfreeze_quit);
}
