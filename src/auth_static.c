/* 
 * auth_static.c - ...
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

#include "auth_static.h"
#include "input.h" /* xgetstr */

#include <stdio.h> /* NULL */
#include <stdlib.h> /* rand, srand */
#include <ctype.h> /* isalnum */
#include <time.h> /* time */
#include <string.h> /* strcmp */
#include <unistd.h> /* crypt */ 

static const char *gen_salt(void);
static char rand_char(void);

static char static_key[14];

void auth_static_init(const char *passwd)
{
    char *key;
    srand(time(NULL));
    key = crypt(passwd, gen_salt());
    strcpy(static_key, key);
}

int auth_static(Display *display)
{
    const char *passwd;
    passwd = xgetstr(display, NULL, 0);
    return (strcmp(static_key, crypt(passwd, static_key)) == 0) ? 1 : 0;
}

static const char *gen_salt(void)
{
    static char salt[3];
    int i;

    for (i = 0; i < 2; i++)
        salt[i] = rand_char();
    salt[2] = '\0';

    return salt;
}

static char rand_char(void)
{
    char c;

    for (;;) {
        c = (char) rand();
        if (isalnum(c) || c == '.' || c == '/')
            break;
    }

    return c;
}
