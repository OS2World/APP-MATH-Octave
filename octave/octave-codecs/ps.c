/*
*******************************************************************************
** Codecs for PS                                                             **
** (c) Klaus Gebhardt, 1997                                                  **
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>

#include "oct-img.h"

#define MARGIN 0.95

static BOOL PS_writeheader (FILE *fp, char* name, UINT y, UINT x, INT grey)
{
  INT iy, ix, devpix;
  OCTAVE pixfac, sy, sx, llx, lly, scale = 1.0;
  INT turnflag = 0;
  INT turnokflag = 1;

  iy = y;
  ix = x;
  if ( turnflag || ( turnokflag && y > x ) )
    {
      turnflag = 1;
      y = ix;
      x = iy;
    }

  devpix = 300.0 / 72.0 + 0.5;
  pixfac = 72.0 / 300.0 * devpix;

  sy = scale * y * pixfac;
  sx = scale * x * pixfac;

  if ((sy > 612 * MARGIN) || (sx > 762 * MARGIN))
    {
      if (sy > 612 * MARGIN)
	{
	  scale *= 612 / sy * MARGIN;
	  sy = scale * y * pixfac;
	  sx = scale * x * pixfac;
	}

      if (sx > 762 * MARGIN)
	{
	  scale *= 762 / sx * MARGIN;
	  sy = scale * y * pixfac;
	  sx = scale * x * pixfac;
	}
    }

  llx = (612 - sy) / 2;
  lly = (762 - sx) / 2;

  if (fprintf (fp, "%%!PS-Adobe-2.0 EPSF-2.0\n") == EOF)  return -1;
  if (fprintf (fp, "%%%%Creator: PS_Encode\n") == EOF)      return -1;
  if (fprintf (fp, "%%%%Title: %s.ps\n", name) == EOF)    return -1;
  if (fprintf (fp, "%%%%Pages: 1\n") == EOF)              return -1;
  if (fprintf (fp, "%%%%BoundingBox: %d %d %d %d\n", (int) llx, (int) lly,
	       (int) (llx + sy + 0.5), (int) (lly + sx + 0.5)) == EOF)
    return -1;
  if (fprintf (fp, "%%%%EndComments\n") == EOF)  return -1;
  if (fprintf (fp, "/readstring {\n") == EOF)    return -1;
  if (fprintf (fp, "  currentfile exch readhexstring pop\n") == EOF)
    return -1;
  if (fprintf (fp, "} bind def\n") == EOF)  return -1;

  if (grey == OCT_RGB)
    {
      if (fprintf (fp, "/rpicstr %d string def\n", iy) == EOF)  return -1;
	  
      if (fprintf (fp, "/gpicstr %d string def\n", iy) == EOF)  return -1;
      if (fprintf (fp, "/bpicstr %d string def\n", iy) == EOF)  return -1;
    }
  else
    if (fprintf (fp, "/picstr %d string def\n", iy) == EOF)  return -1;

  if (fprintf (fp, "%%%%EndProlog\n") == EOF)              return -1;
  if (fprintf (fp, "%%%%Page: 1 1\n") == EOF)              return -1;
  if (fprintf (fp, "gsave\n") == EOF)                      return -1;
  if (fprintf (fp, "%g %g translate\n", llx, lly) == EOF)  return -1;
  if (fprintf (fp, "%g %g scale\n", sy, sx) == EOF)        return -1;
  if (turnflag)
    if (fprintf (fp, "0.5 0.5 translate  90 rotate  -0.5 -0.5 translate\n")
	== EOF)  return -1;
  if (fprintf (fp, "%d %d %d\n", iy, ix, 8) == EOF)              return -1;
  if (fprintf (fp, "[ %d 0 0 -%d 0 %d ]\n", iy, ix, ix) == EOF)  return -1;

  if (grey == OCT_RGB)
    {
      if (fprintf (fp, "{ rpicstr readstring }\n") == EOF)  return -1;
      if (fprintf (fp, "{ gpicstr readstring }\n") == EOF)  return -1;
      if (fprintf (fp, "{ bpicstr readstring }\n") == EOF)  return -1;
      if (fprintf (fp, "true 3\n") == EOF)      return -1;
      if (fprintf (fp, "colorimage\n") == EOF)  return -1;
    }
  else
    {
      if (fprintf (fp, "{ picstr readstring }\n") == EOF) return -1;
      if (fprintf (fp, "image\n") == EOF)                 return -1;
    }

  return 0;
}


static BOOL PS_writerow (FILE *fp, INT *r, UINT x, INT n, UCHAR **cm, INT *ct)
{
  UINT i;
  INT c;

  for (c = 0; c < n; c++)
    {
      for (i = 0; i < x; i++)
	{
	  if (*ct == 30)
	    {
	      if (fprintf (fp, "\n") == EOF)  return -1;
	      *ct = 0;
	    }

	  if (fprintf (fp, "%02X", cm[c][r[i]]) == EOF)  return -1;
	  (*ct)++;
	}
    }
}


static BOOL PS_writerest (FILE *fp)
{
  if (fprintf (fp, "\ngrestore\nshowpage\n%%%%Trailer\n") == EOF)  return -1;
  return 0;
}


BOOL PS_Encode (FILE *fp, INT grey, UINT x, UINT y, INT **pixels,
		INT colors, UCHAR **cm)
{
  UINT j;
  INT ct = 0;

  if (PS_writeheader (fp, "noname", x, y, grey) == -1)  return -1;

  for (j = 0; j < y; j++)
    {
      if (grey == OCT_RGB)
	{
	  if (PS_writerow (fp, pixels[j], x, 3, cm, &ct) == -1)  return -1;
	}
      else
	{
	  if (PS_writerow (fp, pixels[j], x, 1, cm, &ct) == -1)  return -1;
	}
    }

  PS_writerest(fp);

  return 0;
}
