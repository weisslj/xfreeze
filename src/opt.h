/*
 * opt.c - option management
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

/* Revision date: So Okt 16 22:18:25 UTC 2005 */

#ifndef OPT_H
#define OPT_H

#if HAVE_CONFIG_H
# include <config.h>
#else
# undef STDC_HEADERS
# define STDC_HEADERS 1
#endif

#if STDC_HEADERS
# include <stdio.h> /* FILE */
#endif

#ifndef OPT_GETTEXT_DOMAIN
# define OPT_GETTEXT_DOMAIN "libc"
#endif

enum { ARG_NONE, ARG_STRING, ARG_INT, ARG_LONG, ARG_FLOAT, ARG_DOUBLE };

enum {
    ARGFLAG_OPTIONAL = 0x0010,
    ARGFLAG_DOC_HIDDEN = 0x0020
};

#define OPT_TABLEEND { NULL, '\0', 0, NULL, 0, NULL, NULL }

enum {
    OPT_ERROR_BADOPT = -10,
    OPT_ERROR_NOARG = -20,
    OPT_ERROR_ILLEGALARG = -30,
    OPT_ERROR_BADNUMBER = -40
};

struct Option {
    const char *longname;  /* long option name, e.g. "help" */
    char shortname;        /* short option name, e.g. 'h' */
    int arginfo;           /* bitmask describing the arg (see enums above) */
    void *arg;             /* location where to store the arg */
    int retval;            /* if not 0, opt_next returns with that value */
    char *descrip;         /* option description (e.g. "create FILE") */
    char *arg_descrip;     /* arg description (e.g. "FILE") */
};

typedef struct OptContext * OptContext;

struct OptContext {
    const char *appname;
    int argc;
    char **argv;
    int major;
    int minor;
    int ignore;
    struct Option *options;
    char **args;
    int args_len;
    int args_pos;
    char *arg;
};

OptContext opt_new(int argc, char **argv, struct Option *options);
void opt_free(OptContext oc, int free_args);
int opt_next(OptContext oc);
char *opt_get_arg(OptContext oc);
char *opt_peek_arg(OptContext oc);
char **opt_get_args(OptContext oc);
void opt_print_descrip(OptContext oc, FILE *fp);
void opt_perror(OptContext oc, int error);

#endif /* OPT_H */
