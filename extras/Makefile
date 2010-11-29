CC=gcc
WARN_FLAGS=-ansi -pedantic -W -Wall
CFLAGS=-static $(WARN_FLAGS)

all: lock_vtswitch lock_sysrq

lock_vtswitch: lock_vtswitch.c
	$(CC) $(CFLAGS) -o lock_vtswitch lock_vtswitch.c

lock_sysrq: lock_sysrq.c
	$(CC) $(CFLAGS) -o lock_sysrq lock_sysrq.c

clean:
	rm -f lock_vtswitch lock_sysrq *.o
