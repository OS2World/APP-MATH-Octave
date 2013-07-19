/* 
** Some IEEE standard 754 recommended functions and remainder and sqrt for 
** supporting the C elementary functions.
*******************************************************************************
** WARNING:
**      These codes are developed (in double) to support the C elementary
** functions temporarily. They are not universal, and some of them are very
** slow (in particular, drem and sqrt is extremely inefficient). Each 
** computer system should have its implementation of these functions using 
** its own assembler.
*******************************************************************************
**
** IEEE 754 required operations:
**     drem(x,p) 
**              returns  x REM y  =  x - [x/y]*y , where [x/y] is the integer
**              nearest x/y; in half way case, choose the even one.
**     sqrt(x) 
**              returns the square root of x correctly rounded according to 
**		the rounding mod.
**
** IEEE 754 recommended functions:
** (a) copysign(x,y) 
**              returns x with the sign of y. 
** (b) scalb(x,N) 
**              returns  x * (2**N), for integer values N.
** (c) logb(x) 
**              returns the unbiased exponent of x, a signed integer in 
**              double precision, except that logb(0) is -INF, logb(INF) 
**              is +INF, and logb(NAN) is that NAN.
** (d) finite(x) 
**              returns the value TRUE if -INF < x < +INF and returns 
**              FALSE otherwise.
**
**
** CODED IN C BY K.C. NG, 11/25/84;
** REVISED BY K.C. NG on 1/22/85, 2/13/85, 3/24/85.
*/

#define MERLIN_MATH
#include "../merlin.h"

extern double _merlin_inf();

static const unsigned short msign=0x7fff, mexp =0x7ff0;
static const short prep1=54, gap=4, bias=1023;
static const double novf=1.7E308, nunf=3.0E-308;

double merlin_logb(double x)
{
  short *px=(short *) &x+3, k;

  if(((unsigned short) (k= *px & mexp )) != mexp )
    {
      if ( k != 0 )
	return ( (k>>gap) - bias );
      else if( x != 0.0)
	return ( -1022.0 );
      else        
	return(-_merlin_inf());    
    }
  else if(x != x)
    return(x);
  else
    {*px &= msign; return(x);}
}
