/* 
 * main.c - main module
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

#include "lock.h" /* xlock, xunlock */
#include "signals.h" /* init_signals */
#include "opt.h"

#include <stdio.h> /* printf */
#include <string.h> /* memset, strlen */
#include <stdlib.h> /* getenv */
#include <errno.h> /* errno */

#if HAVE_CONFIG_H
#include <config.h>
#endif

#if USE_LOCKFILE
#include <sys/types.h> /* pid_t */
#include <signal.h> /* kill */
#define LOCKFILE PACKAGE ".lock"
#endif /* USE_LOCKFILE */

#include <X11/Xlib.h> /* XOpenDisplay */
#include <unistd.h> /* getpass, getpid */

#if HAVE_LOCALE_H
#include <locale.h> /* setlocale */
#endif

#if USE_AUTH_PAM
#include "auth_pam.h"
#endif
#if USE_AUTH_UNIX
#include "auth_unix.h"
#endif
#if USE_AUTH_STATIC
#include "auth_static.h"
#endif


enum { OPTION_HELP = 1, OPTION_VERSION };

static char *Tmpdir;

static void print_help(OptContext optcon);
static void print_version(void);
static void cleanup(void);

int main(int argc, char *argv[])
{
    int (*auth) (Display *);
    Display *display;
    int mouse_lock, vtlock, sysrq_lock;
    char *display_name, *env;
#if USE_LOCKFILE
    FILE *lockfile;
    pid_t pid;
#endif
    static int do_askpass, timeout, opt_vtswitch, opt_sysrq, opt_mouse;
    static char *passwd;
    int r;
    OptContext optcon;
    static struct Option options[] = {
#if USE_AUTH_STATIC
        { "askpass", 'a', ARG_NONE, &do_askpass, 0,
          "asks for password", NULL },
        { "passwd", 'p', ARG_STRING, &passwd, 0,
          "specify password", "PASSWORD" },
#endif /* USE_AUTH_STATIC */
        { "vtswitch", 's', ARG_NONE, &opt_vtswitch, 0,
          "enable VT switching", NULL },
        { "sysrq", 'r', ARG_NONE, &opt_sysrq, 0,
          "do not disable sysrq", NULL },
        { "timeout", 't', ARG_INT, &timeout, 0,
          "seconds to wait till freezing", "TIME" },
        { "mouse", 'm', ARG_NONE, &opt_mouse, 0,
          "enable mouse", NULL },
        { "help", 'h', ARG_NONE, NULL, OPTION_HELP,
          "display this help and exit", NULL },
        { "version", 'v', ARG_NONE, NULL, OPTION_VERSION,
          "output version information and exit", NULL },
        OPT_TABLEEND
    };

#if HAVE_SETLOCALE
    setlocale(LC_ALL, "");
#endif

    optcon = opt_new(argc, argv, options);

    while ((r = opt_next(optcon)) > 0) {
        switch (r) {
        case OPTION_HELP:
            print_help(optcon);
            break;
        case OPTION_VERSION:
            print_version();
            break;
        }
    }

    if (r < 0) {
        opt_perror(optcon, r);
        exit(EXIT_FAILURE);
    }

    /* opt_set("lock_mouse", INT, opt_mouse ? 0 : 1); */

    opt_free(optcon, 1);

#if USE_LOCKFILE
    env = getenv("TMPDIR");
    Tmpdir = env ? env : "/tmp";
    chdir(Tmpdir);
    lockfile = fopen(LOCKFILE, "r");
    if (lockfile) {
        fscanf(lockfile, "%u\n", (unsigned int *) &pid);
        fclose(lockfile);
        if (kill(pid, 0) < 0 && errno == ESRCH) {
            fprintf(stderr, "%s: removing stale lockfile\n", PACKAGE);
            remove(LOCKFILE);
        } else {
            fprintf(stderr, "%s: already running\n", PACKAGE);
            exit(EXIT_FAILURE);
        }
    }
    lockfile = fopen(LOCKFILE, "w");
    if (lockfile) {
        fprintf(lockfile, "%u\n", (unsigned int) getpid());
        fclose(lockfile);
    }
#endif /* USE_LOCKFILE */

    atexit(cleanup);


#if HAVE_GETPASS
    if (do_askpass)
        passwd = getpass("Password: ");
#endif /* HAVE_GETPASS */

#if USE_AUTH_STATIC
    if (passwd) {
        auth_static_init(passwd);
        memset(passwd, '\0', strlen(passwd));
        auth = auth_static;
    } else
#endif /* USE_AUTH_STATIC */
    {
#if USE_AUTH_PAM
        auth = auth_pam;
#elif USE_AUTH_UNIX
        auth = auth_unix;
#else
        fprintf(stderr, "%s: no password specified\n", PACKAGE);
        exit(EXIT_FAILURE);
#endif
    }

#if HAVE_SLEEP
    if (timeout)
        sleep(timeout);
#endif /* HAVE_SLEEP */

    display_name = getenv("DISPLAY");
    display = XOpenDisplay(display_name);
    if (!display) {
        fprintf(stderr, "can't open display: %s\n", display_name);
        exit(EXIT_FAILURE);
    }

    init_signals(display);

    mouse_lock = opt_mouse ? 0 : 1;
    vtlock = opt_vtswitch ? 0 : 1;
    sysrq_lock = opt_sysrq ? 0 : 1;

    xlock(display, mouse_lock, vtlock, sysrq_lock);

    while (!auth(display));

    xunlock(display, mouse_lock);

    XCloseDisplay(display);

#ifdef PIDFILE
    remove(PIDFILE);
#endif

    return EXIT_SUCCESS;
}

static void print_help(OptContext optcon)
{
    printf("Usage: %s [OPTION]...\n", optcon->appname);
    printf("Locks X until the correct password is typed.\n\n");
    printf("Options:\n");
    opt_print_descrip(optcon, stdout);
    printf("\nReport bugs to %s.\n", PACKAGE_BUGREPORT);
    exit(EXIT_SUCCESS);
}

static void print_version(void)
{
    printf("%s\n\n", PACKAGE_STRING);
    printf("Copyright (C) 2004-2017 Johannes Weißl\n"
           "This is free software; see the source for copying conditions.  "
           "There is NO\nwarranty; not even for MERCHANTABILITY "
           "or FITNESS FOR A PARTICULAR PURPOSE.\n\n");
    printf("Written by Johannes Weißl.\n");
    exit(EXIT_SUCCESS);
}

static void cleanup(void)
{
#if USE_LOCKFILE
    chdir(Tmpdir);
    remove(LOCKFILE);
#endif /* USE_LOCKFILE */
}
