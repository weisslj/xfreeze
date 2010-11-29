/* 
 * auth_unix.c - ...
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

#include "auth_unix.h"
#include "input.h" /* xgetstr */

#include <stdio.h> /* NULL */
#include <stdlib.h> /* free */
#include <string.h> /* strlen, strcmp */

#include <unistd.h> /* getuid, crypt */
#include <pwd.h> /* getpwuid */
#include <sys/types.h> /* uid_t */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if HAVE_SHADOW_H
# include <shadow.h> /* getspnam */
#endif

static char *get_key(uid_t uid);

int auth_unix(Display *display)
{
    const char *key, *passwd;

    key = get_key(getuid());
    if (!key)
        return 0;

    passwd = xgetstr(display, NULL, 0);
    return (strcmp(key, crypt(passwd, key)) == 0) ? 1 : 0;
}

static char *get_key(uid_t uid)
{
    struct passwd *pw;
#if HAVE_SHADOW_H
    struct spwd *sp;
#endif

    pw = getpwuid(uid);
    if (!pw)
        return NULL;
#if HAVE_SHADOW_H
    sp = getspnam(pw->pw_name);
    if (sp)
        return sp->sp_pwdp;
#endif
    return (strlen(pw->pw_passwd) >= 13) ? pw->pw_passwd : NULL;
}
