/* config.h: master configuration file, included first by all compilable
   source files (not headers).

Copyright (C) 1993, 95, 96 Free Software Foundation, Inc.

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.  */

#ifndef KPATHSEA_CONFIG_H
#define KPATHSEA_CONFIG_H

#ifdef WIN32
#define __STDC__ 1
#include "../win32/win32-compat.h"
#endif /* not WIN32 */

/* System dependencies that are figured out by `configure'.  If we are
   compiling standalone, we get our c-auto.h.  Otherwise, the package
   containing us must provide this (unless it can somehow generate ours
   from c-auto.h.in).  We use <...> instead of "..." so that the current
   cpp directory (i.e., kpathsea/) won't be searched. */
#include <c-auto.h>

#include <kpathsea/c-std.h>    /* <stdio.h>, <math.h>, etc.  */

#include <kpathsea/c-proto.h>  /* Macros to discard or keep prototypes.  */

#include <kpathsea/debug.h>    /* Runtime tracing.  */
#include <kpathsea/lib.h>      /* STREQ, etc. */
#include <kpathsea/types.h>    /* <sys/types.h>, boolean, string, etc. */
#include <kpathsea/progname.h> /* for program_invocation_*name */

/* System defines are for non-Unix systems only.  (Testing for all Unix
   variations should be done in configure.)  Presently the defines used
   are: AMIGA DOS OS2 VMCMS VMS WIN32.  I do not use any of these systems
   myself; if you do, I'd be grateful for any changes. --kb@mail.tug.org */
   
/* If we have either DOS or OS2, we are DOSISH.  */
#if defined (DOS) || defined (OS2) || defined (WIN32)
#define DOSISH
#endif

/* If you want to find subdirectories in a directory with non-Unix
   semantics (specifically, if a directory with no subdirectories does
   not have exactly two links), define this.  */
#if !defined (DOSISH) && !defined (VMS) && !defined (VMCMS)
#define ST_NLINK_TRICK
#endif /* not DOS and not VMS and not VMCMS */

#ifdef AMIGA
/* No popen/pclose on Amiga, but rather than put #ifdef's in tex-make.c,
   let's get rid of the functions here.  (CallMF will automatically
   generate fonts.)  pclose must not be simply empty, since it still
   occurs in a comparison.  */
#define popen(cmd, mode) NULL
#define pclose(file) 0
#endif /* AMIGA */

#endif /* not KPATHSEA_CONFIG_H */
