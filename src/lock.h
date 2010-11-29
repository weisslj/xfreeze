/* 
 * lock.h - locking functions
 *
 * Copyright 2004 Johannes Weißl
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
 * along with xfreeze; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef LOCK_H
#define LOCK_H

#include <X11/Xlib.h> /* Display */
#include <unistd.h> /* ssize_t */

int xlock(Display *display, int mouse, int vtswitch, int sysrq);
int xunlock(Display *display, int mouse);

#endif /* LOCK_H */
