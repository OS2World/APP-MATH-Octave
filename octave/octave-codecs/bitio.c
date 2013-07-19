/*
*******************************************************************************
** Bitstream I/O                                                             **
** (c) 1997, Klaus Gebhardt                                                  **
*******************************************************************************
*/

#include "bitio.h"

struct bitstream
{
  FILE *f;                      /* bytestream */
  unsigned long bitbuf;         /* bit buffer */
  int nbitbuf;                  /* number of bits in 'bitbuf' */
  char mode;
};

#define Mask(n)		((1<<(n))-1)

#define BitPut(b,ul,n)	((b)->bitbuf = (((b)->bitbuf<<(n))	\
					|((ul)&Mask(n))),	\
			(b)->nbitbuf += (n))

#define BitGet(b,n)	(((b)->bitbuf>>((b)->nbitbuf-=(n))) & Mask(n))


struct bitstream *bitinit(FILE *f, const char *mode)
{
  struct bitstream *ans = (struct bitstream *)0;

  if(!f || !mode || !*mode)                   return ans;
  if(strcmp(mode, "r") && strcmp(mode, "w"))  return ans;

  ans = (struct bitstream *)calloc(1, sizeof(struct bitstream));
  if(ans)
    {
      ans->f = f;
      ans->mode = *mode;
    }

  return ans;
}


int bitfini(struct bitstream *b)
{
  int nbyte = 0;

  if(!b)  return -1;

  if(b->mode == 'w')
    {
      if ((b->nbitbuf < 0) || (b->nbitbuf >= 8))  return -1;

      if(b->nbitbuf)
	{
	  char	c;

	  BitPut(b, 0, (long)8-(b->nbitbuf));
	  c = (char) BitGet(b, (long)8);
	  if(putc(c, b->f) == EOF)  return -1;
	  nbyte++;
	}
    }

  free(b);
  return nbyte;
}

int bitread(struct bitstream *b, UINT nbits, UINT *val)
{
  int nbyte = 0;
  int c;

  if(!b)  return -1;

  while (b->nbitbuf < nbits)
    {
      if((c = getc(b->f)) == EOF)  return -1;
      nbyte++;
      BitPut(b, c, (long) 8);
    }

  *val = BitGet(b, nbits);
  return nbyte;
}

int bitwrite(struct bitstream *b, UINT nbits, UINT val)
{
  int nbyte = 0;
  char c;

  if(!b)  return -1;

  BitPut(b, val, nbits);

  while (b->nbitbuf >= 8)
    {
      c = (char) BitGet(b, (long)8);
      if(putc(c, b->f) == EOF)  return -1;
      nbyte++;
    }

  return nbyte;
}
