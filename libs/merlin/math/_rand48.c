/*
 * Copyright (c) 1993 Martin Birgmeier
 * All rights reserved.
 *
 * You may redistribute unmodified or modified versions of this source
 * code provided that the above copyright notice and this and the
 * following conditions are retained.
 *
 * This software is provided ``as is'', and comes with no warranties
 * of any kind. I shall in no event be liable for anything that happens
 * to anyone/anything when using this software.
 */

/* Modified by Klaus Gebhardt, 1997 */

#define MERLIN_MATH
#include "../merlin.h"

unsigned short merlin_rand48_seed[3] =
{
  _MERLIN_MATH_RAND48_SEED_0,
  _MERLIN_MATH_RAND48_SEED_1,
  _MERLIN_MATH_RAND48_SEED_2
};

unsigned short merlin_rand48_mult[3] =
{
  _MERLIN_MATH_RAND48_MULT_0,
  _MERLIN_MATH_RAND48_MULT_1,
  _MERLIN_MATH_RAND48_MULT_2
};

unsigned short merlin_rand48_add = _MERLIN_MATH_RAND48_ADD;

void merlin_dorand48(unsigned short xseed[3])
{
  unsigned long accu;
  unsigned short temp[2];

  accu = (unsigned long) merlin_rand48_mult[0] * (unsigned long) xseed[0] +
    (unsigned long) merlin_rand48_add;

  temp[0] = (unsigned short) accu;	/* lower 16 bits */

  accu >>= sizeof(unsigned short) * 8;
  accu += (unsigned long) merlin_rand48_mult[0] * (unsigned long) xseed[1] +
    (unsigned long) merlin_rand48_mult[1] * (unsigned long) xseed[0];

  temp[1] = (unsigned short) accu;	/* middle 16 bits */

  accu >>= sizeof(unsigned short) * 8;
  accu += merlin_rand48_mult[0] * xseed[2] + merlin_rand48_mult[1] * xseed[1] +
    merlin_rand48_mult[2] * xseed[0];

  xseed[0] = temp[0];
  xseed[1] = temp[1];
  xseed[2] = (unsigned short) accu;
}
