/* 
 * auth_unix.h - ...
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

#ifndef AUTH_UNIX_H
#define AUTH_UNIX_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_FEATURES_H
# define _XOPEN_SOURCE
# include <features.h>
#endif /* HAVE_FEATURES_H */

#include <X11/Xlib.h> /* Display */

int auth_unix(Display *display);

#endif /* AUTH_UNIX_H */
