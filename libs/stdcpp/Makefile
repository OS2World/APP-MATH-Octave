# Copyright (C) 1995 by Klaus Gebhardt

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

VERSION = 0.3

LIBDIR = $(EMXDIR)/lib
INCDIR = $(EMXDIR)/include/cpp
DLLDIR = $(EMXDIR)/dll

AR = ar
AR_FLAGS = rc
RANLIB = ar s

CC = gcc
CFLAGS = $(MC_OFLAGS) -D__ST_MT_ERRNO__ -I.

%.o: %.c;	$(CC) -c $(CFLAGS) $< -o $@
%.o: %.cc;	$(CC) -c $(CFLAGS) $< -o $@
%.lib: %.a;	emxomf -s -l $<

%.lib : %.def
	emximp -o $(patsubst %.def,%.imp,$<) $<
	emximp -o $@ $(patsubst %.def,%.imp,$<)
	@rm -f $(patsubst %.def,%.imp,$<)

%.a   : %.def
	emximp -o $(patsubst %.def,%.imp,$<) $<
	emximp -o $@ $(patsubst %.def,%.imp,$<)
	@rm -f $(patsubst %.def,%.imp,$<)

OBJECTS = iosystem.o iopopen.o filebuf.o pfstream.o procbuf.o procstream.o

LIBS = stdcpp.a stdcpp.lib stdcpp2.a stdcpp2.lib gpp2.a gpp2.lib

DLLS = stdcpp2.dll gpp2.dll

INCS = procbuf.h procstream.h

all: $(LIBS) $(DLLS)

stdcpp.a: $(OBJECTS)
	cp stdcpp.emx stdcpp.a
	$(AR) $(AR_FLAGS) stdcpp.a $(OBJECTS)

stdcpp2.def: stdcpp.lib
	echo "LIBRARY stdcpp2 INITINSTANCE TERMINSTANCE" > $@
	echo "DESCRIPTION 'stdcpp2.dll, (c) Klaus Gebhardt 1995-1996'" >> $@
	echo "" >> $@
	echo "DATA" >> $@
	echo "  MULTIPLE NONSHARED" >> $@
	echo "">> $@
	echo "EXPORTS" >> $@
	emxexp -o1 stdcpp.lib >> $@

gpp.lib: $(LIBDIR)/gpp.lib
	cp $(LIBDIR)/gpp.lib gpp.lib

gpp2.def: gpp.lib
	echo "LIBRARY gpp2 INITINSTANCE TERMINSTANCE" > $@
	echo "DESCRIPTION 'gpp2.dll, (c) Klaus Gebhardt 1995-1996'" >> $@
	echo "" >> $@
	echo "DATA" >> $@
	echo "  MULTIPLE NONSHARED" >> $@
	echo "" >> $@
	echo "EXPORTS" >> $@
	emxexp -o1 gpp.lib >> $@

stdcpp2.dll: stdcpp2.def stdcpp.lib
	$(CC) -o $@ stdcpp2.def stdcpp.lib -Zdll -Zomf -Zcrtdll

gpp2.dll: gpp2.def gpp.lib stdcpp2.lib
	$(CC) -o $@ gpp2.def gpp.lib stdcpp2.lib -Zdll -Zomf -Zcrtdll

install: all
	cp $(LIBS) $(LIBDIR)
	cp $(INCS) $(INCDIR)
	cp $(DLLS) $(DLLDIR)

install_new:
	rm -f *.o *.a *.lib
	cp $(INCS) $(INCDIR)
	cp $(LIBDIR)/stdcpp.a stdcpp.emx
	cp $(LIBDIR)/gpp.lib gpp.lib
	make install

clean:
	rm -f *.o *.a *.lib *.def *.dll
