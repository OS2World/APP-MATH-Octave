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

#if !defined (procstream_h)
#define procstream_h 1

#include "iostream.h"
#include "procbuf.h"

class procstreambase : virtual public ios
{
 public:
  procstreambase (void);
  procstreambase (const char *command,
		  int mode = ios::out, int p_mode = procbuf::nowait);

  procbuf *rdbuf (void) const { return (procbuf *) _strbuf; }

  void open (const char *command,
	     int mode = ios::out, int p_mode = procbuf::nowait);
  int is_open (void) { return rdbuf()->is_open (); }
  int close (void);
  _IO_pid_t pid (void) { return rdbuf()->pid(); }
  ~procstreambase();
};

class iprocstream : public procstreambase, public istream
{
 public:
  iprocstream (void) : procstreambase () {}
  iprocstream (const char *command, int mode = ios::in,
	       int p_mode = procbuf::nowait)
    : procstreambase (command, mode, p_mode) {}

  void open (const char *command, int mode = ios::in,
	     int p_mode = procbuf::nowait)
  { procstreambase::open (command, mode, p_mode); }
};

class oprocstream : public procstreambase, public ostream
{
 public:
  oprocstream (void) : procstreambase () {}
  oprocstream (const char *command, int mode = ios::out,
	       int p_mode = procbuf::nowait)
    : procstreambase (command, mode, p_mode) {}

  void open (const char *command, int mode = ios::out,
	     int p_mode = procbuf::nowait)
  { procstreambase::open (command, mode, p_mode); }
};

#endif
