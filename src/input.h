/* 
 * input.h - X input functions
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

#ifndef INPUT_H
#define INPUT_H

#include <X11/Xlib.h> /* Display */

char *xgetstr(Display *display, char *str, size_t max);

#endif /* INPUT_H */
