# Makefile for Readline/History for OS/2
# Klaus Gebhardt, 1996

include ../../makeversions

TEX      = -ksh -c "cmd /c 'tex386 ^&plain3 \\nonstopmode\\input
TEXINDEX = texindex

RLSRC = rlman.tex rluser.tex rltech.tex
HISTSRC = hist.tex hsuser.tex hstech.tex

DVIOBJ = readline.dvi history.dvi
INFOOBJ = readline.info history.info
PSOBJ = readline.ps history.ps

all: info dvi

install: all
	cp *.info $(INFODIR)
	cp *.i?? $(INFODIR)

readline.dvi: $(RLSRC)
	$(TEX) rlman.tex'"
	$(TEXINDEX) rlman.??
	$(TEX) rlman.tex'"
	mv rlman.dvi readline.dvi

readline.info: $(RLSRC)
	makeinfo rlman.tex

history.dvi: ${HISTSRC}
	$(TEX) hist.tex'"
	$(TEXINDEX) hist.??
	$(TEX) hist.tex'"
	mv hist.dvi history.dvi

history.info: ${HISTSRC}
	makeinfo hist.tex

readline.ps: readline.dvi
	dvips -D 300 -o $@ readline.dvi

history.ps: history.dvi
	dvips -D 300 -o $@ history.dvi

info:	$(INFOOBJ)
dvi:	$(DVIOBJ)
ps:	$(PSOBJ)


distclean mostlyclean clean:
	rm -f *.aux *.cp *.fn *.ky *.log *.pg *.toc *.tp *.vr *.cps *.pgs \
	      *.fns *.kys *.tps *.vrs *.o core

maintainer-clean realclean:	clean
	rm -f *.dvi *.info *.i?? *.ps
