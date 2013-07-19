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

/* Modified by Klaus Gebhardt, 1997 - 1999 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cfloat>
#include <cmath>

#if defined (HAVE_IEEEFP_H)
#include <ieeefp.h>
#endif

#if defined (HAVE_SUNMATH_H)
#include <sunmath.h>
#endif

#include "lo-error.h"
#include "lo-ieee.h"
#include "lo-mappers.h"
#include "lo-specfun.h"
#include "lo-utils.h"
#include "oct-cmplx.h"

#include "f77-fcn.h"

#if defined (_AIX) && defined (__GNUG__)
#undef finite
#define finite(x) ((x) < DBL_MAX && (x) > -DBL_MAX)
#endif

extern "C"
{
  double F77_FCN (dacosh, DACOSH) (const double*);
  double F77_FCN (dasinh, DASINH) (const double*);
  double F77_FCN (datanh, DATANH) (const double*);

  double F77_FCN (derf, DERF) (const double&);
  double F77_FCN (derfc, DERFC) (const double&);
}

#ifndef M_LOG10E
#define M_LOG10E 0.43429448190325182765
#endif

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// Double -> double mappers.

double
arg (double x)
{
  if (x < 0.0)
    return M_PI;
  else
#if defined (HAVE_ISNAN)
    return xisnan (x) ? octave_NaN : 0.0;
#else
    return 0.0;
#endif
}

double
conj (double x)
{
  return x;
}

double
fix (double x)
{
  return x > 0 ? floor (x) : ceil (x);
}

double
imag (double x)
{
#if defined (HAVE_ISNAN)
  return xisnan (x) ? octave_NaN : 0.0;
#else
  return 0.0;
#endif
}

double
real (double x)
{
  return x;
}

double
round (double x)
{
  return D_NINT (x);
}

double
signum (double x)
{
  double tmp = 0.0;
  if (x < 0.0)
    tmp = -1.0;
  else if (x > 0.0)
    tmp = 1.0;

#if defined (HAVE_ISNAN)
  return xisnan (x) ? octave_NaN : tmp;
#else
  return tmp;
#endif
}

double
xacosh (double x)
{
#if defined (HAVE_ACOSH)
  return acosh (x);
#else
  double y;
  F77_YXFCN (dacosh, DACOSH, y, (&x));
  return y;
#endif
}

double
xasinh (double x)
{
#if defined (HAVE_ASINH)
  return asinh (x);
#else
  double y;
  F77_YXFCN (dasinh, DASINH, y, (&x));
  return y;
#endif
}

double
xatanh (double x)
{
#if defined (HAVE_ATANH)
  return atanh (x);
#else
  double y;
  F77_YXFCN (datanh, DATANH, y, (&x));
  return y;
#endif
}

double
xerf (double x)
{
#if defined (HAVE_ERF)
  return erf (x);
#else
  double y;
  F77_YXFCN (derf, DERF, y, (x));
  return y;
#endif
}

double
xerfc (double x)
{
#if defined (HAVE_ERFC)
  return erfc (x);
#else
  double y;
  F77_YXFCN (derfc, DERFC, y, (x));
  return y;
#endif
}

double
xisnan (double x)
{
#if defined (HAVE_ISNAN)
  return (double) (isnan (x) != 0);
#else
  return 0;
#endif
}

double
xfinite (double x)
{
#if defined (HAVE_FINITE)
  return (double) (finite (x) != 0);
#elif defined (HAVE_ISINF) && defined (HAVE_ISNAN)
  return (double) (! isinf (x) && ! isnan (x));
#else
  return 1;
#endif
}

double
xisinf (double x)
{
#if defined (HAVE_ISINF)
  return (double) isinf (x);
#elif defined (HAVE_FINITE) && defined (HAVE_ISNAN)
  return (double) (! (finite (x) || isnan (x)));
#else
  return 0;
#endif
}

// Complex -> double mappers.

double
xisnan (const Complex& x)
{
#if defined (HAVE_ISNAN)
  double rx = real (x);
  double ix = imag (x);
  return (double) (isnan (rx) || isnan (ix));
#else
  return 0;
#endif
}

double
xfinite (const Complex& x)
{
  double rx = real (x);
  double ix = imag (x);
  return (double) ((int) xfinite (rx) && (int) xfinite (ix));
}

double
xisinf (const Complex& x)
{
  double rx = real (x);
  double ix = imag (x);
  return (double) ((int) xisinf (rx) || (int) xisinf (ix));
}

// Complex -> complex mappers.

Complex
acos (const Complex& x)
{
  static Complex i (0, 1);

  return (real (x) * imag (x) < 0.0) ? i * acosh (x) : -i * acosh (x);
}

Complex
acosh (const Complex& x)
{
  Complex retval = log (x + sqrt (x*x - 1.0));
  return retval;
}

Complex
asin (const Complex& x)
{
  static Complex i (0, 1);
  Complex retval = -i * log (i*x + sqrt (1.0 - x*x));
  return retval;
}

Complex
asinh (const Complex& x)
{
  Complex retval = log (x + sqrt (x*x + 1.0));
  return retval;
}

Complex
atan (const Complex& x)
{
  static Complex i (0, 1);
  Complex retval = i * log ((i + x) / (i - x)) / 2.0;
  return retval;
}

Complex
atanh (const Complex& x)
{
  static Complex i (0, 1);
  Complex retval = log ((1.0 + x) / (1.0 - x)) / 2.0;
  return retval;
}

Complex
ceil (const Complex& x)
{
  int re = (int) ceil (real (x));
  int im = (int) ceil (imag (x));
  return Complex (re, im);
}

Complex
fix (const Complex& x)
{
  return Complex (fix (real (x)), fix (imag (x)));
}

Complex
floor (const Complex& x)
{
  int re = (int) floor (real (x));
  int im = (int) floor (imag (x));
  return Complex (re, im);
}

Complex
log10 (const Complex& x)
{
  return M_LOG10E * log (x);
}

Complex
round (const Complex& x)
{
  double re = D_NINT (real (x));
  double im = D_NINT (imag (x));
  return Complex (re, im);
}

Complex
signum (const Complex& x)
{
  return x / abs (x);
}

Complex
tan (const Complex& x)
{
  Complex retval = sin (x) / cos (x);
  return retval;
}

Complex
tanh (const Complex& x)
{
  Complex retval = sinh (x) / cosh (x);
  return retval;
}

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
