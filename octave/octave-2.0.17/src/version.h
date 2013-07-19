/*

Copyright (C) 1996, 1997, 1998 John W. Eaton

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

/* Modified by Klaus Gebhardt, 1996 - 1998 */

#if defined (__EMX__) && defined (OS2)
#define OCTAVE_VERSION "2.0.17"
#define OCTAVE_OS2_PATCHLEVEL "2.0.17-b01"

#define OCTAVE_COPYRIGHT \
"Copyright (C) 1996, 1997, 1999, 2000, 2001, 2002 John W. Eaton.\n\
OS/2-Port by Klaus Gebhardt, 1996 - 2000."

#define OCTAVE_NAME_AND_VERSION \
"Octave " OCTAVE_VERSION " for OS/2 2.x, Warp 3 and Warp 4.\n(Patchlevel " \
OCTAVE_OS2_PATCHLEVEL ")"

#define OCTAVE_NAME_VERSION_AND_COPYRIGHT \
  OCTAVE_NAME_AND_VERSION ".\n" OCTAVE_COPYRIGHT "\n\
This is free software with ABSOLUTELY NO WARRANTY."

#define OCTAVE_STARTUP_MESSAGE \
  OCTAVE_NAME_VERSION_AND_COPYRIGHT "\n\
For details, type `warranty'."
#else
#define OCTAVE_VERSION "2.0.17"

#define OCTAVE_COPYRIGHT \
  "Copyright (C) 1996, 1997, 1998, 1999, 2000, 2001, 2002 John W. Eaton."

#define OCTAVE_NAME_AND_VERSION \
  "GNU Octave, version " OCTAVE_VERSION " (" CANONICAL_HOST_TYPE ")"

#define OCTAVE_NAME_VERSION_AND_COPYRIGHT \
  OCTAVE_NAME_AND_VERSION ".\n" OCTAVE_COPYRIGHT "\n\
This is free software with ABSOLUTELY NO WARRANTY."

#define OCTAVE_STARTUP_MESSAGE \
  OCTAVE_NAME_VERSION_AND_COPYRIGHT "\n\
For details, type `warranty'."
#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
