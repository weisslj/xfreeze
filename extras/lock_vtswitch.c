#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/vt.h>

#define DEV_CONSOLE "/dev/console"

int main(int argc, char *argv[])
{
    int lock, fd, success;

    success = 1;
    lock = (argc > 1 && argv && argv[0] && argv[1] &&
            argv[1][0] == 'u') ? 0 : 1;

    fd = open(DEV_CONSOLE, O_RDWR);
    if (fd < 0) {
        perror("open");
        success = 0;
    }

    if (fd >= 0 && ioctl(fd, (lock ? VT_LOCKSWITCH : VT_UNLOCKSWITCH)) < 0) {
        perror("ioctl");
        success = 0;
    }

    if (fd >= 0 && close(fd) < 0) {
        perror("close");
        success = 0;
    }

    return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}
