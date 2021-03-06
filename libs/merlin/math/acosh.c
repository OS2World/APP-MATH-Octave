/*
** ACOSH(X)
** RETURN THE INVERSE HYPERBOLIC COSINE OF X
**
** Required system supported functions :
**	sqrt(x)
**
** Required kernel function:
**	log1p(x) 		...return log(1+x)
**
** Method :
**	Based on 
**		acosh(x) = log [ x + sqrt(x*x-1) ]
**	we have
**		acosh(x) := log1p(x)+ln2,	if (x > 1.0E20); else
**		acosh(x) := log1p( sqrt(x-1) * (sqrt(x-1) + sqrt(x+1)) ) .
**	These formulae avoid the over/underflow complication.
**
** Special cases:
**	acosh(x) is NaN with signal if x<1.
**	acosh(NaN) is NaN without signal.
**
** Accuracy:
**	acosh(x) returns the exact inverse hyperbolic cosine of x nearly 
**	rounded. In a test run with 512,000 random arguments on a VAX, the
**	maximum observed error was 3.30 ulps (units of the last place) at
**	x=1.0070493753568216 .
**
** Constants:
** The hexadecimal values are the intended ones for the following constants.
** The decimal values may be used, provided that the compiler will convert
** from decimal to binary accurately enough to produce the hexadecimal values
** shown.
*/

#define MERLIN_MATH
#include "../merlin.h"

static const double ln2hi = 6.9314718036912381649E-1;
static const double ln2lo = 1.9082149292705877000E-10;

extern double _merlin_log1p(double);

double merlin_acosh(double x)
{	
  double t;

  if(x!=x) return(x);

  if(x>_MERLIN_MATH_BIG) {t=_merlin_log1p(x)+ln2lo; return(t+ln2hi);} 

  t=sqrt(x-1.);
  return(_merlin_log1p(t*(t+sqrt(x+1.))));
}
