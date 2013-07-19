# Makefile: f2c for os2
# (c) Klaus Gebhardt, 1997

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

CC = gcc

all: f2cdll.a f2cdll.lib
	cd src    ; $(MAKE) all ; cd ..
	cd libF77 ; $(MAKE) all ; cd ..
	cd libI77 ; $(MAKE) all ; cd ..
	$(CC) $(MC_LFLAGS) -o f2c.dll f2c.def libF77/F77.lib libI77/I77.lib -Zdll -Zomf -Zcrtdll

clean:
	cd src    ; $(MAKE) clean ; cd ..
	cd libF77 ; $(MAKE) clean ; cd ..
	cd libI77 ; $(MAKE) clean ; cd ..
	rm -f *.a *.lib *.dll *.exe

install: all
	cp src/f2c.h $(INCLDIR)
	cp src/f2c.exe $(BINDIR)
	cp f2cdll.a f2cdll.lib $(LIBDIR)
	cp f2c.dll $(DLLDIR)

f2c.imp: f2c.def
	emximp -o f2c.imp f2c.def

f2cdll.a: f2c.imp
	emximp -o f2cdll.a f2c.imp

f2cdll.lib: f2c.imp
	emximp -o f2cdll.lib f2c.imp
