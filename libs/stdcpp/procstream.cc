/* This is part of libio/iostream, providing -*- C++ -*- input/output.
Copyright (C) 1993 Free Software Foundation

This file is part of the GNU IO Library.  This library is free
software; you can redistribute it and/or modify it under the
terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option)
any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with GNU CC; see the file COPYING.  If not, write to
the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

As a special exception, if you link this library with files
compiled with a GNU compiler to produce an executable, this does not cause
the resulting executable to be covered by the GNU General Public License.
This exception does not however invalidate any other reasons why
the executable file might be covered by the GNU General Public License. */

/* Written by Klaus Gebhardt, 1994 - 1995 */

#include "procstream.h"

procstreambase::procstreambase (void)
{
  init (new procbuf ());
}

procstreambase::procstreambase (const char *command, int mode, int p_mode)
{
  init (new procbuf ());
  if (! rdbuf()->open (command, mode, p_mode))  set (ios::badbit);
}

void
procstreambase::open (const char *command, int mode, int p_mode)
{
  clear ();
  if (! rdbuf()->open (command, mode, p_mode))  set (ios::badbit);
}

int
procstreambase::close (void)
{
  int status = rdbuf()->sys_close();
  if (! rdbuf()->close ())  set (ios::failbit);
  return status;
}

procstreambase::~procstreambase()
{
  close();
}
