/* 
 * lock.c - locking functions
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

/* some ideas come from xscreensaver (http://www.jwz.org/xscreensaver/) */

#include "lock.h"

#include <stdio.h> /* perror */
#include <stdlib.h> /* getenv */

#include <X11/Xlib.h> /* Display */

#include <sys/types.h> /* open */
#include <sys/stat.h> /* open */
#include <fcntl.h> /* open */

#include <unistd.h> /* write, close */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_SYS_IOCTL_H
#include <sys/ioctl.h> /* ioctl */
#endif

#ifdef HAVE_SYS_VT_H
#include <sys/vt.h> /* VT_LOCKSWITCH, VT_UNLOCKSWITCH */
#endif

#ifdef HAVE_LOCKMODESWITCH
#include <X11/extensions/xf86vmode.h> /* XF86VidModeLockModeSwitch */
#endif

#ifdef HAVE_SETGRABKEYSSTATE
#include <X11/extensions/xf86misc.h> /* XF86MiscSetGrabKeysState */
#endif

#define DEV_CONSOLE "/dev/console"
#define SYSRQ_PROC "/proc/sys/kernel/sysrq"

#if defined HAVE_IOCTL && defined HAVE_SYS_VT_H
static int lock_vtswitch(int lock);
#endif

static int lock_sysrq(int lock);

int xlock(Display *display, int mouse, int vtswitch, int sysrq)
{
    int screen_num;
    Window root_window;

    screen_num = DefaultScreen(display);
    root_window = RootWindow(display, screen_num);

#ifdef HAVE_LOCKMODESWITCH
    XF86VidModeLockModeSwitch(display, screen_num, 1);
#endif

#ifdef HAVE_SETGRABKEYSSTATE
    XF86MiscSetGrabKeysState(display, 0);
#endif

#if defined HAVE_IOCTL && defined HAVE_SYS_VT_H
    if (vtswitch)
        lock_vtswitch(1);
#endif

    if (sysrq)
        lock_sysrq(1);

    if (mouse)
        XGrabPointer(display, root_window, False,
                     ButtonPressMask | ButtonReleaseMask,
                     GrabModeAsync, GrabModeAsync, None, None, CurrentTime);
    XGrabKeyboard(display, root_window, False,
                  GrabModeAsync, GrabModeAsync, CurrentTime);

    return 1;
}

int xunlock(Display *display, int mouse)
{
    XUngrabKeyboard(display, CurrentTime);
    if (mouse)
        XUngrabPointer(display, CurrentTime);

    lock_sysrq(0);

#if defined HAVE_IOCTL && defined HAVE_SYS_VT_H
    lock_vtswitch(0);
#endif

#ifdef HAVE_SETGRABKEYSSTATE
    XF86MiscSetGrabKeysState(display, 1);
#endif

#ifdef HAVE_LOCKMODESWITCH
    XF86VidModeLockModeSwitch(display, DefaultScreen(display), 0);
#endif

    return 1;
}

#if defined HAVE_IOCTL && defined HAVE_SYS_VT_H
static int lock_vtswitch(int lock)
{
    static int status;
    char *dev_console;
    int fd;

    if (status == lock)
        return 2;

    /* is this illegal? */
    dev_console = getenv("DEV_CONSOLE");
    if (!dev_console)
        dev_console = DEV_CONSOLE;

    fd = open(dev_console, O_RDWR);
    if (fd < 0) {
        perror("lock_vtswitch [open]");
        return 0;
    }

    if (ioctl(fd, (lock ? VT_LOCKSWITCH : VT_UNLOCKSWITCH)) < 0) {
        close(fd);
        perror("lock_vtswitch [ioctl]");
        return 0;
    }

    close(fd);

    status = lock;

    return 1;
}
#endif

static int lock_sysrq(int lock)
{
    static int status;
    int fd;

    if (status == lock)
        return 2;

    fd = open(SYSRQ_PROC, O_WRONLY);
    if (fd < 0) {
        perror("lock_sysrq [open]");
        return 0;
    }

    write(fd, (lock ? "0" : "1"), 1);

    close(fd);

    status = lock;

    return 1;
}
