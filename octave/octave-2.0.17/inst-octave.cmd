/*
*******************************************************************************
** Install and Setup script for Octave/2 Version 0.6                         **
** (c) Klaus Gebhardt, 1996 - 1997                                           **
*******************************************************************************
*/

/*
*******************************************************************************
** This script will make all the necessary changes in the following files:   **
**                                                                           **
**   1. CONFIG.SYS                                                           **
**   2. .emacs                                                               **
**   3. .octaverc                                                            **
**     (or the file pointed to by the environment variable OCTAVE_INITFILE)  **
**   4. %INFOPATH%dir                                                        **
*******************************************************************************
** It will also copy the info files to the directoy pointed to by the        **
** variable INFOPATH, and it replaces emx.dll, emxlibcs.dll and              **
** termcap.dat, if the files coming with Octave/2 are newer than those       **
** on your system.                                                           **
*******************************************************************************
** This script also creates a folder with a program object for Octave/2      **
** on the WPS.                                                               **
*******************************************************************************
** ALL ORIGINAL FILES, WHICH ARE MODIFIED OR REPLACED BY THIS SCRIPT         **
** ARE BACKUPED IN THE DIRECTORY YOU HAVE OCTAVE/2 INSTALLED IN.             **
*******************************************************************************
** NO WARRANTY!                                                              **
*******************************************************************************
*/

"@echo off"
debug = ">NUL 2>NUL"
debug_mode = 0;

version     = "2.0.17";
script_arc  = "SCRIPTS.ZIP";
dlfcn_arc   = "DLFCN.ZIP";

default_dir.0 = 3;
default_dir.1 = "j:/apps2/science/octave";
default_dir.2 = "h:/apps/science/octave-";
default_dir.3 = "i:/apps/octave-";

config_modified   = 0;
config.0.nr =  9;
config.1.nr =  8; config.1.name = "LIBPATH=";             config.1.zeile = "";
config.2.nr =  9; config.2.name = "SET PATH=";            config.2.zeile = "";
config.3.nr = 16; config.3.name = "SET OCTAVE_HOME=";     config.3.zeile = "";
config.4.nr = 12; config.4.name = "SET TERMCAP=";         config.4.zeile = "";
config.5.nr =  9; config.5.name = "SET TERM=";            config.5.zeile = "";
config.6.nr =  9; config.6.name = "SET HOME=";            config.6.zeile = "";
config.7.nr = 20; config.7.name = "SET OCTAVE_INITFILE="; config.7.zeile = "";
config.8.nr = 13; config.8.name = "SET INFOPATH=";        config.8.zeile = "";
config.9.nr = 12; config.9.name = "SET GNUPLOT=";         config.9.zeile = "";

emacs_modified = 0;
octaverc_modified = 0;
dir_modified = 0;


call RxFuncAdd "SysLoadFuncs", "RexxUtil", "SysLoadFuncs"
call SysLoadFuncs

parse upper arg option cmdl

/*
*******************************************************************************
** Debug-Mode                                                                **
*******************************************************************************
*/
if (option == "/DEBUG") then
  do
    say "info: Running in DEBUG mode!";
    "@echo on"
    debug = ""
    debug_mode = 1;
  end
else
  do
    debug_mode = 0;
    cmdl = option;
  end

/*
*******************************************************************************
** Removing unnecessary files                                                **
*******************************************************************************
*/
if (cmdl == "/CLEAN") then
  do
    say "info: Removing all for running Octave/2" version "unnecessary files";
    "del Change*.* *.zip COPY*.* BUGS NEWS NEWS.1 PROJECTS README.*" debug
    "del ROADMAP SENDING.PAT THANKS" debug
    "del updt-oct*.cmd *.old *.new emx* unzip.exe" debug
    exit;
  end

/*
*******************************************************************************
** Wrong argument and usage message                                          **
*******************************************************************************
*/
if ((cmdl <> "/INSTALL") & (cmdl <> "/USAGE") & (cmdl <> "")) then
  do
    say "error: Unknown command line option!";
    say "";
    cmdl = "";
  end

if ((cmdl == "/USAGE") | (cmdl == "")) then
  do
    say "Usage:"
    say "  Type 'inst-octave /install' to install Octave/2" version;
    say "  Type 'inst-octave /usage'   to see this message.";
    say "  Type 'inst-octave /clean'   to remove unnecessary files.";
    say "on FAT systems you must type 'inst-oct' instead of 'inst-octave'";
    exit;
  end

/*
*******************************************************************************
** Installation or Uodating the installation                                 **
*******************************************************************************
*/
say "info: Installing Octave/2" version "..."

boot_drive = get_boot_drive();
say "info: OS/2 booted from" boot_drive;

octave_home = to_unix_sep(directory());
octave_dll = octave_home || "/dll";
if (check_octave_files(octave_home, 1) == 0) then
  do
    say "error: Run this script from within in the directory octave is"
    say "error: installed in! If you are installing from the sources, you"
    say "error: must compile Octave" version "before running this script!"
    say "error: To do so, type: make -f makefile.os2"
    exit;
  end
say "info: Octave/2 is installed in" octave_home || ".";

/* Analyse the CONFIG.SYS */
octave_old_home = read_config_sys(boot_drive, octave_home);
if (octave_old_home == "") then
  do
    say "info: There is no OCTAVE_HOME in your config.sys and also no";
    say "info: entry in LIBPATH and SET PATH corresponding to an";
    say "info: Octave/2 directory: This is the first installation of";
    say "info: Octave/2 on this system.";
    octave_old_dll = "";
  end
else
  do
    say "info: I found an old version of Octave/2 in your config.sys.";
    say "info: This version was installed in:" octave_old_home;
    say "info: I will remove all entries pointing to that version!"
    octave_old_dll = octave_old_home || "/dll";
  end

/* Replace older DLLs coming with EMX, if necessary */
call emx_dlls config.1.zeile, "emx.dll"
call emx_dlls config.1.zeile, "emxlibcs.dll"

/* Modify LIBPATH, PATH and OCTAVE_HOME */
if to_upper(octave_old_home) <> to_upper(octave_home) then
  do
    config_modified = 1;
    config.1.zeile = subst_paths(config.1.zeile, octave_old_dll,  octave_dll);
    config.2.zeile = subst_paths(config.2.zeile, octave_old_home, octave_home);
  end

if (to_upper(octave_home) <> to_upper(default_dir.1)) then
  do
    if (to_upper(config.3.zeile) <> to_upper(octave_home)) then
      do
        config_modified = 1;
        config.3.zeile  = octave_home;
      end
  end
else
  do
    if (to_upper(config.3.zeile) <> "") then
      do
        config_modified = 1;
        config.3.zeile  = "";
      end
  end

/* Modify TERMCAP, TERM and move TERMCAP.DAT */
if (config.4.zeile <> "") then
  do
    if ((octave_old_home == "") | (to_upper(octave_old_home || "/etc/termcap.dat") <> to_upper(config.4.zeile))) then
      do
        config.4.zeile = emx_termcap(config.4.zeile);
      end
    else config.4.zeile = "";
  end

if (config.4.zeile == "") then
  do
    config.4.zeile  = octave_home || "/etc/termcap.dat";
    config_modified = 1;
  end

if (config.5.zeile == "") then
  do
    config.5.zeile  = "ansi";
    config_modified = 1;
  end

/* Set the thie home directory to the root of your boot drive. */
if (config.6.zeile == "") then
  do
    config.6.zeile  = boot_drive || "/.";
    config_modified = 1;
  end

/* Create or modify .octaverc */
octave_rc = update_octaverc(config.6.zeile, config.7.zeile, octave_old_home, octave_home);
if (to_upper(octave_rc) <> to_upper(config.7.zeile)) then
  do
    config.7.zeile = octave_rc;
    config_modified = 1;
  end

/* Modify INFO_PATH */
if (config.8.zeile == "") then
  do
    config.8.zeile  = ".;" || octave_home || "/doc";
    config_modified = 1;
  end
else
  do
    if (pos(";", config.8.zeile) == 0) then
      do
        len_info_path = length(config.8.zeile);
        if (substr(config.8.zeile, len_info_path, 1) == "/") then
          do
            old_info_path = substr(config.8.zeile, 1, len_info_path-1);
          end
        else
          do
            old_info_path = config.8.zeile;
          end
        config.8.zeile = ".;" || old_info_path;
        config_modified = 1;
      end
    info_path = to_unix_sep(subst_paths(config.8.zeile, octave_old_home || "/doc", octave_home || "/doc"));
    call info_path_dir info_path, octave_home, version
  if (to_upper(info_path) <> to_upper(config.8.zeile)) then
    do
      config.8.zeile = info_path;
      config_modified = 1;
    end
  end

/* Modify or Add the variable GNUPLOT */
if (config.9.zeile == "") then
  do
    fullgplt = SysSearchPath(PATH, "gnuplot.exe");
    if (fullgplt <> "") then
      do
        pathgplt = filespec("drive", fullgplt) || filespec("path", fullgplt);
        gplt = to_unix_sep(substr(pathgplt, 1, length(pathgplt)-1));
        if (gplt <> config.9.zeile) then
          do
            config.9.zeile = gplt;
            config_modified = 1;
          end
      end
  end

/* Unzip the files for dynamic loading */
call unzip_dlfcn_files dlfcn_arc

/* Unzip the script files */
call unzip_script_files script_arc

/* Install on the WPS */
if (debug_mode == 0) then call create_wps_object octave_home, version

rc = ini_files(config.6.zeile, ".emacs", "emacs", octave_old_home, octave_home);
if (rc == 2) then
  do
    if (debug_mode == 0) then
      do
        "copy emacs.new" to_os2_sep(config.6.zeile) || "\.emacs" debug
      end
    say "info: EMACS MUST BE RESTARTED!"
  end

if (config_modified == 1) then
  do
    call write_config_new boot_drive, octave_home
    if (debug_mode == 0) then
      do
        "copy config.new" boot_drive || "\config.sys" debug
      end
    say "info: YOU MUST REBOOT YOUR SYSTEM!";
  end

say "info: Done.";
exit;

/*
*******************************************************************************
** Write the modified config.sys                                             **
*******************************************************************************
*/
write_config_new: procedure expose config. debug version
parse arg boot, dir
  config_old = to_os2_sep(dir) || "\config.old";
  config_new = to_os2_sep(dir) || "\config.new";
  "del" config_new debug

  rc = stream(config_old, "C", "open read");
  if rc <> "READY:" then
    do
      say "error: Cannot open the backup of CONFIG.SYS!";
      exit;
    end

  rc = stream(config_new, "C", "open write");
  if rc <> "READY:" then
    do
      say "error: Cannot open CONFIG.NEW!";
      rc = stream(config_old, "C", "close");
      exit;
    end

  say "info: Writing" config_new "...";
  do while(lines(config_old))
    line = linein(config_old);
    do i=1 to config.0.nr
      if (to_upper(substr(strip(line), 1, config.i.nr)) == config.i.name) then
        do
          if config.i.zeile <> "" then
            do
              p = pos(to_upper(config.i.name), to_upper(config.i.name));
              if (p <= 1) then
                line = config.i.name || config.i.zeile;
              else
                line = substr(" ",1,p," ") || config.i.name || config.i.zeile;
              config.i.zeile = "";
            end
          leave;
        end
    end
    call lineout config_new, line
  end

  sep = 0;
  do i=1 to config.0.nr
    if (config.i.zeile <> "") then
      do
        if (sep == 0) then
          do
            call lineout config_new, ""
            call lineout config_new, "REM Octave/2" version
            sep = 1;
          end
        call lineout config_new, config.i.name || config.i.zeile;
        config.i.zeile = "";
      end
  end

  rc = stream(config_new, "C", "close");
  rc = stream(config_old, "C", "close");
  return;

/*
*******************************************************************************
** Analysing CONFIG.SYS:                                                     **
*******************************************************************************
*/
read_config_sys: procedure expose config. default_dir. debug
parse arg boot, dir
  config_old = to_os2_sep(dir) || "\config.old";
  say "info: Copying" boot || "\config.sys to" config_old "...";
  "copy" boot || "\config.sys" config_old debug

  rc = stream(config_old, "C", "open read");
  if (rc <> "READY:") then
    do
      say "error: Cannot open the backup of CONFIG.SYS!";
      exit;
    end

  say "info: Analysing" config_old "...";
  do while(lines(config_old))
    line  = strip(linein(config_old));
    do i=1 to config.0.nr
      strupper = to_upper(substr(line, 1, config.i.nr));
      if (strupper == config.i.name) then
        do
          config.i.zeile = substr(line, config.i.nr+1);
          leave;
        end
    end
  end
  rc = stream(config_old, "C", "close");

  if (config.1.zeile == "") then
    do
      say "error: No "LIBPATH" statement found!";
      exit;
    end
  else libpath = config.1.zeile;

  if (config.2.zeile == "") then
    do
      say "error: No "SET PATH" statement found!";
      exit;
    end
  else path = config.2.zeile;

  oh = config.3.zeile;
  if (oh <> "") then return to_unix_sep(oh);
  else
    do
      do i = 1 to default_dir.0
        oh = check_octave_old_home(default_dir.i, libpath, path);
        if (oh <> "") then return to_unix_sep(oh);
      end
    end
  return "";

check_octave_old_home: procedure expose debug
parse arg str, libpath, path
  string = to_os2_sep(str);
  pa = 0;
  do while(1)
    pa = pos(to_upper(string), to_upper(path), pa + 1);
    if (pa == 0) then return "";
    if (pa <> 1) then
      do
        if (substr(path, pa - 1, 1) <> ";") then iterate;
      end
    pe = pos(";", path, pa);
    if (pe == 0) then old_home = substr(path, pa);
    else              old_home = substr(path, pa, pe-pa);
    qa = pos(to_upper(old_home || "\DLL"), to_upper(libpath));
    if (qa == 0) then iterate;
    if (qa <> 1) then
      do
        if (substr(path, qa - 1, 1) <> ";") then iterate;
      end
    qe = pos(";", libpath, qa);
    if qe == 0 then old_dll = to_upper(substr(libpath, qa));
    else            old_dll = to_upper(substr(libpath, qa, qe-qa));
    if (to_upper(old_home || "\DLL") == old_dll) then
      do
	flag = check_octave_files(old_home, 0);
	if (flag == 0) then
          do
            say "notice: I FOUND AN OLD OCTAVE DIRECTORY ("fullpath") IN";
            say "notice: LIBPATH AND PATH, BUT WITHOUT ANY OCTAVE FILES.";
            say "notice: SHOULD I REMOVE ALL ENTRIES IN LIBPATH AND PATH";
            say "notice: POINTING TO THAT DIRECTORY [Y/N]";
            parse pull in;
            flag = (in == "Y") | (in == "y");
          end
        if (flag) then return to_unix_sep(old_home);
      end
    else  return "";
  end

check_octave_files: procedure expose debug
parse arg string, flag
  path = to_os2_sep(string);
  rc = SysFileTree(path || "\octave.exe",     exe, "FO");
  rc = SysFileTree(path || "\octave.ico",     ico, "FO");
  rc = SysFileTree(path || "\dll\cruft?.dll", crt, "FO");
  if (flag <> 0) then rc = SysFileTree(path || "\dll\octave?.dll", oct, "FO");
  else                oct.0 = 2;
  res = (exe.0 == 1) & (ico.0 == 1) & (crt.0 == 4) & (oct.0 == 2);
  return res;

/*
*******************************************************************************
** Updating the emx TERMCAP.DAT                                              **
*******************************************************************************
*/
emx_termcap: procedure expose debug
parse arg termcap
  call SysFileTree "etc\termcap.dat", oct_datei, "FT";
  if oct_datei.0 <> 1 then return termcap;
  if datei.1 > 80 then oct = "19" || oct_datei.1;
  else                 oct = "20" || oct_datei.1;

  call SysFileTree to_os2_sep(termcap), emx_datei, "FT";
  if emx_datei.0 <> 1 then return "";
  if datei.1 > 80 then emx = "19" || emx_datei.1;
  else                 emx = "20" || emx_datei.1;

  if oct == emx then return termcap;
  if oct > emx then
    do
      say "info: Replacing" to_os2_sep(termcap) "...";
      "copy" to_os2_sep(termcap) "termcap.old" debug
      "copy etc\termcap.dat" to_os2_sep(termcap) debug
    end
  else
    do
      say "info: Removing termcap.dat coming with Octave/2 ...";
      "del etc\termcap.dat" debug
      "rd etc" debug
    end

  return termcap;

/*
*******************************************************************************
** Replacing the emx-DLLs                                                    **
*******************************************************************************
*/
emx_dlls: procedure expose debug
parse arg libpath, file
  call SysFileTree "dll\" || file, oct_datei, "FT";
  if oct_datei.0 <> 1 then return;
  if datei.1 > 80 then oct = "19" || oct_datei.1;
  else                 oct = "20" || oct_datei.1;

  count = 0;
  do while(1)
    n = setlocal();
    "SET OCTAVE_LIBPATH=" || libpath
    fullpath = SysSearchPath(OCTAVE_LIBPATH, file);
    if fullpath == "" then leave;
    call SysFileTree fullpath, emx_datei, "FT";
    if emx_datei.0 <> 1 then leave;
    if datei.1 > 80 then emx = "19" || emx_datei.1;
    else                 emx = "20" || emx_datei.1;
    n = endlocal();

    if oct == emx then leave;
    if oct > emx then
      do
        count = count+1;
        bak_file = substr(file, 1, length(file)-3) || count;
        "copy" fullpath bak_file debug
        say "info: Older DLL (" || file || ") saved as:" bak_file;
        say "info: Removing DLL (" || file || ") ...";
        rc = 1;
        do while(rc <> 0)
          rc = SysFileDelete(fullpath);
          if rc <> 0 then
            do
              say "notice: UNABLE TO DELETE FILE:" fullpath;
              say "notice: THE DLL IS USED BY ONE OR MORE EMX PROGRAMS!";
              say "notice: KILL ALL THOSE PROGRAMS BEFORE CONTINUING!";
              say "notice: PRESS ENTER, WHEN READY ...";
              parse pull in;
            end
        end            
      end
    else
      do
        say "info: Removing" file "coming with Octave/2 ...";
        "del dll\" || file debug;
        leave;
      end
  end
  return;

/*
*******************************************************************************
** Updating .octaverc                                                        **
*******************************************************************************
*/
update_octaverc: procedure expose debug debug_mode version
parse arg home, initfile, old, new
  if (initfile == "") then octrc = ".octaverc";
  else                     octrc = initfile;

  octrc_new = "octaverc.new";
  rc = ini_files(home, octrc, "octaverc", old, new);
  if (rc == -1) then
    do
      "del" octrc_new debug
      rc = stream(octrc_new, "C", "open write");
      if (rc == "READY:") then
        do
          call lineout octrc_new, "# Startup file"
          call lineout octrc_new, "# Octave" version "for OS/2"
          call lineout octrc_new, "# (c) 1996 - 1997, Klaus Gebhardt"
          rc = stream(octrc_new, "C", "close");
          rc = 2;
        end
      else
        do
          say "error: Cannot create octaverc.new!";
         exit;
        end
    end

  if (rc == 2) then
    do
      "del" to_os2_sep(home) || "\" || octrc debug
      octrc_ini = to_os2_sep(home) || "\octave.ini"
      "copy" octrc_new octrc_ini debug
      "ren" octrc_ini ".octaverc" debug
      rc = stream(octrc_ini, "C", "open read");
      if (rc == "READY:") then
        do
          rc = stream(octrc_new, "C", "close");
          rc = stream(octrc_ini, "C", "open write");
          call lineout octrc_new, ''
          call lineout octrc_new, 'history_file = "octave.hst"'
          rc = stream(octrc_new, "C", "close");
          return "octave.ini";
        end
      return "";
    end
  return initfile;

/*
*******************************************************************************
** Modify the files .emacs, .octaverc                                        **
*******************************************************************************
*/
ini_files: procedure expose debug
parse arg home, inifile, newfile, oldpath, newpath
  file = to_os2_sep(home) || "\" || inifile;
  ini_old = newfile || ".old";
  ini_new = newfile || ".new";
  say "info: Copying" file "to" ini_old "...";
  "copy" file ini_old debug

  old = to_unix_sep(oldpath);
  new = to_unix_sep(newpath);

  rc = stream(ini_old, "C", "open read");
  if (rc <> "READY:") then return -1;

  "del" ini_new debug;
  rc = stream(ini_new, "C", "open write");
  if (rc <> "READY:") then
    do
      say "info: Cannot open" ini_new || "!";
      rc = stream(ini_old, "C", "close");
      exit;
    end

  if (to_upper(old) == to_upper(new)) then return 0;

  rv = 1;
  do while(lines(ini_old))
    line  = linein(ini_old);
    p = pos(to_upper(old), to_upper(line));
    if (p <> 0) then
      do
        rv = 2;
        if (p == 1) then
          do
            line = new || substr(line, 1+length(old));
          end
        else
          do
            line = substr(line, 1, p-1) || new || substr(line, p+length(old));
          end
      end
    call lineout ini_new, line
  end

  rc = stream(ini_new, "C", "close");
  rc = stream(ini_old, "C", "close");
  return rv;

/*
*******************************************************************************
** Remove old INFO files, modify all dir files                               **
*******************************************************************************
*/
info_path_dir: procedure expose debug
parse arg info_path, octave_home, version
  infopath = to_os2_sep(info_path);
  octaveinfopath = to_os2_sep(octave_home || "/doc");

  p = 1;
  q = 1;
  do while (q > 0)
    q = pos(";", infopath, p);
    if (q == 0) then infodir = substr(infopath, p);
    else             infodir = substr(infopath, p, q - p);
    p = q + 1;

    if (infodir == "")  then iterate;
    if (infodir == ".")  then iterate;
    if (to_upper(infodir) == to_upper(octaveinfopath)) then iterate;

    say "info: Removing old info files in" infodir;
    "del" infodir || "\octave" debug
    "del" infodir || "\octave.i0?" debug
    "del" infodir || "\octave.i1?" debug
    "del" infodir || "\liboct" debug
    "del" infodir || "\liboct.i0?" debug
    "del" infodir || "\faq" debug
    "del" infodir || "\oct-faq" debug

    file = infodir || "\dir"
    dir_old = "dir" || p || ".old";
    dir_new = "dir" || p || ".new";
    say "info: Copying" file "to" dir_old "...";
    "copy" file dir_old debug

    rc = stream(dir_old, "C", "open read");
    if (rc <> "READY:") then return;

    "del" dir_new debug;
    rc = stream(dir_new, "C", "open write");
    if (rc <> "READY:") then
      do
        say "error: Cannot open" dir_new || "!";
        rc = stream(dir_old, "C", "close");
        exit;
      end

    line = " ";
    do while(lines(dir_old))
      if line == d2c(31) then call lineout dir_new, line
      line  = linein(dir_old);
      parse var line w1 w2 ":" w3 "." w4
      if ((w1 <> "*") | ((to_upper(w3) <> "(FAQ)") & (to_upper(w3) <> "(OCT-FAQ)") & (to_upper(w3) <> "(OCTAVE)") & (to_upper(w3) <> "(LIBOCT)"))) then
        do
          if line <> d2c(31) then call lineout dir_new, line
        end
    end

    call lineout dir_new, "* octave:    (octave).      Octave" version || "."
    call lineout dir_new, "* liboctave: (liboct).      Info about liboctave" version || "."
    call lineout dir_new, "* octave-faq:(oct-faq).     FAQs about Octave" version || "."
   call lineout dir_new, d2c(31)

    rc = stream(dir_new, "C", "close");
    rc = stream(dir_old, "C", "close");

    say "info: Copying" dir_new "to" file "...";
    "copy" dir_new file debug
  end
  return;

/*
*******************************************************************************
** Unzip DLFCN files                                                         **
*******************************************************************************
*/
unzip_dlfcn_files: procedure expose debug
parse arg dlfcn
  ".\unzip -uo" dlfcn debug
  return;

/*
*******************************************************************************
** Unzip script files                                                        **
*******************************************************************************
*/
unzip_script_files: procedure expose debug
parse arg zipfile
  rc = SysFileTree(zipfile, fs, "F");
  if fs.0 = 0 then return;
  rc = SysFileTree("ChangeLog", fs, "F");
  if fs.0 = 1 then scr = "scripts/*";
  else             scr = "scripts.fat/*";
  say "info: Unzipping scriptfiles ...";
  if fs.0 = 0 then "ren scripts scripts.fat";
  ".\unzip -uo" zipfile scr debug
  if fs.0 = 0 then "ren scripts.fat scripts";
  return;

/*
*******************************************************************************
** Create a WPS object for Octave/2                                          **
*******************************************************************************
*/
create_wps_object: procedure expose debug
parse arg octave_home, version

  octave_folder_id = "<HWB_OCTAVE_FOLDER>";

  call SysCreateObject "WPFolder", "Octave/2", "<WP_DESKTOP>", ,
       "OBJECTID="||octave_folder_id, "fail"

  object_name = "Octave" version;
  octave_file = to_os2_sep(octave_home) || "\octave.exe";
  octave_icon = to_os2_sep(octave_home) || "\octave.ico";

  rc = SysCreateObject("WPProgram", object_name, octave_folder_id, ,
       "EXENAME="octave_file";PROGTYPE=WINDOWABLEVIO;ICONFILE="octave_icon||,
       ";OBJECTID=<HWB_OCTAVE>", "replace");

  if rc == 1 then say "info: Program object for Octave created successfully."
  else            say "notice: Could not create program object for Octave."

  call make_book 'oct-faq.inf', 'FAQ about Octave', ''
  call make_book 'octave.inf', 'GNU Octave', ''
  call make_book 'liboct.inf', 'Octave C++ Classes', ''

  return;

make_book: procedure expose octave_folder_id octave_home
parse arg name, title, parm

  file = to_os2_sep(octave_home) || "\doc\" || name;

  rc = SysCreateObject('WPProgram', title, octave_folder_id, ,
                      'PROGTYPE=PM;EXENAME=VIEW.EXE;PARAMETERS='||file||parm, ,
                      'replace');

  if rc == 1 then say "info: Book object" title "created successfully."
  else            say "notice: Could not create book object" title "."
  return;

/*
*******************************************************************************
** Determine the drive OS/2 is booted from                                   **
*******************************************************************************
*/
get_boot_drive: procedure expose debug
  irc = SysIni("BOTH", "FolderWorkareaRunningObjects",,
               "ALL:", "Objects");
  boot1 = left(Objects.1, 2);;
  do i = 2 to Objects.0
    if (to_upper(right(Objects.i, 7)) == "DESKTOP")           then boot1 = left(Objects.i, 2);
    if (to_upper(right(Objects.i,17)) == "ARBEITSOBERFLŽCHE") then boot1 = left(Objects.i, 2);
    if (to_upper(right(Objects.i, 9)) == "SKRIVBORD")         then boot1 = left(Objects.i, 2);
  end
  boot2 = substr(translate(value("PATH", , "OS2ENVIRONMENT")), pos("\OS2\SYSTEM", translate(value("PATH", , "OS2ENVIRONMENT")))-2, 2);
  rc = SysFileTree(boot1 || "\config.sys", cfg, "FO");
  if ((to_upper(boot1) == to_upper(boot2)) & (cfg.0 == 1)) then return boot1;
  else
    do
      say "error: Unable to determine the boot drive!";
      do while (1)
        say "error: Available disk drives are:"
        drivelist = SysDriveMap()
        say "error:   " drivelist

        say "error: Please enter the OS/2 boot drive OS/2 (example, c:)?  "
        pull drive

        rc = SysFileTree(drive || "\config.sys", cfg, "FO");
        if ((wordpos(drive, drivelist) == 0) & (cfg.0 <> 1)) then
          do
            say "error: You must enter a proper drive letter with colon."
            say "error: There must also be the file CONFIG.SYS in the root!"
          end
        else return drive;
      end
    end
  return boot1;

/*
*******************************************************************************
** Replace old pathes                                                        **
*******************************************************************************
*/
subst_paths: procedure expose debug
parse arg path_arg, old_path, new_path
  path= to_os2_sep(path_arg);
  old = to_os2_sep(old_path);
  new = to_os2_sep(new_path);

  p = 0;
  do while(1)
    p = pos(to_upper(old), to_upper(path), p + 1);
    if (p == 0) then
      do
        if (substr(path, length(path)) == ";") then return path || new || ";";
        else                                        return path || ";" || new;
      end
    if (p <> 1) then
      do
        if (substr(path, p - 1, 1) <> ";") then iterate;
      end
    q = pos(";", path, p);
    if (q == 0) then old_path = substr(path, p);
    else             old_path = substr(path, p, q - p);
    if (to_upper(old_path) <> to_upper(old)) then iterate;
    if (q == 0) then return substr(path, 1, p-1) || new;
    else             return substr(path, 1, p-1) || new || substr(path, q);
  end

/*
*******************************************************************************
** Utilities                                                                 **
*******************************************************************************
*/
to_upper: procedure
parse arg string
  return translate(string, "ABCDEFGHIJKLMNOPQRSTUVWXYZ", "abcdefghijklmnopqrstuvwxyz");

to_lower: procedure
parse arg string
  return translate(string, "abcdefghijklmnopqrstuvwxyz", "ABCDEFGHIJKLMNOPQRSTUVWXYZ");

to_unix_sep: procedure
parse arg string
  return translate(string, "/", "\");

to_os2_sep: procedure
parse arg string
  return translate(string, "\", "/");
