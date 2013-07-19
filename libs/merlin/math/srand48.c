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

void merlin_srand48(long seed)
{
  merlin_rand48_seed[0] = _MERLIN_MATH_RAND48_SEED_0;
  merlin_rand48_seed[1] = (unsigned short) seed;
  merlin_rand48_seed[2] = (unsigned short) (seed >> 16);
  merlin_rand48_mult[0] = _MERLIN_MATH_RAND48_MULT_0;
  merlin_rand48_mult[1] = _MERLIN_MATH_RAND48_MULT_1;
  merlin_rand48_mult[2] = _MERLIN_MATH_RAND48_MULT_2;
  merlin_rand48_add     = _MERLIN_MATH_RAND48_ADD;
}
