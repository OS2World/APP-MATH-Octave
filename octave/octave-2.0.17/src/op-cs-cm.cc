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

#include "gripes.h"
#include "ov.h"
#include "ov-complex.h"
#include "ov-cx-mat.h"
#include "ov-typeinfo.h"
#include "op-cs-cm.h"
#include "ops.h"
#include "xdiv.h"
#include "xpow.h"

// complex scalar by complex matrix ops.

static octave_value
add (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return octave_value (v1.complex_value () + v2.complex_matrix_value ());
}

static octave_value
sub (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return octave_value (v1.complex_value () - v2.complex_matrix_value ());
}

static octave_value
mul (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return octave_value (v1.complex_value () * v2.complex_matrix_value ());
}

static octave_value
div (const octave_value&, const octave_value& v2)
{
  gripe_nonconformant ("operator /", 1, 1, v2.rows (), v2.columns ());
  return octave_value ();
}

static octave_value
pow (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return xpow (v1.complex_value (), v2.complex_matrix_value ());
}

static octave_value
ldiv (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  Complex d = v1.complex_value ();

  if (d == 0.0)
    gripe_divide_by_zero ();

  return octave_value (v2.complex_matrix_value () / d);
}

#define BOOL_OP(OP, EMPTY_RESULT) \
  SC_MX_BOOL_OP (Complex, s, v1.complex_value (), \
		 ComplexMatrix, m, v2.complex_matrix_value (), \
		 real (s) OP real (m (i, j)), EMPTY_RESULT)

static octave_value
lt (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  BOOL_OP (<, Matrix ());
}

static octave_value
le (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  BOOL_OP (<=, Matrix ());
}

static octave_value
eq (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  SC_MX_BOOL_OP (Complex, s, v1.complex_value (),
		 ComplexMatrix, m, v2.complex_matrix_value (),
		 s == m (i, j), 0.0);
}

static octave_value
ge (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  BOOL_OP (>=, Matrix ());
}

static octave_value
gt (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  BOOL_OP (>, Matrix ());
}

static octave_value
ne (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  SC_MX_BOOL_OP (Complex, s, v1.complex_value (),
		 ComplexMatrix, m, v2.complex_matrix_value (),
		 s != m (i, j), 1.0);
}

static octave_value
el_mul (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return octave_value (v1.complex_value () * v2.complex_matrix_value ());
}

static octave_value
el_div (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return x_el_div (v1.complex_value (), v2.complex_matrix_value ());
}

static octave_value
el_pow (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  return elem_xpow (v1.complex_value (), v2.complex_matrix_value ());
}

static octave_value
el_ldiv (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  Complex d = v1.complex_value ();

  if (d == 0.0)
    gripe_divide_by_zero ();

  return octave_value (v2.complex_matrix_value () / d);
}

static octave_value
el_and (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  SC_MX_BOOL_OP (Complex, s, v1.complex_value (),
		 ComplexMatrix, m, v2.complex_matrix_value (),
		 s != 0.0 && m (i, j) != 0.0, Matrix ());
}

static octave_value
el_or (const octave_value& a1, const octave_value& a2)
{
  CAST_BINOP_ARGS (const octave_complex&, const octave_complex_matrix&);

  SC_MX_BOOL_OP (Complex, s, v1.complex_value (),
		 ComplexMatrix, m, v2.complex_matrix_value (),
		 s != 0.0 || m (i, j) != 0.0, Matrix ());
}

static octave_value *
complex_matrix_conv (const octave_value& a)
{
  CAST_CONV_ARG (const octave_complex&);

  return new octave_complex_matrix (v.complex_matrix_value ());
}

void
install_cs_cm_ops (void)
{
  INSTALL_BINOP (add, octave_complex, octave_complex_matrix, add);
  INSTALL_BINOP (sub, octave_complex, octave_complex_matrix, sub);
  INSTALL_BINOP (mul, octave_complex, octave_complex_matrix, mul);
  INSTALL_BINOP (div, octave_complex, octave_complex_matrix, div);
  INSTALL_BINOP (pow, octave_complex, octave_complex_matrix, pow);
  INSTALL_BINOP (ldiv, octave_complex, octave_complex_matrix, ldiv);
  INSTALL_BINOP (lt, octave_complex, octave_complex_matrix, lt);
  INSTALL_BINOP (le, octave_complex, octave_complex_matrix, le);
  INSTALL_BINOP (eq, octave_complex, octave_complex_matrix, eq);
  INSTALL_BINOP (ge, octave_complex, octave_complex_matrix, ge);
  INSTALL_BINOP (gt, octave_complex, octave_complex_matrix, gt);
  INSTALL_BINOP (ne, octave_complex, octave_complex_matrix, ne);
  INSTALL_BINOP (el_mul, octave_complex, octave_complex_matrix, el_mul);
  INSTALL_BINOP (el_div, octave_complex, octave_complex_matrix, el_div);
  INSTALL_BINOP (el_pow, octave_complex, octave_complex_matrix, el_pow);
  INSTALL_BINOP (el_ldiv, octave_complex, octave_complex_matrix, el_ldiv);
  INSTALL_BINOP (el_and, octave_complex, octave_complex_matrix, el_and);
  INSTALL_BINOP (el_or, octave_complex, octave_complex_matrix, el_or);

  INSTALL_ASSIGNCONV (octave_complex, octave_complex_matrix, octave_complex_matrix);

  INSTALL_WIDENOP (octave_complex, octave_complex_matrix, complex_matrix_conv);
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
