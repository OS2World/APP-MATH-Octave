/*
** ASINH(X)
** RETURN THE INVERSE HYPERBOLIC SINE OF X
**
** Required system supported functions :
**	copysign(x,y)
**	sqrt(x)
**
** Required kernel function:
**	log1p(x) 		...return log(1+x)
**
** Method :
**	Based on 
**		asinh(x) = sign(x) * log [ |x| + sqrt(x*x+1) ]
**	we have
**	asinh(x) := x  if  1+x*x=1,
**		 := sign(x)*(log1p(x)+ln2))	 if sqrt(1+x*x)=x, else
**		 := sign(x)*log1p(|x| + |x|/(1/|x| + sqrt(1+(1/|x|)^2)) )  
**
** Accuracy:
**	asinh(x) returns the exact inverse hyperbolic sine of x nearly rounded.
**	In a test run with 52,000 random arguments on a VAX, the maximum 
**	observed error was 1.58 ulps (units in the last place).
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

double merlin_asinh(double x)
{	
  double t,s;

  if(x!=x) return(x);

  if((t=copysign(x,1.))>_MERLIN_MATH_EPS)
    {
      if(t<_MERLIN_MATH_BIG)
	{
	  s=1./t;
	  return(copysign(_merlin_log1p(t+t/(s+sqrt(1.+s*s))),x));
	}
      else	/* if |x| > big */
	{
	  s=_merlin_log1p(t)+ln2lo;
	  return(copysign(s+ln2hi,x));
	}
    }
  else	/* if |x| < small */
    return(x);
}
