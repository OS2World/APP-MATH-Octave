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

#if !defined (octave_octave_strstream_h)
#define octave_octave_strstream_h 1

#include <string>

#include <strstream.h>

#include "oct-stream.h"

class
octave_base_strstream : public octave_base_stream
{
public:

  octave_base_strstream (ios::openmode arg_md = ios::out,
			 oct_mach_info::float_format flt_fmt =
			 oct_mach_info::native)
    : octave_base_stream (arg_md, flt_fmt) { }

  ~octave_base_strstream (void) { }

  // Position a stream at OFFSET relative to ORIGIN.

  int seek (streamoff offset, ios::seek_dir origin);

  // Return current stream position.

  long tell (void) const;

  // The name of the file.

  string name (void) { return string (); }

  virtual streambuf *rdbuf (void) = 0;

  virtual bool bad (void) const = 0;

  virtual void clear (void) = 0;

private:

  // No copying!

  octave_base_strstream (const octave_base_strstream&);

  octave_base_strstream& operator = (const octave_base_strstream&);
};

class
octave_istrstream : public octave_base_strstream
{
public:

  octave_istrstream (const char *data,
		     ios::openmode arg_md = ios::out,
		     oct_mach_info::float_format flt_fmt =
		     oct_mach_info::native)
    : octave_base_strstream (arg_md, flt_fmt), is (data) { }

  octave_istrstream (const string& data,
		     ios::openmode arg_md = ios::out,
		     oct_mach_info::float_format flt_fmt =
		     oct_mach_info::native)
    : octave_base_strstream (arg_md, flt_fmt), is (data.c_str ()) { }

  ~octave_istrstream (void) { }

  // Return non-zero if EOF has been reached on this stream.

  bool eof (void) const { return is.eof (); }

  istream *input_stream (void) { return &is; }

  ostream *output_stream (void) { return 0; }

  streambuf *rdbuf (void) { return is ? is.rdbuf () : 0; }

  bool bad (void) const { return is.bad (); }

  void clear (void) { is.clear (); }

private:

  istrstream is;

  // No copying!

  octave_istrstream (const octave_istrstream&);

  octave_istrstream& operator = (const octave_istrstream&);
};

class
octave_ostrstream : public octave_base_strstream
{
public:

  octave_ostrstream (ios::openmode arg_md = ios::out,
		     oct_mach_info::float_format flt_fmt =
		     oct_mach_info::native)
    : octave_base_strstream (arg_md, flt_fmt) { }

  ~octave_ostrstream (void) { }

  // Return non-zero if EOF has been reached on this stream.

  bool eof (void) const { return os.eof (); }

  istream *input_stream (void) { return 0; }

  ostream *output_stream (void) { return &os; }

  char *str (void)
    {
      os << ends;
      return os.str ();
    }

  streambuf *rdbuf (void) { return os ? os.rdbuf () : 0; }

  bool bad (void) const { return os.bad (); }

  void clear (void) { os.clear (); }

private:

  ostrstream os;

  // No copying!

  octave_ostrstream (const octave_ostrstream&);

  octave_ostrstream& operator = (const octave_ostrstream&);
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
