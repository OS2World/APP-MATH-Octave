/* Converting all files to FAT name rules */
/* (c) K. Gebhardt, 1997 - 1999 */

'@echo off'

call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
call SysLoadFuncs

'SETLOCAL'
'SET DELDIR='


/* The sed script ... */
fatsed = directory() || "\fat.sed";


/* Load the file fat.sed ... */
call LoadFatSed


/* Find all sub directories ... */
call SysFileTree "*",subdirs,'DSO'

/* Rename the directories ... */
call MaybeRenameDirs


/* Find all sub directories and patch the files. */
call SysFileTree "*",subdirs,'DSO'

call PatchScriptFiles "."

do i = 1 to subdirs.0
  call PatchScriptFiles subdirs.i
end

exit



LoadFatSed: procedure expose fatsed oldname. newname.

rc = stream(fatsed,'C','open read');

if (rc <> 'READY:') then
  do
    rc = stream(fatsed,'C','open read');

    if (rc <> 'READY:') then
      do
	say 'error: Cannot open' fatsed || '!'
	exit;
      end
  end

cnt = 0;

do while(lines(fatsed))
  line = linein(fatsed);

  if (line <> '') then
    do
      parse var line 's/' old '/' new '/g'

      cnt = cnt+1;
      oldname.cnt = old;
      newname.cnt = new;
    end
end

oldname.0 = cnt;
newname.0 = cnt;

rc = stream(fatsed,'C','close');
return;



FindName: procedure expose oldname.
parse arg name

do i = 1 to oldname.0
  if (filespec('n',name) == oldname.i) then return i;
end

return 0;



DirDepth: procedure
parse arg dirname

depth = 0;

p = pos("\",dirname);

do while (p > 0)
  depth = depth+1;
  p = pos("\",dirname,p+1);
end

return depth;



MaybeRenameDirs: procedure expose subdirs. oldname. newname.

maxDepth = 0;

do i = 1 to subdirs.0
  depth.i = DirDepth(subdirs.i)
  if (depth.i > maxDepth) then maxDepth = depth.i
end

do j = maxDepth to 0 by -1
  do i = 1 to subdirs.0
    if (depth.i == j) then
      do
	p = FindName(subdirs.i);

	if (p > 0) then
	  do
	    say 'info: renaming' subdirs.i 'to' newname.p
	    'ren' subdirs.i newname.p
	  end
      end
  end
end

return;



PatchScriptFiles: procedure expose fatsed oldname. newname.
parse arg dirname

say '*** Entering' dirname
olddir = directory(dirname);

call SysFileTree "*.m",scripts,"FO"

do i = 1 to scripts.0
  scrName = filespec("n",scripts.i);
  octName = substr(scripts.i,1,length(scripts.i)-2);

  p = FindName(octname);

  if (p == 0) then
    do
      tmpname = scrName || ".tmp";

      say 'info: patching' scrName
      'ren' scrName tmpname
      'sed -f' fatsed tmpname '>' scrName
      'del' tmpname '>NUL'
    end
  else
    do
      say 'info: patching' scrName 'and renaming to' newname.p || ".m"
      'sed -f' fatsed scrName '>' newname.p || ".m"
      'del' scrName '>NUL'
    end
end

say '*** Leaving' dirname
call directory olddir
return
