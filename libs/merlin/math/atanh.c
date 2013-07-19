/*
** ATANH(X)
** RETURN THE HYPERBOLIC ARC TANGENT OF X
**
** Required kernel function:
**	log1p(x) 	...return log(1+x)
**
** Method :
**	Return 
**                          1              2x                          x
**		atanh(x) = --- * log(1 + -------) = 0.5 * log1p(2 * --------)
**                          2             1 - x                      1 - x
**
** Special cases:
**	atanh(x) is NaN if |x| > 1 with signal;
**	atanh(NaN) is that NaN with no signal;
**	atanh(+-1) is +-INF with signal.
**
** Accuracy:
**	atanh(x) returns the exact hyperbolic arc tangent of x nearly rounded.
**	In a test run with 512,000 random arguments on a VAX, the maximum
**	observed error was 1.87 ulps (units in the last place) at
**	x= -3.8962076028810414000e-03.
*/

#define MERLIN_MATH
#include "../merlin.h"

extern double _merlin_log1p(double);

double merlin_atanh(double x)
{
  double z,t;
  z = copysign(.5,x);
  t = copysign(x,1.);
  t = t/(1.-t);
  return( z*_merlin_log1p(t+t) );
}
