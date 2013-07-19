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

// Based on Tony Richardson's filter.m.
//
// Originally translated to C++ by KH (Kurt.Hornik@ci.tuwien.ac.at)
// with help from Fritz Leisch and Andreas Weingessel on Oct 20, 1994.
//
// Rewritten to use templates to handle both real and complex cases by
// jwe, Wed Nov  1 19:15:29 1995.

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "defun-dld.h"
#include "error.h"
#include "oct-obj.h"
#include "help.h"

#if !defined (CXX_NEW_FRIEND_TEMPLATE_DECL)
extern MArray<double>
filter (MArray<double>&, MArray<double>&, MArray<double>&);

extern MArray<Complex>
filter (MArray<Complex>&, MArray<Complex>&, MArray<Complex>&);
#endif

template <class T>
MArray<T>
filter (MArray<T>& b, MArray<T>& a, MArray<T>& x, MArray<T>& si)
{
  MArray<T> y;

  int a_len  = a.length ();
  int b_len  = b.length ();
  int x_len  = x.length ();

  int si_len = si.length ();

  int ab_len = a_len > b_len ? a_len : b_len;

  b.resize (ab_len, 0.0);

  if (si.length () != ab_len - 1)
    {
      error ("filter: si must be a vector of length max (length (a), length (b)) - 1");
      return y;
    }

  T norm = a (0);

  if (norm == 0.0)
    {
      error ("filter: the first element of a must be non-zero");
      return y;
    }

  y.resize (x_len, 0.0);

  if (norm != 1.0)
    b = b / norm;

  if (a_len > 1)
    {
      a.resize (ab_len, 0.0);

      if (norm != 1.0)
	a = a / norm;

      T *py = y.fortran_vec ();
      T *psi = si.fortran_vec ();

      const T *pb = b.data ();
      const T *pa = a.data ();
      const T *px = x.data ();

      for (int i = 0; i < x_len; i++)
	{
	  py[i] = psi[0] + pb[0] * px[i];

	  if (si_len > 1)
	    {
	      for (int j = 0; j < si_len - 1; j++)
		psi[j] = psi[j+1] - pa[j+1] * py[i] + pb[j+1] * px[i];

	      psi[si_len-1] = pb[si_len] * px[i] - pa[si_len] * py[i];
	    }
	  else
	    psi[0] = pb[si_len] * px[i] - pa[si_len] * py[i];
	}
    }
  else if (si_len > 0)
    {
      T *py = y.fortran_vec ();
      T *psi = si.fortran_vec ();

      const T *pb = b.data ();
      const T *px = x.data ();

      for (int i = 0; i < x_len; i++)
	{
	  py[i] = psi[0] + pb[0] * px[i];

	  if (si_len > 1)
	    {
	      for (int j = 0; j < si_len - 1; j++)
		psi[j] = psi[j+1] + pb[j+1] * px[i];

	      psi[si_len-1] = pb[si_len] * px[i];
	    }
	  else
	    psi[0] = pb[1] * px[i];
	}
    }
  else
    y = b (0) * x;

  return y;
}

#if !defined (CXX_NEW_FRIEND_TEMPLATE_DECL)
extern MArray<double>
filter (MArray<double>&, MArray<double>&, MArray<double>&,
	MArray<double>&);

extern MArray<Complex>
filter (MArray<Complex>&, MArray<Complex>&, MArray<Complex>&,
	MArray<Complex>&);
#endif

template <class T>
MArray<T>
filter (MArray<T>& b, MArray<T>& a, MArray<T>& x)
{
  int a_len = a.length ();
  int b_len = b.length ();

  int si_len = (a_len > b_len ? a_len : b_len) - 1;

  MArray<T> si (si_len, T (0.0));

  return filter (b, a, x, si);
}

DEFUN_DLD (filter, args, nargout,
  "usage: [y [, sf]] = filter (b, a, x [, si])\n\
\n\
y = filter (b, a, x) returns the solution to the following linear,\n\
time-invariant difference equation:\n\
\n\
  a[1] y[n] + ... + a[la] y[n-la+1] = b[1] x[n] + ... + b[lb] x[n-lb+1],\n\
where la = length (a) and lb = length (b).\n\
\n\
[y, sf] = filter (b, a, x, si) sets the initial state of the system, si,\n\
and returns the final state, sf.  The state vector is a column vector\n\
whose length is equal to the length of the longest coefficient vector\n\
minus one.  If si is not set, the initial state vector is set to all\n\
zeros.\n\
\n\
The particular algorithm employed is known as a transposed Direct Form II\n\
implementation.")
{
  octave_value_list retval;

  int nargin  = args.length ();

  if (nargin < 3 || nargin > 4)
    {
      print_usage ("filter");
      return retval;
    }

  const char *errmsg = "filter: arguments must be vectors";

  int x_is_vector = (args(2).rows () == 1 || args(2).columns () == 1);

  int si_is_vector = (nargin == 4
		      && (args(3).rows () == 1 || args(3).columns () == 1));

  if (args(0).is_complex_type ()
      || args(1).is_complex_type ()
      || args(2).is_complex_type ()
      || (nargin == 4 && args(3).is_complex_type ()))
    {
      ComplexColumnVector b = args(0).complex_vector_value ();
      ComplexColumnVector a = args(1).complex_vector_value ();
      ComplexColumnVector x = args(2).complex_vector_value ();

      if (! error_state)
	{
	  ComplexColumnVector si;

	  if (nargin == 3)
	    {
	      int a_len = a.length ();
	      int b_len = b.length ();

	      int si_len = (a_len > b_len ? a_len : b_len) - 1;

	      si.resize (si_len, 0.0);
	    }
	  else
	    si = args(3).complex_vector_value ();

	  if (! error_state)
	    {
	      ComplexColumnVector y (filter (b, a, x, si));

	      if (nargout == 2)
		{
		  if (si_is_vector)
		    retval (1) = octave_value (si, (args(3).columns () == 1));
		  else
		    retval (1) = si;
		}

	      if (x_is_vector)
		retval (0) = octave_value (y, (args(2).columns () == 1));
	      else
		retval (0) = y;
	    }
	  else
	    error (errmsg);
	}
      else
	error (errmsg);
    }
  else
    {
      ColumnVector b = args(0).vector_value ();
      ColumnVector a = args(1).vector_value ();
      ColumnVector x = args(2).vector_value ();

      if (! error_state)
	{
	  ColumnVector si;

	  if (nargin == 3)
	    {
	      int a_len = a.length ();
	      int b_len = b.length ();

	      int si_len = (a_len > b_len ? a_len : b_len) - 1;

	      si.resize (si_len, 0.0);
	    }
	  else
	    si = args(3).vector_value ();

	  if (! error_state)
	    {
	      ColumnVector y (filter (b, a, x, si));

	      if (nargout == 2)
		{
		  if (si_is_vector)
		    retval (1) = octave_value (si, (args(3).columns () == 1));
		  else
		    retval (1) = si;
		}

	      if (x_is_vector)
		retval (0) = octave_value (y, (args(2).columns () == 1));
	      else
		retval (0) = y;
	    }
	  else
	    error (errmsg);
	}
      else
	error (errmsg);
    }

  return retval;
}

template MArray<double>
filter (MArray<double>&, MArray<double>&, MArray<double>&,
	MArray<double>&);

template MArray<double>
filter (MArray<double>&, MArray<double>&, MArray<double>&);

template MArray<Complex>
filter (MArray<Complex>&, MArray<Complex>&, MArray<Complex>&,
	MArray<Complex>&);

template MArray<Complex>
filter (MArray<Complex>&, MArray<Complex>&, MArray <Complex>&);

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/  
