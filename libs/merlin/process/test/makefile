CFLAGS=-Wall -O2 -m486 -Zomf
CC=gcc $(CFLAGS)

LFLAGS=-Zsys


all: dlltest.exe


dlltest.exe: dlltest.def dlltest.obj
	$(CC) -o dlltest.exe dlltest.def dlltest.obj -lmerlin $(LFLAGS)

dlltest.obj: dlltest.c
	$(CC) -o dlltest.obj -c dlltest.c

clean:
	rm -f *.exe *.o *.obj
