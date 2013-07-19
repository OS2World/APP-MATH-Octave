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

double merlin_erand48(unsigned short[3]);

double merlin_drand48(void)
{
  return merlin_erand48(merlin_rand48_seed);
}
