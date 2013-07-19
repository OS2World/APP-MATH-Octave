/*

Copyright (C) 1996 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <iostream.h>

#include <ctime>

#if defined (WITH_SHL)
#include <cerrno>
#include <cstring>
#endif

#include <string>

#include <strstream.h>

extern "C"
{
#if defined (WITH_DL)
#if defined (HAVE_DLFCN_H)
#include <dlfcn.h>
#else
#ifdef OS2
#define MERLIN_PROCESS
#include <merlin.h>
#else
extern void *dlopen (const char *, int);
extern const char *dlerror (void);
extern void *dlsym (void *, const char *);
extern int dlclose (void *);
#endif
#endif
#ifndef RTLD_LAZY
#define RTLD_LAZY 1
#endif
#elif defined (WITH_SHL)
#include <dl.h>
#endif
}

#include "file-ops.h"

#include "DLList.h"

#include <defaults.h>
#include "defun.h"
#include "dirfns.h"
#include "dynamic-ld.h"
#include "error.h"
#include "toplev.h"
#include "pathsearch.h"
#include "ov.h"
#include "parse.h"
#include "pt-fvc.h"
#include "symtab.h"
#include "utils.h"
#include "variables.h"

// TRUE means that we unload .oct files when there are no functions in
// the symbol table that reference them.
static bool Vauto_unload_dot_oct_files;

typedef builtin_function * (*Octave_builtin_fcn_struct_fcn)(void);

class
oct_dl_info
{
public:

  oct_dl_info (void)
    : fsf (0), handle (0), fcn_nm () { }

  oct_dl_info (Octave_builtin_fcn_struct_fcn f, void *h, const string& fn)
    : fsf (f), handle (h), fcn_nm (fn) { }

  oct_dl_info (const oct_dl_info& odi)
    : fsf (odi.fsf), handle (odi.handle), fcn_nm (odi.fcn_nm) { }

  ~oct_dl_info (void) { }

  oct_dl_info& operator = (const oct_dl_info& odi)
    {
      if (this != &odi)
	{
	  fsf = odi.fsf;
	  handle = odi.handle;
	  fcn_nm = odi.fcn_nm;
	}

      return *this;
    }

  Octave_builtin_fcn_struct_fcn function (void) const { return fsf; }

  void *library (void) const { return handle; }

  string function_name (void) const { return fcn_nm; }

private:

  Octave_builtin_fcn_struct_fcn fsf;

  void *handle;

  string fcn_nm;
};

template class DLNode<oct_dl_info>;
template class DLList<oct_dl_info>;

typedef DLList<oct_dl_info> dl_fcn_list;

class
oct_dl_lib
{
public:

  oct_dl_lib (void)
    : nm (), tm (0), handle (0), fcn_list (), obsolete (false) { }

  oct_dl_lib (const string& n, time_t t, const oct_dl_info& dl_info)
    : nm (n), tm (t), handle (0), fcn_list (), obsolete (false)
  {
    handle = dl_info.library ();

    fcn_list.append (dl_info);
  }

  oct_dl_lib (const oct_dl_lib &odl)
    : nm (odl.nm), tm (odl.tm), handle (odl.handle),
      fcn_list (odl.fcn_list), obsolete (odl.obsolete) { }

  oct_dl_lib& operator = (const oct_dl_lib &odl);

  ~oct_dl_lib (void) { }

  string name (void) const { return nm; }

  time_t load_time (void) const { return tm; }

  void *library (void) const { return handle; }

  int count (void) const { return fcn_list.length (); }

  void add_function (const oct_dl_info& dl_info)
  {
    fcn_list.append (dl_info);
  }

  void clear_function (void *f);

  void clear_all_functions (void);

  string_vector loaded_function_list (void) const;

  void mark_out_of_date (void) { obsolete = true; }

  bool out_of_date (void) const { return obsolete; }

private:

  string nm;

  time_t tm;

  void *handle;

  dl_fcn_list fcn_list;

  bool obsolete;
};

oct_dl_lib&
oct_dl_lib::operator = (const oct_dl_lib &odl)
{
  if (this != &odl)
    {
      nm = odl.nm;
      tm = odl.tm;
      handle = odl.handle;
      fcn_list = odl.fcn_list;
      obsolete = odl.obsolete;
    }

  return *this;
}

void
oct_dl_lib::clear_function (void *f)
{
  for (Pix p = fcn_list.first (); p != 0; fcn_list.next (p))
    {
      if (fcn_list(p).function () == f)
	{
	  fcn_list.del (p);

	  break;
	}
    }
}

void
oct_dl_lib::clear_all_functions (void)
{
  string_vector fcns_to_clear = loaded_function_list ();

  int len = fcns_to_clear.length ();

  if (len == 0)
    {
      void *lib = library ();

      clear_oct_file_fcn (lib, 0);
    }
  else
    {
      for (int i = 0; i < len; i++)
	{
	  string fcn_nm = fcns_to_clear(i);

	  curr_sym_tab->clear (fcn_nm);

	  if (curr_sym_tab != top_level_sym_tab)
	    top_level_sym_tab->clear (fcn_nm);

	  global_sym_tab->clear (fcn_nm);
	}
    }
}

string_vector
oct_dl_lib::loaded_function_list (void) const
{
  int len = fcn_list.length ();

  string_vector retval (len);

  int j = 0;

  for (Pix p = fcn_list.first (); p != 0; fcn_list.next (p))
    retval(j++) = fcn_list(p).function_name ();

  return retval;
}

template class DLNode<oct_dl_lib>;
template class DLList<oct_dl_lib>;

typedef DLList<oct_dl_lib> oct_dl_lib_list;

// List of libraries we have loaded.
static oct_dl_lib_list lib_list;

#if defined (WITH_DYNAMIC_LINKING)

// Convert a the function named NAME to the corresponding symbol name.
// The convention for the prefix of the name is defined by the macros
// in defun-int.h.  The suffix depends on the way the C++ compiler
// mangles names and should really be determined by the configure
// script.  XXX FIXME XXX

static string
mangle_octave_oct_file_name (const string& name)
{
  string retval ("FS");
  retval.append (name);
  retval.append ("__Fv");
  return retval;
}

#if defined (WITH_DL)

static oct_dl_info
dl_resolve_ref (const string& name, const string& mangled, const string& file)
{
  oct_dl_info retval;

  const char *nm = mangled.c_str ();
  
  for (Pix p = lib_list.first (); p != 0; lib_list.next (p))
    {
      void *function = 0;

      const char *errmsg = 0;

      void *library = lib_list(p).library ();

      file_stat fs (lib_list(p).name ());

      if (fs.is_newer (lib_list(p).load_time ()))
	{
	  lib_list(p).mark_out_of_date ();

	  lib_list(p).clear_all_functions ();

	  continue;
	}

      function = dlsym (library, nm);

      errmsg = dlerror ();

      if (! errmsg)
	{
	  retval = oct_dl_info ((Octave_builtin_fcn_struct_fcn) function,
				library, name);

	  lib_list(p).add_function (retval);

	  return retval;
	}
    }

  void *library = dlopen (file.c_str (), RTLD_LAZY);

  time_t now = time (0);

  time_t dlopen_time = now;

  file_stat fs (file);

  if (Vwarn_future_time_stamp && fs.is_newer (now))
    warning ("time stamp for `%s' is in the future", file.c_str ());

  if (library)
    {
      void *function = dlsym (library, nm);

      //      const char *errmsg = dlerror ();

      //      if (errmsg)
      //	error ("%s: `%s'", nm, errmsg);

      retval = oct_dl_info ((Octave_builtin_fcn_struct_fcn) function,
			    library, name);

      lib_list.append (oct_dl_lib (file, dlopen_time, retval));
    }
  else
    error ("%s: %s `%s'", dlerror (), file.c_str (), nm);

  return retval;
}

#elif defined (WITH_SHL)

static oct_dl_info
shl_resolve_ref (const string& name, const string& mangled, const string& file)
{
  oct_dl_info retval;

  const char *nm = mangled.c_str ();

  for (Pix p = lib_list.first (); p != 0; lib_list.next (p))
    {
      void *function = 0;

      int status = 0;

      shl_t library = (shl_t) (lib_list(p).library ());

      file_stat fs (lib_list(p).name ());

      if (fs.is_newer (lib_list(p).load_time ()))
	{
	  lib_list(p).mark_out_of_date ();

	  lib_list(p).clear_all_functions ();
	}

      // Don't use TYPE_PROCEDURE here.  The man page says that future
      // versions of HP-UX may not support it.

      status = shl_findsym (&library, nm, TYPE_UNDEFINED, &function);

      if (status >= 0)
	{
	  retval = oct_dl_info ((Octave_builtin_fcn_struct_fcn) function,
				library, name);

	  lib_list(p).add_function (retval);

	  return retval;
	}
    }

  shl_t library = shl_load (file.c_str (), BIND_DEFERRED, 0L);

  time_t now = time (0);

  time_t shl_load_time = now;

  file_stat fs (file);

  if (Vwarn_future_time_stamp && fs.is_newer (now))
    warning ("time stamp for `%s' is in the future", file.c_str ());

  if (library)
    {
      // Don't use TYPE_PROCEDURE here.  The man page says that future
      // versions of HP-UX may not support it.

      void *function;

      int status = shl_findsym (&library, nm, TYPE_UNDEFINED, &function);

      if (status < 0)
	{
	  //	  const char *errmsg = strerror (errno);

	  //	  if (errmsg)
	  //	    error("%s: `%s'", nm, errmsg);
	  //	  else
	  //	    error("unable to link function `%s'", nm);
	}
      else
	{
	  retval = oct_dl_info ((Octave_builtin_fcn_struct_fcn) function,
				library, name);

	  lib_list.append (oct_dl_lib (file, shl_load_time, retval));
	}
    }
  else
    error ("%s: %s `%s'", strerror (errno), file.c_str (), nm);

  return retval;
}

#endif
#endif

#if defined (WITH_DYNAMIC_LINKING)
static oct_dl_info
resolve_ref (const string& name, const string& mangled, const string& file)
{
#if defined (WITH_DL)

  return dl_resolve_ref (name, mangled, file);

#elif defined (WITH_SHL)

  return shl_resolve_ref (name, mangled, file);

#endif
}
#endif

int
load_octave_oct_file (const string& name)
{
  int retval = 0;

#if defined (WITH_DYNAMIC_LINKING)

  string oct_file = oct_file_in_path (name);

  if (! oct_file.empty ())
    {
      string mangled = mangle_octave_oct_file_name (name);

      oct_dl_info f_info = resolve_ref (name, mangled, oct_file);

      // XXX FIXME XXX -- this should probably be handled correctly by
      // mangle_octave_oct_file_name using a configure test.

      if (! f_info.function ())
	{
	  string t = "_";

	  mangled = t.append (mangled);

	  f_info = resolve_ref (name, mangled, oct_file);
	}

      if (f_info.function ())
	{
	  Octave_builtin_fcn_struct_fcn fcn_to_call = f_info.function ();

	  builtin_function *s = fcn_to_call ();

	  if (s)
	    {
	      install_dot_oct_function (*s, oct_file, f_info.library (),
					f_info.function ());
	      retval = 1;
	    }
	}
      else
	error ("unable to load functions from %s\n", oct_file.c_str ());
    }

#else

  (void) name;

#endif

  return retval;
}

string_vector
clear_oct_file_fcn (void *library, void *function)
{
  string_vector fcns_to_clear;

#if defined (WITH_DYNAMIC_LINKING)

  if (! library)
    return fcns_to_clear;

  for (Pix p = lib_list.first (); p != 0; lib_list.next (p))
    {
      if (lib_list(p).library () == library)
	{
	  if (function)
	    lib_list(p).clear_function (function);

	  if (lib_list(p).count () == 0)
	    {
	      if (lib_list(p).out_of_date () || Vauto_unload_dot_oct_files)
		{
#if defined (WITH_DL)
		  dlclose (library);
#elif defined (WITH_SHL)
		  shl_unload ((shl_t) library);
#endif
		  lib_list.del (p);
		}
	    }
	  else
	    fcns_to_clear = lib_list(p).loaded_function_list ();

	  break;
	}
    }

#endif

  return fcns_to_clear;
}

void
init_dynamic_linker (void)
{
  // Nothing to do anymore...
}

static int
auto_unload_dot_oct_files (void)
{
  Vauto_unload_dot_oct_files = check_preference ("auto_unload_dot_oct_files");

  return 0;
}

void
symbols_of_dynamic_ld (void)
{
  DEFVAR (auto_unload_dot_oct_files, 0.0, 0, auto_unload_dot_oct_files,
    "control whether .oct files are automatically unloaded when there\n\
are no longer any functions in the symbol table that reference them");
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
