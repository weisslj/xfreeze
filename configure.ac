#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(xfreeze, 0.1.0, jargon@molb.org)
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADER([config.h])
AM_INIT_AUTOMAKE

# Checks for programs.
AC_PROG_CC
if test "$GCC" = yes ; then
    WARN_FLAGS="-Wall"
else
    WARN_FLAGS=""
fi
AC_SUBST(WARN_FLAGS)
AC_PROG_INSTALL
AC_PROG_RANLIB

# Checks for header files.
AC_CHECK_HEADERS([fcntl.h locale.h shadow.h sys/ioctl.h sys/vt.h unistd.h features.h])

# Check for lock-file
use_lockfile=yes
AC_ARG_ENABLE(lockfile, AC_HELP_STRING([--disable-lockfile],
              [prevents from starting twice (default: yes)]),
              [use_lockfile=enableval])
if test "$use_lockfile" != no ; then
    AC_CHECK_FUNCS([kill getpid chdir], [], [use_lockfile=no])
    if test "$use_lockfile" != no ; then
        AC_DEFINE_UNQUOTED(USE_LOCKFILE, 1, [Define to 1 to enable lock-file.])
    fi
fi

# Checks for X
AC_PATH_XTRA
if test "$no_x" = yes ; then
    AC_MSG_ERROR([This program doesn't make much sense without X.])
fi
X_CUSTOM_LIBS="-lXext -lX11"
lock_mode_switch=yes
set_grab_keys_state=yes
AC_CHECK_LIB(Xxf86vm, XF86VidModeLockModeSwitch,
             [X_CUSTOM_LIBS="-lXxf86vm $X_CUSTOM_LIBS"],
             [lock_mode_switch=no],
             [$X_LIBS $X_EXTRA_LIBS $X_CUSTOM_LIBS $X_PRE_LIBS])
AC_CHECK_LIB(Xxf86misc, XF86MiscSetGrabKeysState,
             [X_CUSTOM_LIBS="-lXxf86misc $X_CUSTOM_LIBS"],
             [set_grab_keys_state=no],
             [$X_LIBS $X_EXTRA_LIBS $X_CUSTOM_LIBS $X_PRE_LIBS])
if test "$lock_mode_switch" = yes; then
    AC_DEFINE_UNQUOTED(HAVE_LOCKMODESWITCH, 1,
        [Define to 1 if you have the XF86VidModeLockModeSwitch function.])
fi
if test "$set_grab_keys_state" = yes ; then
    AC_DEFINE_UNQUOTED(HAVE_SETGRABKEYSSTATE, 1,
        [Define to 1 if you have the XF86MiscSetGrabKeysState function.])
fi
AC_SUBST(X_CUSTOM_LIBS)

XFREEZE_AUTH=""

# Checks for PAM authentication
use_auth_pam=yes
AC_ARG_WITH(pam, AC_HELP_STRING([--without-pam], [use PAM (default: yes)]),
            [use_auth_pam=withval])
if test "$use_auth_pam" = yes ; then
    AC_CHECK_FUNCS([getuid getpwuid], [], [use_auth_pam=no])
    AC_CHECK_HEADERS([security/pam_appl.h], [], [use_auth_pam=no])
    AC_CHECK_LIB(pam, pam_start, :, [use_auth_pam=no], [-ldl])
    if test "$use_auth_pam" = yes ; then
        PAM_LIBS="-lpam -ldl"
        AC_SUBST(PAM_LIBS)
        AC_DEFINE_UNQUOTED(USE_AUTH_PAM, 1,
            [Define to 1 if you want to use PAM for authentication.])
        XFREEZE_AUTH="$XFREEZE_AUTH auth_pam.\$(OBJEXT)"
    fi
else
    use_auth_pam=no
    AC_MSG_NOTICE([PAM support disabled])
fi

# Checks for crypt
have_crypt=yes
AC_CHECK_LIB(crypt, crypt, [], [have_crypt=no])
AC_CHECK_FUNCS(crypt, [], [have_crypt=no])

# Checks for UNIX authentication
use_auth_unix=no
if test "$use_auth_pam" = no ; then
    if test "$have_crypt" = yes ; then
        use_auth_unix=yes
        AC_CHECK_FUNCS([getuid getpwuid], [], [use_auth_unix=no])
        if test "$use_auth_unix" = yes ; then
            AC_DEFINE_UNQUOTED(USE_AUTH_UNIX, 1,
                [Define to 1 if you want to use UNIX authentication.])
            XFREEZE_AUTH="$XFREEZE_AUTH auth_unix.\$(OBJEXT)"
        fi
    fi
fi

# Checks for static authentication
use_auth_static=yes
if test "$have_crypt" = no ; then
    use_auth_static=no
else
    AC_DEFINE_UNQUOTED(USE_AUTH_STATIC, 1,
        [Define to 1 if you want to use static authentication.])
    XFREEZE_AUTH="$XFREEZE_AUTH auth_static.\$(OBJEXT)"
fi

if test -z "$XFREEZE_AUTH" ; then
    AC_MSG_ERROR([No authentication mechanism available.])
fi

AC_SUBST([XFREEZE_AUTH])

# Checks for libraries.

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_SIZE_T
AC_TYPE_UID_T
AC_TYPE_PID_T
AC_TYPE_SIGNAL
AC_PROG_GCC_TRADITIONAL

# Checks for library functions.
AC_CHECK_FUNCS([getpass wctomb ioctl sleep setlocale])

AC_CONFIG_FILES([Makefile doc/Makefile src/Makefile])

AC_OUTPUT

echo
echo "summary"
echo
echo "use pam      : $use_auth_pam"
echo "use lockfile : $use_lockfile"
echo
echo "PLEASE READ THE README-FILE CAREFULLY BEFORE STARTING XFREEZE"
echo
