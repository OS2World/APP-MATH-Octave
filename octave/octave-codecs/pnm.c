/*
*******************************************************************************
** Codecs for PNM                                                            **
** (c) Klaus Gebhardt, 1997                                                  **
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>

#include "oct-img.h"

extern UINT PutByte (FILE *, UCHAR);
extern UINT GetByte (FILE *, UCHAR *);

extern UINT GetBShort (FILE *, SHORT *);

extern UINT ScanUCHAR (FILE *, UCHAR *);
extern UINT ScanUINT  (FILE *, UINT *);

extern UCHAR **malloc_uchar_matrix (UINT, UINT);
extern INT   **malloc_int_matrix   (UINT, UINT);

extern UCHAR **compute_colormap (INT **, UINT, UINT, INT *);


static UINT PNM_writepixels (FILE *fp, UINT x, UINT y, INT **pixels,
			     INT colors, UCHAR **cm, INT n)
{
  UINT i, j, nbyte = 0;
  INT k;

  for (j = 0; j < y; j++)
    for (i = 0; i < x; i++)
      for (k = 0; k < n; k++)
	nbyte += PutByte (fp, cm[k][pixels[j][i]]);

  return nbyte;
}


BOOL PNM_Encode (FILE *fp, INT grey, UINT x, UINT y, INT **pixels,
		 INT colors, UCHAR **cm)
{
  UINT nbyte = 0;

  if ((x == 0) || (y == 0) || (colors == 0)) return -1;

  if (grey != OCT_RGB)
    {
      fprintf (fp, "P5\n%i %i\n255\n", x, y);
      nbyte += PNM_writepixels (fp, x, y, pixels, colors, cm, 1);
      nbyte += PutByte (fp, '\n');

      if (nbyte != (x * y + 1))  return -1;
      return 0;
    }
  else
    {
      fprintf (fp, "P6\n%i %i\n255\n", x, y);
      nbyte += PNM_writepixels (fp, x, y, pixels, colors, cm, 3);
      nbyte += PutByte (fp, '\n');

      if (nbyte != (3 * x * y + 1))  return -1;
      return 0;
    }
}


static BOOL PNM_readinit (FILE *fp, UINT *y, UINT *x, UINT *m, SHORT *fmt)
{
  if (GetBShort (fp, fmt) != 2)  return -1;

  switch (*fmt)
    {
    case PPM_TYPE:
    case RPPM_TYPE:
      if (ScanUINT (fp, x) == 0)  return -1;
      if (ScanUINT (fp, y) == 0)  return -1;
      if (ScanUINT (fp, m) == 0)  return -1;
      if ((*m) > PPM_MAXVAL)      return -1;
      return 0;

    case PGM_TYPE:
    case RPGM_TYPE:
      if (ScanUINT (fp, x) == 0)  return -1;
      if (ScanUINT (fp, y) == 0)  return -1;
      if (ScanUINT (fp, m) == 0)  return -1;
      if ((*m) > PGM_MAXVAL)      return -1;
      return 0;

    case PBM_TYPE:
    case RPBM_TYPE:
      if (ScanUINT (fp, x) == 0)  return -1;
      if (ScanUINT (fp, y) == 0)  return -1;
      *m = PBM_MAXVAL;
      return 0;

    default:
      return -1;
    }
}


static BOOL PNM_readrow (FILE *fp, INT *row, UINT cx, UINT maxval, SHORT fmt)
{
  UINT x;
  UCHAR r, g, b;
  int bitshift = -1;

  switch (fmt)
    {
    case PPM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if (ScanUCHAR (fp, &r) == 0)  return -1;
	  if (ScanUCHAR (fp, &g) == 0)  return -1;
	  if (ScanUCHAR (fp, &b) == 0)  return -1;
	  row[x] = (((INT) r) << 16) | (((INT) g) << 8) | ((INT) b);
	}
      return 0;

    case RPPM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if (GetByte (fp, &r) != 1)  return -1;
	  if (GetByte (fp, &g) != 1)  return -1;
	  if (GetByte (fp, &b) != 1)  return -1;
	  row[x] = (((INT) r) << 16) | (((INT) g) << 8) | ((INT) b);
	}
      return 0;

    case PGM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if (ScanUCHAR (fp, &g) == 0)  return -1;
	  row[x] = (((INT) g) << 16) | (((INT) g) << 8) | ((INT) g);
	}
      return 0;

    case RPGM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if (GetByte (fp, &g) != 1)  return -1;
	  row[x] = (((INT) g) << 16) | (((INT) g) << 8) | ((INT) g);
	}
      return 0;

    case PBM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if (ScanUCHAR (fp, &g) == 0)  return -1;
	  g = (g == 0) ? 255 : 0;
	  row[x] = (((INT) g) << 16) | (((INT) g) << 8) | ((INT) g);
	}
      return 0;

    case RPBM_TYPE:
      for (x = 0; x < cx; x++)
	{
	  if ( bitshift == -1 )
	    {
	      if (GetByte (fp, &g) != 1)  return -1;
	      bitshift = 7;
	    }
	  b = (((g >> bitshift) & 1) == 0) ? 255 : 0;
	  row[x] = (((INT) b) << 16) | (((INT) b) << 8) | ((INT) b);
	  --bitshift;
	}
      return 0;

    default:
      return -1;
    }
}


BOOL PNM_Decode (FILE *fp, UINT *x, UINT *y, INT *col_min, INT *col_max,
		 UCHAR ***cm, INT ***pixels)
{
  UINT r, m;
  SHORT fmt;

  *cm = NULL;
  *col_min = 0;
  *col_max = 0;

  PNM_readinit (fp, y, x, &m, &fmt );

  if ((*pixels = malloc_int_matrix (*y, *x)) == NULL)  return -1;

  for (r = 0; r < *y; r++)
    if (PNM_readrow (fp, (*pixels)[r], *x, m, fmt) == -1)
      {
	free (*pixels);
	return -1;
      }

  if ((*cm = compute_colormap (*pixels, *y, *x, col_max)) == NULL)
    {
      free (*pixels);
      return -1;
    }

  return 0;
}
