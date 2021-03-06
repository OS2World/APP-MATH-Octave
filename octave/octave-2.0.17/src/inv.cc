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

#include "defun-dld.h"
#include "error.h"
#include "gripes.h"
#include "help.h"
#include "oct-obj.h"
#include "utils.h"

DEFUN_DLD (inv, args, ,
  "inv (X): inverse of a square matrix")
{
  octave_value_list retval;

  int nargin = args.length ();

  if (nargin != 1)
    {
      print_usage ("inv");
      return retval;
    }

  octave_value arg = args(0);

  int nr = arg.rows ();
  int nc = arg.columns ();

  int arg_is_empty = empty_arg ("inverse", nr, nc);

  if (arg_is_empty < 0)
    return retval;
  else if (arg_is_empty > 0)
    return Matrix ();

  if (nr != nc)
    {
      gripe_square_matrix_required ("inverse");
      return retval;
    }

  if (arg.is_real_type ())
    {
      Matrix m = arg.matrix_value ();

      if (! error_state)
	{
	  int info;
	  double rcond = 0.0;

	  retval = m.inverse (info, rcond, 1);

	  if (info == -1)
	    warning ("inverse: matrix singular to machine precision,\
 rcond = %g", rcond);
	}
    }
  else if (arg.is_complex_type ())
    {
      ComplexMatrix m = arg.complex_matrix_value ();

      if (! error_state)
	{
	  int info;
	  double rcond = 0.0;

	  retval = m.inverse (info, rcond, 1);

	  if (info == -1)
	    warning ("inverse: matrix singular to machine precision,\
 rcond = %g", rcond);
	}
    }
  else
    {
      gripe_wrong_type_arg ("inv", arg);
    }

  return retval;
}

// XXX FIXME XXX -- this should really be done with an alias, but
// alias_builtin() won't do the right thing if we are actually using
// dynamic linking.

DEFUN_DLD (inverse, args, nargout,
  "inverse (X): inverse of a square matrix")
{
  return Finv (args, nargout);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
