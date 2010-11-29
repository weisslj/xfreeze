#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#define SYSRQ_PROC "/proc/sys/kernel/sysrq"

int main(int argc, char *argv[])
{
    int lock, fd, success;

    success = 1;
    lock = (argc > 1 && argv && argv[0] && argv[1] &&
            argv[1][0] == 'u') ? 0 : 1;

    fd = open(SYSRQ_PROC, O_WRONLY);
    if (fd < 0) {
        perror("open");
        success = 0;
    }

    if (fd >= 0 && write(fd, (lock ? "0" : "1"), 1) != 1) {
        perror("write");
        success = 0;
    }

    if (fd >= 0 && close(fd) < 0) {
        perror("close");
        success = 0;
    }

    return (success) ? EXIT_SUCCESS : EXIT_FAILURE;
}
