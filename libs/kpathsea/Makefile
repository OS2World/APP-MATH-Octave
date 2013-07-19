# Generated automatically from Makefile.in by configure.
# Makefile for kpathsea --karl@cs.umb.edu.
# Modified by Klaus Gebhardt, 1995

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

version = 3.0
lib = kpathsea.a kpathsea.lib
dll = kpathsea.dll

CC = gcc
OFLAGS = $(MC_OFLAGS)
CFLAGS = -Wall $(OFLAGS) -mprobe -D__ST_MT_ERRNO__ -I. -I..

%.obj: %.c
	$(CC) $(CFLAGS) -c $< -o $@ -Zomf

headers = c-proto.h default.h expand.h pathsearch.h progname.h str-llist.h types.h

tex-obj = tex-file.obj tex-glyph.obj tex-hush.obj tex-make.obj

objects = tex-file.obj absolute.obj atou.obj basename.obj cnf.obj concat.obj \
	concat3.obj concatn.obj db.obj debug.obj dir.obj elt-dirs.obj \
	expand.obj extend-fname.obj file-p.obj find-suffix.obj fn.obj \
	fontmap.obj hash.obj kdefault.obj \
	line.obj magstep.obj make-suffix.obj path-elt.obj pathsearch.obj \
	proginit.obj progname.obj putenv.obj readable.obj rm-suffix.obj \
	str-list.obj str-llist.obj strcasecmp.obj strstr.obj strtol.obj \
	tex-glyph.obj tex-hush.obj tex-make.obj tilde.obj \
	truncate.obj uppercasify.obj variable.obj version.obj xcalloc.obj \
	xfopen.obj xfseek.obj xftell.obj xgetcwd.obj xmalloc.obj \
	xopendir.obj xputenv.obj xrealloc.obj xstat.obj xstrdup.obj

all: $(lib) $(dll)

kpathsea.imp: kpathsea.def
	emximp -o kpathsea.imp kpathsea.def

kpathsea.a: kpathsea.imp
	emximp -o kpathsea.a kpathsea.imp

kpathsea.lib: kpathsea.imp
	emximp -o kpathsea.lib kpathsea.imp

kpathsea.dll: kpathsea.def $(objects)
	$(CC) $(MC_LFLAGS) -o kpathsea.dll kpathsea.def $(objects) -Zdll -Zomf -Zcrtdll

install: all
	cp $(headers) $(INCLDIR)/kpathsea
	cp $(lib) $(LIBDIR)
	cp $(dll) $(DLLDIR)
	cp doc/*.info* $(INFODIR)

clean:
	rm -f *.o *.obj *.a *.lib *.dll *.exe
