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

#include <iostream.h>

#include "dbleSVD.h"
#include "f77-fcn.h"
#include "mx-inlines.cc"

extern "C"
{
  int F77_FCN (dgesvd, DGESVD) (const char*, const char*, const int&,
				const int&, double*, const int&,
				double*, double*, const int&, double*,
				const int&, double*, const int&, int&,
				long, long);
}

Matrix
SVD::left_singular_matrix (void) const
{
  if (type_computed == SVD::sigma_only)
    {
      (*current_liboctave_error_handler)
	("ComplexSVD: U not computed because type == SVD::sigma_only");
      return Matrix ();
    }
  else
    return left_sm;
}

Matrix
SVD::right_singular_matrix (void) const
{
  if (type_computed == SVD::sigma_only)
    {
      (*current_liboctave_error_handler)
	("ComplexSVD: V not computed because type == SVD::sigma_only");
      return Matrix ();
    }
  else
    return right_sm;
}

int
SVD::init (const Matrix& a, SVD::type svd_type)
{
  int info;

  int m = a.rows ();
  int n = a.cols ();

  Matrix atmp = a;
  double *tmp_data = atmp.fortran_vec ();

  int min_mn = m < n ? m : n;
  int max_mn = m > n ? m : n;

  char jobu = 'A';
  char jobv = 'A';

  int ncol_u = m;
  int nrow_vt = n;
  int nrow_s = m;
  int ncol_s = n;

  switch (svd_type)
    {
    case SVD::economy:
      jobu = jobv = 'S';
      ncol_u = nrow_vt = nrow_s = ncol_s = min_mn;
      break;

    case SVD::sigma_only:

      // Note:  for this case, both jobu and jobv should be 'N', but
      // there seems to be a bug in dgesvd from Lapack V2.0.  To
      // demonstrate the bug, set both jobu and jobv to 'N' and find
      // the singular values of [eye(3), eye(3)].  The result is
      // [-sqrt(2), -sqrt(2), -sqrt(2)].

      jobu = 'O';
      jobv = 'N';
      ncol_u = nrow_vt = 1;
      break;

    default:
      break;
    }

  type_computed = svd_type;

  if (! (jobu == 'N' || jobu == 'O'))
    left_sm.resize (m, ncol_u);

  double *u = left_sm.fortran_vec ();

  sigma.resize (nrow_s, ncol_s);
  double *s_vec  = sigma.fortran_vec ();

  if (! (jobv == 'N' || jobv == 'O'))
    right_sm.resize (nrow_vt, n);

  double *vt = right_sm.fortran_vec ();

  int tmp1 = 3*min_mn + max_mn;
  int tmp2 = 5*min_mn - 4;
  int lwork = tmp1 > tmp2 ? tmp1 : tmp2;

  Array<double> work (lwork);
  double *pwork = work.fortran_vec ();

  F77_XFCN (dgesvd, DGESVD, (&jobu, &jobv, m, n, tmp_data, m, s_vec,
			     u, m, vt, nrow_vt, pwork, lwork, info,
			     1L, 1L));

  if (f77_exception_encountered)
    (*current_liboctave_error_handler) ("unrecoverable error in dgesvd");
  else
    {
      if (! (jobv == 'N' || jobv == 'O'))
	right_sm = right_sm.transpose ();
    }

  return info;
}

ostream&
operator << (ostream& os, const SVD& a)
{
  os << a.left_singular_matrix () << "\n";
  os << a.singular_values () << "\n";
  os << a.right_singular_matrix () << "\n";

  return os;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
