bin_PROGRAMS = xfreeze
xfreeze_SOURCES = \
    main.c        \
    lock.c        lock.h        \
    input.c       input.h       \
    opt.c         opt.h         \
    signals.c     signals.h
EXTRA_xfreeze_SOURCES = \
    auth_pam.c    auth_pam.h    \
    auth_unix.c   auth_unix.h   \
    auth_static.c auth_static.h
xfreeze_LDADD = @XFREEZE_AUTH@
xfreeze_DEPENDENCIES = @XFREEZE_AUTH@

AM_CFLAGS = @WARN_FLAGS@ @X_CFLAGS@
LIBS = @PAM_LIBS@ @X_LIBS@ @X_EXTRA_LIBS@ @X_CUSTOM_LIBS@ @X_PRE_LIBS@ @LIBS@
