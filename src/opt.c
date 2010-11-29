/*
 * opt.c - option management
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

/* Revision date: So Okt 16 22:18:25 UTC 2005 */

/* 
 * The API of this module is heavily influenced by both getopt and popt.
 * I don't use popt because in my opinion there shouldn't be a
 * dependency just for option handling. And I don't want to include
 * a library with about 2000+ lines of code, if the program is smaller
 * or not much bigger.
 */

#include "opt.h"

#if HAVE_CONFIG_H
# include <config.h>
#else
# undef STDC_HEADERS
# define STDC_HEADERS 1
#endif

#if STDC_HEADERS
# include <stdio.h> /* NULL */
# include <stdlib.h> /* malloc, free, strtod, strtol */
# include <string.h> /* strncmp */
# include <ctype.h> /* isspace */
#endif

#if ENABLE_NLS
# include <libintl.h>
# define _(String) dgettext(OPT_GETTEXT_DOMAIN, String)
#else
# define _(String) String
#endif

#define ARG_MASK 0x000f

enum { OPT_NONE, OPT_SHORT, OPT_LONG };

static char *find_arg(char **argv, int opt_type, int arginfo,
                      int major, int minor, int *arg_incl);
static int convert_arg(const char *arg, int arginfo, void *dest);
static int ident_opt(const char *o);
static int larg_is_incl(const char *l);
static int lookup_short(struct Option *options, char s);
static int lookup_long(struct Option *options, const char *l);
static char *print_wrap(char *text, size_t len, FILE *fp);

/* 
 * Creates a new OptContext pointer used by the other functions.
 * The "options" structure has to be allocated manually, see the
 * header file for member description.
 */
OptContext opt_new(int argc, char **argv, struct Option *options)
{
    OptContext oc;
    int i;

    if (!options || argc < 1 || !argv || !argv[0] || argv[argc] != NULL)
        return NULL;
    for (i = 1; i < argc; i++)
        if (!argv[i])
            return NULL;

    oc = (OptContext) malloc(sizeof(struct OptContext));
    if (!oc)
      return NULL;

    oc->args = (char **) malloc(sizeof(char *));
    if (!oc->args) {
      free(oc);
      return NULL;
    }

    oc->appname = argv[0];
    oc->argc = argc;
    oc->argv = argv;
    oc->options = options;
    oc->args_len = 0;
    oc->args_pos = 0;
    oc->major = 1; /* because argv[0] is no argument */
    oc->minor = 0;
    oc->ignore = 0;

    return oc;
}

/* 
 * Frees all memory associated with "oc".
 * If "free_args" is false, oc->args is not freed.
 */
void opt_free(OptContext oc, int free_args)
{
    if (free_args)
        free(oc->args);
    free(oc);
}

/*
 * Processes the next option. If no retval has been specified for that
 * option, it does not return but calls itself again.
 */
int opt_next(OptContext oc)
{
    int opt_n, opt_type, opt_arg_incl, rc;
    struct Option *opt;

    if (!oc || oc->major >= oc->argc)
        return 0;

    if (oc->ignore)
        opt_type = OPT_NONE;
    else {
        if (strcmp(oc->argv[oc->major], "--") == 0) {
            oc->ignore = 1;
            oc->major++;
            return opt_next(oc);
        }
        opt_type = oc->minor ? OPT_SHORT : ident_opt(oc->argv[oc->major]);
    }

    opt_n = -1;
    switch (opt_type) {
    case OPT_NONE:
        oc->args[oc->args_len++] = oc->argv[oc->major++];
        return opt_next(oc);
    case OPT_SHORT:
        oc->minor || (oc->minor = 1);
        opt_n = lookup_short(oc->options, oc->argv[oc->major][oc->minor]);
        break;
    case OPT_LONG:
        opt_n = lookup_long(oc->options, oc->argv[oc->major] + 2);
        break;
    }

    if (opt_n < 0)
        return OPT_ERROR_BADOPT;
    opt = oc->options + opt_n;

    oc->arg = find_arg(oc->argv, opt_type, opt->arginfo,
                       oc->major, oc->minor, &opt_arg_incl);
    rc = convert_arg(oc->arg, opt->arginfo, opt->arg);
    if (rc < 0)
        return rc;

    if (oc->arg) {
        oc->major += (opt_arg_incl) ? 1 : 2;
        oc->minor = 0;
    } else switch (opt_type) {
        case OPT_SHORT:
            if (oc->argv[oc->major][oc->minor + 1] == '\0') {
                oc->major += 1;
                oc->minor = 0;
            } else
                oc->minor += 1;
            break;
        case OPT_LONG:
            oc->major += 1;
            break;
    }


    return opt->retval ? opt->retval : opt_next(oc);
}

/*
 * Outputs human-readable messages of error-code "error"
 * (returned by opt_next) to stderr.
 */
void opt_perror(OptContext oc, int error)
{
    if (!oc)
        return;

    switch (error) {
    case OPT_ERROR_NOARG:
        if (oc->minor)
            fprintf(stderr, _("%s: option requires an argument -- %c\n"),
                    oc->appname, oc->argv[oc->major][oc->minor]);
        else
            fprintf(stderr, _("%s: option `%s' requires an argument\n"),
                    oc->appname, oc->argv[oc->major]);
        break;
    case OPT_ERROR_BADOPT:
        if (oc->minor)
            fprintf(stderr, _("%s: invalid option -- %c\n"),
                     oc->appname, oc->argv[oc->major][oc->minor]);
        else
            fprintf(stderr, _("%s: unrecognized option `--%s'\n"),
                    oc->appname, oc->argv[oc->major] + 2);
        break;
    case OPT_ERROR_ILLEGALARG:
        fprintf(stderr, _("%s: option `--%s' doesn't allow an argument\n"),
                oc->appname, oc->argv[oc->major] + 2);
        break;
    case OPT_ERROR_BADNUMBER:
        fprintf(stderr, _("%s: invalid numeric value: %s\n"),
                oc->appname, oc->arg);
        break;
    }
}

/* Proccesses and returns the next leftover argument. */
char *opt_get_arg(OptContext oc)
{
    return (oc && oc->args[oc->args_pos]) ? oc->args[oc->args_pos++] : NULL;
}

/* Returns the next leftover argument. */
char *opt_peek_arg(OptContext oc)
{
    return (oc && oc->args[oc->args_pos]) ? oc->args[oc->args_pos + 1] : NULL;
}

/*
 * After option parsing with opt_next is finished, returns the allocated
 * list of leftover arguments (similar to argv).
 */
char **opt_get_args(OptContext oc)
{
    return (oc) ? oc->args : NULL;
}

/* Outputs a GNU-style option description to "fp". */
void opt_print_descrip(OptContext oc, FILE *fp)
{
    struct Option *opts;
    size_t j, len, cols, left_col, right_col;
    char gap, *env, *desc;
    int i;

    if (!oc || !fp)
        return;

    opts = oc->options;
    env = getenv("COLUMNS");
    cols = env ? atoi(env) : 80;

    left_col = 0;
    for (i = 0; opts[i].longname || opts[i].shortname; i++) {
        len = (opts[i].longname ? strlen(opts[i].longname) : 0) + 12 +
              (opts[i].arg_descrip ? strlen(opts[i].arg_descrip) : 0);
        if (len > left_col)
            left_col = len;
    }
    right_col = cols - left_col;

    for (i = 0; opts[i].longname || opts[i].shortname; i++) {
        if (opts[i].longname) {
            if (opts[i].shortname)
                len = fprintf(fp, "  -%c, --%s",
                    opts[i].shortname, opts[i].longname);
            else
                len = fprintf(fp, "      --%s", opts[i].longname);
            gap = '=';
        } else {
            len = fprintf(fp, "  -%c", opts[i].shortname);
            gap = ' ';
        }
        if (opts[i].arg_descrip)
            len += fprintf(fp, "%c%s", gap, opts[i].arg_descrip);

        desc = opts[i].descrip;
        while (desc) {
            for (j = len; j < left_col; j++)
                fputc(' ', fp);
            desc = print_wrap(desc, right_col, fp);
            fputc('\n', fp);
            len = 0;
        }
    }
}

/*
 * Finds the argument to current option in "argv".
 *
 * argv: argument vector
 * opt_type: type of current option
 * arginfo: information about arg
 * major, minor: current position in argv
 * arg_incl: if the arg is included (e.g.: "--file=foobar"),
 *           set *arg_incl to 1
 *
 * Returns the argument or NULL if none was found.
 */
static char *find_arg(char **argv, int opt_type, int arginfo,
                      int major, int minor, int *arg_incl)
{
    char *ev_arg, *arg;

    *arg_incl = 0;
    arg = ev_arg = NULL;

    if (opt_type == OPT_SHORT && argv[major][minor + 1] != '\0' &&
            (arginfo & ARG_MASK) != ARG_NONE) {
        *arg_incl = minor + 1;
        ev_arg = argv[major] + *arg_incl;
    } else if (opt_type == OPT_LONG &&
            (*arg_incl = larg_is_incl(argv[major]))) {
        arg = argv[major] + *arg_incl;
    } else if (argv[major + 1] != NULL &&
            ident_opt(argv[major + 1]) == OPT_NONE) {
        ev_arg = argv[major + 1];
    }

    if (!arg && ev_arg && !(arginfo & ARGFLAG_OPTIONAL))
        arg = ev_arg;

    return arg;
}

/* Transforms "arg" according to "arginfo", and stores result in "dest". */
static int convert_arg(const char *arg, int arginfo, void *dest)
{
    int argtype;
    long l;
    double d;
    char *endp;

    argtype = arginfo & ARG_MASK;

    if (argtype == ARG_NONE) {
        if (arg) return OPT_ERROR_ILLEGALARG;
    } else {
        if (!arg) return OPT_ERROR_NOARG;
    }

    if (!dest)
        return 0;

    l = d = 0;
    switch (argtype) {
    case ARG_INT: case ARG_LONG:
        l = strtol(arg, &endp, 0);
        break;
    case ARG_FLOAT: case ARG_DOUBLE:
        d = strtod(arg, &endp);
        break;
    default:
        endp = NULL;
        break;
    }

    if (endp && *endp != '\0')
        return OPT_ERROR_BADNUMBER;

    switch (argtype) {
    case ARG_NONE: *((int *) dest) = 1; break;
    case ARG_STRING: *((char **) dest) = (char *) arg; break;
    case ARG_INT: *((int *) dest) = (int) l; break;
    case ARG_LONG: *((long *) dest) = l; break;
    case ARG_FLOAT: *((float *) dest) = (float) d; break;
    case ARG_DOUBLE: *((double *) dest) = d; break;
    }

    return 1;
}

/* Returns type of option "o" (no, short or long option). */
static int ident_opt(const char *o)
{
    if (o[0] != '-' || o[1] == '\0')
        return OPT_NONE;

    if (o[1] == '-')
        return (o[2] != '\0') ? OPT_LONG : OPT_NONE;

    return OPT_SHORT;
}

/* Tests if long option "l" has an included argument.
   When found, it's position in "l" is returned, otherwise 0. */
static int larg_is_incl(const char *l)
{
    int i;

    for (i = 3; l[i] != '\0' && l[i] != '='; i++)
        ;
    if (l[i] == '\0' || l[i+1] == '\0')
        return 0;

    return i + 1;
}

/* Returns position of short option "l" in "options", or -1.*/
static int lookup_short(struct Option *options, char s)
{
    int i;
    for (i = 0; options[i].longname || options[i].shortname; i++)
        if (s == options[i].shortname)
            return i;
    return -1;
}

/* Returns position of long option "l" in "options", or -1. */
static int lookup_long(struct Option *options, const char *l)
{
    int i, len;
    for (len = 0; l[len] != '=' && l[len] != '\0'; len++);
    for (i = 0; options[i].longname || options[i].shortname; i++)
        if (strncmp(l, options[i].longname, len) == 0)
            return i;
    return -1;
}

/*
 * Writes at most "len" chars of "text" (NUL-terminated) into "fp".
 * It doesn't stop in the middle of words.
 *
 * Returns the beginning of the still unwritten text.
 */
static char *print_wrap(char *text, size_t len, FILE *fp)
{
    size_t i, text_len;

    text_len = strlen(text);
    if (text_len >= len)
        for (i = 0; i < len; i++)
            if (isspace(text[i]))
                text_len = i;
    fwrite(text, 1, text_len, fp);
    text += text_len;
    return (text[0] != '\0') ? text + 1 : NULL;
}
