/* 
 * input.c - X input functions
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

#include "input.h"

#include <stdio.h> /* NULL */
#include <stdlib.h> /* malloc */
#include <string.h> /* strncat */

#include <X11/Xlib.h> /* Display */
#include <X11/Xutil.h> /* XLookupString */
#include <X11/keysym.h> /* XK_Return */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#ifndef MB_CUR_MAX
#define MB_CUR_MAX 6
#endif

#define ALLOC_STEP 64

/*
 * read a string from the keyboard
 *
 * display : X-display to use
 * str     : buffer, where the string is stored
 *           if it is NULL, memory is allocated using malloc
 * max     : the maximal length of the string
 *           0 means no length limit
 *           if the string is longer, then NULL is returned
 *
 * returns the read string or NULL on error
 */
char *xgetstr(Display *display, char *str, size_t max)
{
    char *buf;
    int n;
    KeySym keysym;
    XEvent event;
    size_t len, alloc;

    buf = (char *) malloc(MB_CUR_MAX);
    if (!buf)
        return NULL;

    if (!str) {
        alloc = ALLOC_STEP;
        str = (char *) malloc(alloc);
        if (!str) {
            free(buf);
            return NULL;
        }
    } else
        alloc = 0;

    str[0] = '\0';
    len = 0;

    for (;;) {
        XNextEvent(display, &event);
        if (event.type != KeyPress)
            continue;
        XLookupString((XKeyEvent *) &event, buf, MB_CUR_MAX, &keysym, NULL);
        if (keysym == XK_Return)
            break;
        if (IsCursorKey(keysym) || IsFunctionKey(keysym) ||
                IsKeypadKey(keysym) || IsMiscFunctionKey(keysym) ||
                IsModifierKey(keysym) || IsPFKey(keysym) ||
                IsPrivateKeypadKey(keysym))
            continue;
#ifdef HAVE_WCTOMB
        n = wctomb(buf, (wchar_t) keysym);
        if (n < 0)
            continue;
#else
        buf[0] = (char) keysym;
        n = 1;
#endif
        len += n;
        if (max > 0 && len > max) {
            free(str);
            free(buf);
            return NULL;
        }
        if (alloc && len > alloc) {
            alloc += ALLOC_STEP;
            if (max > 0 && alloc > max)
                alloc = max;
            str = (char *) realloc(str, alloc);
            if (!str) {
                free(buf);
                return NULL;
            }
        }
        strncat(str, buf, n);
    }

    free(buf);

    return str;
}
