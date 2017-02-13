/* 
 * auth_static.h - ...
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

#ifndef AUTH_STATIC_H
#define AUTH_STATIC_H

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifdef HAVE_FEATURES_H
# define _XOPEN_SOURCE
# include <features.h>
#endif /* HAVE_FEATURES_H */

#include <X11/Xlib.h> /* Display */

void auth_static_init(const char *key);
int auth_static(Display *display);

#endif /* AUTH_STATIC_H */
