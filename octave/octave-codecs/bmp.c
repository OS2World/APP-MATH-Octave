/*
*******************************************************************************
** Codec for Windows and OS/2 bitmaps                                        **
** (c) 1997, Klaus Gebhardt                                                  **
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>

#include "bitio.h"
#include "oct-img.h"

extern UINT PutByte   (FILE *, UCHAR);
extern UINT PutLShort (FILE *, SHORT);
extern UINT PutLLong  (FILE *, INT);

extern UINT GetByte   (FILE *, UCHAR *);
extern UINT GetLShort (FILE *, SHORT *);
extern UINT GetLLong  (FILE *, INT *);

extern SHORT MaxValtoBits (INT);

extern UCHAR **malloc_uchar_matrix (UINT, UINT);
extern INT   **malloc_int_matrix   (UINT, UINT);


static UINT BMP_offbits (INT class, SHORT bpp)
{
  if (bpp < 1)  return 0;

  switch (class)
    {
    case BMP_WIN:
      return 54 + (1 << bpp) * 4;
    case BMP_OS2:
      return 26 + (1 << bpp) * 3;
    default:
      return 0;
    }
}


static UINT BMP_lenfile (INT class, SHORT bpp, UINT x, UINT y)
{
  UINT bits;

  switch (class)
    {
    case BMP_WIN:
      break;
    case BMP_OS2:
      break;
    default:
      return 0;
    }

  bits = x * bpp;

  if ((bits % 32) != 0)  bits += (32 - (bits % 32));
  if ((bits % 32) != 0)  return 0;

  return BMP_offbits (class, bpp) + y * (bits >> 3);
}


static UINT BMP_writergb (FILE *fp, INT class, UCHAR R, UCHAR G, UCHAR B)
{
  UINT nbyte = 0;

  switch (class)
    {
    case BMP_WIN:
      nbyte += PutByte (fp, B);
      nbyte += PutByte (fp, G);
      nbyte += PutByte (fp, R);
      nbyte += PutByte (fp, 0);
      return nbyte;
    case BMP_OS2:
      nbyte += PutByte (fp, B);
      nbyte += PutByte (fp, G);
      nbyte += PutByte (fp, R);
      return nbyte;
    default:
      return 0;
    }

  return 0;
}


static UINT BMP_writergbtable (FILE *fp, INT class, SHORT bpp,
			      INT colors, UCHAR *R, UCHAR *G, UCHAR *B)
{
  UINT nbyte = 0;
  INT i, ncolors;

  for (i = 0; i < colors; i++)
    nbyte += BMP_writergb (fp, class, R[i], G[i], B[i]);

  ncolors = (1 << bpp);

  for (; i < ncolors; i++)
    nbyte += BMP_writergb (fp, class, 0, 0, 0);

  return nbyte;
}


static int BMP_writerow (FILE *fp, INT *row, UINT cx, SHORT bpp)
{
  BITSTREAM b;
  UINT nbyte = 0, x;
  int rc;

  if ((b = bitinit (fp, "w")) == (BITSTREAM) 0)  return -1;

  for (x = 0; x < cx; x++)
    {
      if ((rc = bitwrite (b, bpp, (UCHAR) row[x])) == -1)  return -1;
      nbyte += rc;
    }

  if ((nbyte += bitfini (b)) == -1)  return -1;

  while (nbyte % 4)
    {
      PutByte (fp, 0);
      nbyte++;
    }

  return nbyte;
}


static UINT BMP_writebits (FILE *fp, UINT cx, UINT cy, SHORT bpp, INT **pixels)
{
  UINT nbyte = 0, y;
  int rc;

  if(bpp > 8)  return 0;

  for (y = cy; y > 0; y--)
    {
      if ((rc = BMP_writerow (fp, pixels[y-1], cx, bpp)) % 4)  return 0;
      nbyte += rc;
    }

  return nbyte;
}


BOOL BMP_Encode (FILE *fp, INT class, UINT x, UINT y, INT **pixels,
		INT colors, UCHAR **cm)
{
  SHORT bpp;
  UINT nbyte = 0;

  if (colors < 1)                             return -1;
  if ((bpp = MaxValtoBits (colors - 1)) > 8)  return -1;

  switch(bpp)
    {
    case 2:
    case 3:
      bpp = 4;
      break;
    case 5:
    case 6:
    case 7:
      bpp = 8;
      break;
    }

  nbyte += PutByte (fp, 'B');
  nbyte += PutByte (fp, 'M');
  nbyte += PutLLong (fp, BMP_lenfile (class, bpp, x, y));
  nbyte += PutLShort (fp, 0);
  nbyte += PutLShort (fp, 0);
  nbyte += PutLLong (fp, BMP_offbits (class, bpp));

  switch (class)
    {
    case BMP_WIN:
      nbyte += PutLLong (fp, 40);
      nbyte += PutLLong (fp, x);
      nbyte += PutLLong (fp, y);
      nbyte += PutLShort (fp, 1);
      nbyte += PutLShort (fp, bpp);
      nbyte += PutLLong (fp, 0);
      nbyte += PutLLong (fp, 0);
      nbyte += PutLLong (fp, 0);
      nbyte += PutLLong (fp, 0);
      nbyte += PutLLong (fp, 0);
      nbyte += PutLLong (fp, 0);
      break;
    case BMP_OS2:
      nbyte += PutLLong (fp, 12);
      nbyte += PutLShort (fp, x);
      nbyte += PutLShort (fp, y);
      nbyte += PutLShort (fp, 1);
      nbyte += PutLShort (fp, bpp);
      break;
    default:
      return -1;
    }

  nbyte += BMP_writergbtable (fp, class, bpp, colors, cm[0], cm[1], cm[2]);

  if (nbyte != BMP_offbits (class, bpp))  return -1;

  nbyte += BMP_writebits (fp, x, y, bpp, pixels);
  if (nbyte != BMP_lenfile (class, bpp, x, y))  return -1;

  return 0;
}

static int BMP_readrow (FILE *fp, INT *row, UINT cx, SHORT bpp,
			INT *col_min, INT *col_max)
{
  BITSTREAM b;
  UINT nbyte = 0, x, c;
  int rc;

  if ((b = bitinit (fp, "r")) == (BITSTREAM) 0)  return -1;

  for (x = 0; x < cx; x++)
    {
      if ((rc = bitread (b, bpp, &c)) == -1)  return -1;
      nbyte += rc;
      *col_min = min (*col_min, (INT) c);
      *col_max = max (*col_max, (INT) c);
      row[x] = (INT) c;
    }

  if ((rc = bitfini (b)) == -1)  return -1;
  nbyte += rc;

  while (nbyte % 4)
    nbyte += SkipBytes (fp, 1);

  return nbyte;
}

static UINT BMP_readbits (FILE *fp, UINT cx, UINT cy, SHORT bpp,
			  INT *col_min, INT *col_max, INT **pixels)
{
  UINT y, nbyte = 0;
  int rc;

  for (y = cy; y > 0; y--)
    {
      if ((rc = BMP_readrow (fp, pixels[y-1], cx, bpp, col_min, col_max))%4)
	return 0;
      nbyte += rc;
    }

  return nbyte;
}

BOOL BMP_Decode (FILE *fp, UINT *x, UINT *y, INT *col_min, INT *col_max,
		 UCHAR ***cm, INT ***pixels)
{
  UCHAR c;
  SHORT n, bpp;
  UINT nbyte = 0, rc, len, off;
  INT class;
  INT cbfix, i;

  *col_max = 0;
  *col_min = 256;

  nbyte += GetByte (fp, &c);  if (c != 'B')  return -1;
  nbyte += GetByte (fp, &c);  if (c != 'M')  return -1;

  nbyte += GetLLong (fp, &len);
  nbyte += SkipBytes (fp, 4);
  nbyte += GetLLong (fp, &off);
  nbyte += GetLLong (fp, &cbfix);

  if (cbfix == 12)
    {
      class = BMP_OS2;
      nbyte += GetLShort (fp, &n);   *x = n;
      nbyte += GetLShort (fp, &n);   *y = n;
      nbyte += GetLShort (fp, &n);
      nbyte += GetLShort (fp, &bpp);
    }
  else if (cbfix == 40)
    {
      class = BMP_WIN;
      nbyte += GetLLong (fp, x);
      nbyte += GetLLong (fp, y);
      nbyte += GetLShort (fp, &n);
      nbyte += GetLShort (fp, &bpp);
      nbyte += SkipBytes (fp, 24);
    }
  else  return -1;

  if ((n != 1) || ((bpp != 1) && (bpp != 4) && (bpp != 8)))  return -1;

  if (!(*cm = malloc_uchar_matrix (3, (1 << bpp))))  return -1;

  for (i = 0; i < (1 << bpp); i++)
    {
      nbyte += GetByte (fp, &c);  (*cm)[2][i] = c;
      nbyte += GetByte (fp, &c);  (*cm)[1][i] = c;
      nbyte += GetByte (fp, &c);  (*cm)[0][i] = c;
      if (cbfix == 40)  nbyte += SkipBytes (fp, 1);
    }

  if (nbyte != BMP_offbits (class, bpp))  return -1;

  if (!(*pixels = malloc_int_matrix (*y, *x)))
    {
      free (*cm);
      return -1;
    }

  nbyte += BMP_readbits (fp, *x, *y, bpp, col_min, col_max, *pixels);

  if (nbyte != BMP_lenfile (class, bpp, *x, *y))
    {
      free (*cm);
      free (*pixels);
      return -1;
    }

  return 0;
}
