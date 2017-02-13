xfreeze
=======

This tool completely locks up your display, using various hacks.
After starting, you cannot even use Ctrl-Alt-Fn or the SysRq keys (but you
can still use SSH to login and kill xfreeze).

To unlock the display, you just have to input your password, and press
ENTER. There is no graphical user interface.

If you use PAM (most distributions do), you have to e.g. create a
xfreeze entry in `/etc/pam.d`:

    cat > /etc/pam.d/xfreeze << "EOF"
    auth        required       pam_unix.so
    EOF

The xfreeze binary must have root privileges to do things like disabling
SysRq keys and disabling VT-switching. If you want to use xfreeze as user,
just set the s-bit, e.g.:

    chown root:root /usr/local/bin/xfreeze
    chmod 4755 /usr/local/bin/xfreeze

If you don't want any untested suid-root binarys on your system, you can
use the script and helper-programs in the extras/ subdir.
Check the c-files, and if you trust them, use them together with the script.


If you want xfreeze to be started after a certain idle time, use
xautolock (<http://www.ibiblio.org/pub/Linux/X11/screensavers/>).


For command-line options, try `xfreeze --help` or read the manpage.
