include ../makeconf

SOURCES = texindex.c

INCL = 

OBJECTS = texindex.obj

all: texindex.exe

#install: all
#	cp info.exe gnuview.exe h:/bin
#	cp gnuinfo.dll h:/bin/dll
#	cp gnuinfo.a gnuinfo.lib ../lib

clean:
	rm -f *.o *.obj *.a *.lib *.dll *.exe

# Dependencies
texindex.exe: texindex.obj
texindex.obj: texindex.c
