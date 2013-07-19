/*
*******************************************************************************
** This file patches the original documentation to ensure it can be used
** under OS/2 (for compiling and viewing). The original documentation will
** be saved in the directory structure doc.orig.
** (c) Klaus Gebhardt, 1997
*******************************************************************************
*/

'@echo off'

DOCDIR = 'doc';

call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
call SysLoadFuncs

'SETLOCAL'
'SET DELDIR='

say 'info: creating backup ...'
'ren' DOCDIR DOCDIR'.orig'
'xcopy' DOCDIR'.orig' DOCDIR'\ /S /E >NUL'
'cd' DOCDIR

'rm -f ChangeLog *.in texinfo.tex'
call patch

'cd faq'
'rm -f *.in *.dvi *.ps *.info*'
'ren Octave-faq.* oct-faq.*'
'mv oct-faq.texi oct-faq.texi.orig'
'sed -e "s/Octave-FAQ\.info/oct-faq\./g" oct-faq.texi.orig > oct-faq.texi'
'rm -f oct-faq.texi.orig'
call patch
'cd ..'

'cd interpreter'
'rm -f dir *.in *.dvi *.ps *.info*'
'mv octave.texi octave.texi.orig'
'sed -e "s/octave\.info/octave\./g" octave.texi.orig > octave.texi'
'rm -f octave.texi.orig'
call patch
'cd ..'

'cd liboctave'
'rm -f *.in *.dvi *.ps *.info*'
'ren liboctave.* liboct.*'
'mv liboct.texi liboct.texi.orig'
'sed -e "s/liboctave\.info/liboct\./g" liboct.texi.orig > liboct.texi'
'rm -f liboct.texi.orig'
call patch
'cd ..'

'cd refcard'
'rm -f *.in'
'ren refcard.* rfc.*'
'ren refcard-a4.* rfc-a4.*'
'ren refcard-legal.* rfc-leg.*'
'ren refcard-letter.* rfc-let.*'
'cd ..'

'cd ..'

'ENDLOCAL'
exit

patch: procedure
rc = SysFileTree('*.texi', 'texinfo', 'FO');
if (rc == 0) then
  do
    do i=1 to texinfo.0
      say 'info: patching' texinfo.i '...'
      file = FILESPEC('name', texinfo.i);
      'mv' file file'.orig'
      'sed -e "s/\.texi/\.tex/g"' file'.orig >' file
      'rm -f' file'.orig'
    end
    'ren *.texi *.tex'
  end
else
  do
    say 'error: Not enough memory.'
    exit;
  end
return;
