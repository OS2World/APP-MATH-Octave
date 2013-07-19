/* Build .OCT file from source */
/* (c) Klaus Gebhardt, 1997 */
'@setlocal'

parse arg ccfile libs ' /d' dir

call RxFuncAdd 'SysLoadFuncs', 'RexxUtil', 'SysLoadFuncs'
call SysLoadFuncs

octave_home = strip(dir);
if (octave_home == '') then
  do
    octave_home = strip(value('OCTAVE_HOME',,'OS2ENVIRONMENT'));
    if (octave_home == '') then octave_home = "h:/apps/science/octave-2.09";
  end

gccopt2 = '-m486 -mieee-fp -O3 -malign-loops=2 -malign-jumps=2 -malign-functions=2 -DEMX09C -DOS2';

lnkopt2 = '-lcruft1 -lcruft2 -lcruft3 -lcruft4 -loctave1 -loctave2 -lreadline -lgnuinfo -lkpathsea -lxmalloc -lgpp2 -lstdcpp2 -lsocket -ldlfcn -lf2cdll -loct-img' libs;


'@SET DELDIR='
'@SET C_INCLUDE_PATH='octave_home'/dlfcn/octave;'octave_home'/dlfcn;%C_INCLUDE_PATH%'
'@SET CPLUS_INCLUDE_PATH='octave_home'/dlfcn/octave;'octave_home'/dlfcn;%CPLUS_INCLUDE_PATH%'
'@SET LIBRARY_PATH='octave_home'/dlfcn/lib;%LIBRARY_PATH%'

if (ccfile <> '') then
  do
    len  = length(ccfile);
    name = substr(ccfile,1,len-3);
    ext  = substr(ccfile,len-2);
  end
if ((ext <> '.cc') & (ext <> '.CC')) then
  do
    say 'error: invalid argument'
    exit;
  end

'gcc' gccopt2 '-s -c' ccfile '-o' name || '.obj -Zomf'
'@echo LIBRARY' name || ' INITINSTANCE TERMINSTANCE>' name || '.def'
'@echo DESCRIPTION ' || d2c(34) || name || ', (c) Klaus Gebhardt 1995-1997' || d2c(34) || '>>' name || '.def'
'@echo.>>' name || '.def'
'@echo DATA>>' name || '.def'
'@echo   MULTIPLE NONSHARED>>' name || '.def'
'@echo.>>' name || '.def'
'@echo EXPORTS>>' name || '.def'
'emxexp' name || '.obj >> ' name || '.def'
'gcc' gccopt2 '-o' name || '.dll' name || '.def' name || '.obj' lnkopt2 '-Zdll -Zomf -Zcrtdll'
'@del' name || '.obj' name || '.def >NUL 2>NUL'

'@endlocal'
exit;
