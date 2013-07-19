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

#if defined (__GNUG__)
#pragma implementation
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "error.h"
#include "oct-obj.h"

int
octave_value_list::all_strings (void) const
{
  int n = length ();

  for (int i = 0; i < n; i++)
    if (! elem(i).is_string ())
      return 0;

  return 1;
}

string_vector
octave_value_list::make_argv (const string& fcn_name) const
{
  string_vector argv;

  if (all_strings ())
    {
      int len = length ();

      int total_nr = 0;

      for (int i = 0; i < len; i++)
	{
	  // An empty string ("") has zero columns and zero rows (a
	  // change that was made for Matlab contemptibility.

	  int n = elem(i).rows ();

	  total_nr += n ? n : 1;
	}

      argv.resize (total_nr+1);

      argv[0] = fcn_name;

      int k = 1;
      for (int i = 0; i < len; i++)
	{
	  int nr = elem(i).rows ();

	  if (nr < 2)
	    argv[k++] = elem(i).string_value ();
	  else
	    {
	      string_vector tmp = elem(i).all_strings ();

	      for (int j = 0; j < nr; j++)
		argv[k++] = tmp[j];
	    }
	}
    }
  else
    error ("%s: expecting all arguments to be strings", fcn_name.c_str ());

  return argv;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
