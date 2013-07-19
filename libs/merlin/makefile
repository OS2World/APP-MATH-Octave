include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makerules

MAKE     = make
MAKE_SUB = make -f ../makefile-subdirs

CC     = gcc
OFLAGS = $(MC_OFLAGS)
CFLAGS = -Wall $(OFLAGS) -mprobe -D__ST_MT_ERRNO__ -Zmt

DIRS = math memory process std vio

LIBS = $(wildcard _*.lib)

all: merlin.a merlin.lib
	@for DIR in $(DIRS) ; do cd $$DIR ; $(MAKE_SUB)                    ; cd .. ; done
	@for DIR in $(DIRS) ; do cd $$DIR ; $(MAKE_SUB) DLLNAME=_$$DIR lib ; cd .. ; done
	$(MAKE) merlin.dll

build: clean all

clean:
	@for DIR in $(DIRS) ; do cd $$DIR ; $(MAKE_SUB) clean ; cd .. ; done
	rm -f *.imp *.dll *.a *.lib

install: all
	cp merlin.h            $(INCLDIR)
	cp merlin.a merlin.lib $(LIBDIR)
	cp merlin.dll          $(DLLDIR)

merlin.imp: merlin.def
merlin.a: merlin.imp
merlin.lib: merlin.imp

merlin.dll: merlin.def $(LIBS)
	$(CC) $(MC_LFLAGS) -o $@ $^ -Zdll -Zomf -Zmt -Zcrtdll
