# Makefile for mkfifo for OS/2
# (c) Klaus Gebhardt, 1997

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

CFLAGS = -D__ST_MT_ERRNO__
OFLAGS = $(MC_OFLAGS)
CC     = gcc $(CFLAGS) $(OFLAGS)

AR = ar
ARFLAGS = r
RANLIB = $(AR) s

all:  nmpserv.exe mkfifo.exe

install: all
	mkfifo -x
	cp *.exe     $(BINDIR)

clean:
	rm -f *.exe *.obj *.o *.a *.lib


getopt.obj: getopt.c getopt.h
	$(CC) -DEMX -include ./config.emx -c $< -o $@ -Zomf

getopt1.obj: getopt1.c getopt.h
	$(CC) -DEMX -include ./config.emx -c $< -o $@ -Zomf

mkfifo.obj: mkfifo.c getopt.h
	$(CC) -c $< -o $@ -Zomf

mkfifo.exe: mkfifo.obj getopt.obj getopt1.obj
	$(CC) $(MC_LFLAGS) -o $@ $^ -lmerlin -Zomf


nmp_thread.obj: nmp_thread.c nmp.h
	$(CC) -c $< -o $@ -Zomf -Zmt

nmpserv.obj: nmpserv.c nmp.h
	$(CC) -c $< -o $@ -Zomf -Zmt

nmpserv.exe: nmpserv.obj nmp_thread.obj
	$(CC) $(MC_LFLAGS) -o $@ $^ -Zomf -Zmt
