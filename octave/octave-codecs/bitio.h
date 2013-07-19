/*
*******************************************************************************
** Bitstream I/O                                                             **
** (c) 1997, Klaus Gebhardt                                                  **
*******************************************************************************
*/

#ifndef _BITIO_H_
#define _BITIO_H_

#include <stdlib.h>
#include <stdio.h>

#include "oct-img.h"

typedef struct bitstream	*BITSTREAM;

extern BITSTREAM bitinit (FILE *, const char *);
extern int bitfini  (BITSTREAM);
extern int bitread  (BITSTREAM, UINT, UINT *);
extern int bitwrite (BITSTREAM, UINT, UINT);

#endif /* _BITIO_H_ */
