CFLAGS = -O -I../../
CC = gcc

%.o : %.cc ; $(CC) $(CFLAGS) -c $< -o $@
%.o : %.c ; $(CC) $(CFLAGS) -c $< -o $@

all: fileman.exe

fileman.exe: fileman.o ../readline.a
	$(CC) -o fileman.exe fileman.o ../readline.a -lxmalloc -Zcrtdll

fileman.o: fileman.c
