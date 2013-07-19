#
# Makefile
#   Copyright (C) 1990, 1991, 1992 Free Software Foundation
#
# This file is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.  */
#

#
# Makefile for readline and history libraries.
#

# Modified by Klaus Gebhardt, 1995

include $(_PRJ_SRC_DIR)\\C\\emx-0.9c\\src\\.make\\makeconf

CC = gcc $(CFLAGS)

%.o : %.c ; $(CC) -c $< -o $@
%.obj : %.c ; $(CC) -c $< -o $@ -Zomf

%.imp : %.def ; emximp -o $@ $<
%.a : %.imp ; emximp -o $@ $<
%.lib : %.imp ; emximp -o $@ $<

READLINE_DEFINES = -DHAVE_CONFIG_H
OFLAGS = $(MC_OFLAGS)
DEBUG_FLAGS = $(OFLAGS) -mprobe -DOS2 -D__ST_MT_ERRNO__
INCL_FLAGS = -I. -I..
CFLAGS = $(DEBUG_FLAGS) $(LOCAL_INCLUDES) $(INCL_FLAGS) $(READLINE_DEFINES)
LIBS = -ltermcap
ZFLAGS = -Zdll -Zomf -Zcrtdll

CSOURCES = readline.c funmap.c keymaps.c vi_mode.c parens.c rltty.c \
           complete.c bind.c isearch.c display.c signals.c emacs_keymap.c \
           vi_keymap.c util.c kill.c undo.c macro.c input.c callback.c \
           terminal.c xmalloc.c history.c histsearch.c histexpand.c \
           histfile.c nls.c search.c setctype.c shell.c tilde.c

# The header files for this library.
HSOURCES = readline.h rldefs.h chardefs.h keymaps.h history.h histlib.h \
           posixstat.h posixdir.h posixjmp.h tilde.h rlconf.h rltty.h \
           ansi_stdlib.h tcap.h

HISTOBJ = history.o histexpand.o histfile.o histsearch.o shell.o
TILDEOBJ = tilde.o

EXEDEP = readline.c xmalloc.obj readline.lib

HISTOBJ  = history.obj histexpand.obj histfile.obj histsearch.obj shell.obj
TILDEOBJ = tilde.obj
OBJECTS  = readline.obj vi_mode.obj funmap.obj keymaps.obj parens.obj \
           search.obj rltty.obj complete.obj bind.obj isearch.obj \
           display.obj signals.obj util.obj kill.obj undo.obj macro.obj \
           input.obj callback.obj terminal.obj nls.obj xmalloc.obj \
           setctype.obj $(HISTOBJ) $(TILDEOBJ)

all: readline.a readline.lib readline.dll

readline.imp: readline.def
readline.a: readline.imp
readline.lib: readline.imp

readline.dll: readline.def $(OBJECTS)
	$(CC) $(MC_LFLAGS) -o readline.dll readline.def $(OBJECTS) $(LIBS) $(ZFLAGS)

install: all
	cp readline.a readline.lib $(LIBDIR)
	cp readline.h keymaps.h chardefs.h history.h tilde.h $(INCLDIR)/readline
	cd doc & make install & cd ..
	cp readline.dll $(DLLDIR)

bind.obj: ansi_stdlib.h posixstat.h rldefs.h config.h rlconf.h
bind.obj: readline.h keymaps.h chardefs.h tilde.h history.h
callback.obj: rlconf.h rldefs.h config.h rlconf.h
callback.obj: readline.h keymaps.h chardefs.h tilde.h
complete.obj: ansi_stdlib.h posixdir.h posixstat.h rldefs.h config.h rlconf.h
complete.obj: readline.h keymaps.h chardefs.h tilde.h
display.obj: ansi_stdlib.h posixstat.h rldefs.h config.h rlconf.h
display.obj: tcap.h readline.h keymaps.h chardefs.h tilde.h history.h
funmap.obj: readline.h keymaps.h chardefs.h tilde.h
funmap.obj: rlconf.h ansi_stdlib.h config.h
histexpand.obj: ansi_stdlib.h history.h histlib.h config.h
histfile.obj: ansi_stdlib.h history.h histlib.h config.h
history.obj: ansi_stdlib.h history.h histlib.h config.h
histsearch.obj: ansi_stdlib.h history.h histlib.h config.h
input.obj: ansi_stdlib.h rldefs.h config.h rlconf.h
input.obj: readline.h keymaps.h chardefs.h tilde.h
isearch.obj: rldefs.h config.h rlconf.h readline.h keymaps.h chardefs.h
isearch.obj: tilde.h ansi_stdlib.h history.h
keymaps.obj: emacs_keymap.c vi_keymap.c
keymaps.obj: keymaps.h chardefs.h rlconf.h ansi_stdlib.h
keymaps.obj: readline.h keymaps.h chardefs.h tilde.h config.h
kill.obj: ansi_stdlib.h rldefs.h config.h rlconf.h
kill.obj: readline.h keymaps.h chardefs.h tilde.h history.h
macro.obj: ansi_stdlib.h rldefs.h config.h rlconf.h
macro.obj: readline.h keymaps.h chardefs.h tilde.h history.h
nls.obj: ansi_stdlib.h rldefs.h config.h rlconf.h
parens.obj: rlconf.h config.h readline.h keymaps.h chardefs.h tilde.h
readline.obj: readline.h keymaps.h chardefs.h tilde.h cursor.h
readline.obj: rldefs.h config.h rlconf.h history.h
readline.obj: posixstat.h ansi_stdlib.h posixjmp.h
rltty.obj: rldefs.h config.h rlconf.h rltty.h
rltty.obj: readline.h keymaps.h chardefs.h tilde.h
search.obj: rldefs.h config.h rlconf.h
search.obj: readline.h keymaps.h chardefs.h tilde.h
search.obj: ansi_stdlib.h history.h
setctype.objbj: setctype.c cursor.h
signals.obj: rldefs.h config.h rlconf.h
signals.obj: readline.h keymaps.h chardefs.h tilde.h history.h
terminal.obj: rldefs.h config.h rlconf.h tcap.h
terminal.obj: readline.h keymaps.h chardefs.h tilde.h history.h
tilde.obj: ansi_stdlib.h config.h tilde.h
undo.obj: ansi_stdlib.h rldefs.h config.h rlconf.h
undo.obj: readline.h keymaps.h chardefs.h tilde.h history.h
util.obj: posixjmp.h ansi_stdlib.h rldefs.h config.h rlconf.h
util.obj: readline.h keymaps.h chardefs.h tilde.h
vi_mode.obj: rldefs.h config.h rlconf.h
vi_mode.obj: readline.h keymaps.h chardefs.h tilde.h
vi_mode.obj: history.h ansi_stdlib.h
xmalloc.obj: config.h ansi_stdlib.h

clean :
	rm -f *.a *.lib *.obj *.dll *.exe
