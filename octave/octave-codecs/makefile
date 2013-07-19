# Makefile for Octave/2 Image processing
# (c) Klaus Gebhardt, 1997

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

INSDIR = j:/apps2/science/octave-$(OCTVER)

MSTDIR  = $(SRCDIR)/octave/octave-$(OCTVER)


%.o   : %.cc ; $(CC) -s -c $< -o $@
%.o   : %.c  ; $(CC) -c $< -o $@
%.obj : %.cc ; $(CC) -s -c $< -o $@ -Zomf
%.obj : %.c  ; $(CC) -c $< -o $@ -Zomf

%.lib : %.def
	emximp -o $(patsubst %.def,%.imp,$<) $<
	emximp -o $@ $(patsubst %.def,%.imp,$<)
	@rm -f $(patsubst %.def,%.imp,$<)

OFLAGS = $(MC_OFLAGS)

IFLAGS = -I. -I$(MSTDIR)/dlfcn -I$(MSTDIR)/dlfcn/octave

LFLAGS = -L$(MSTDIR)/dlfcn/lib \
	-lcruft1 -lcruft2 -lcruft3 -lcruft4 -loctave1 -loctave2 \
	-lreadline -lgnuinfo -lkpathsea -lmerlin -lgpp2 -lstdcpp2 \
	-lsocket -lf2cdll -Zdll -Zomf -Zcrtdll

CFLAGS = $(OFLAGS) -DEMX09C -DOS2 

CC = gcc $(CFLAGS) $(IFLAGS)

OCTOBJS = oct-utils.obj
OBJECTS = utils.obj bitio.obj bmp.obj pnm.obj ps.obj

all: oct-img.dll oct-img.lib oct-view.exe

oct-img.dll: $(OCTOBJS) $(OBJECTS) oct-img.def
	$(CC) -o oct-img.dll oct-img.def $(OCTOBJS) $(OBJECTS) $(LFLAGS)

oct-img.def: $(OCTOBJS) oct-img.header
	cat oct-img.header > oct-img.def
	emxexp -o10000 $(OCTOBJS) >> oct-img.def

oct-view.exe: oct-view.c
	$(CC) -Wall -m486 -O2 -o oct-view.exe oct-view.c -Zcrtdll -Zomf

build: clean all

install: build
	cp *.lib $(OCTDIR)/dlfcn/lib
	cp oct-img.h $(OCTDIR)/dlfcn/octave
	cp *.dll $(OCTDIR)/dll
	cp oct-view.exe $(OCTDIR)

octave: build
	cp *.lib $(MSTDIR)/dlfcn/lib
	cp oct-img.h $(MSTDIR)/dlfcn/octave
	cp *.dll $(MSTDIR)/dll
	cp oct-view.exe $(MSTDIR)

clean:
	rm -f *.a *.lib *.o *.obj *.def *.dll *.exe
