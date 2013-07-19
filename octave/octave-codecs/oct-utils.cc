/*
*******************************************************************************
** Interface functions to Octave                                             **
** (c) 1997, Klaus Gebhardt                                                  **
*******************************************************************************
*/

#include "oct-img.h"

extern "C"
{
  UCHAR **malloc_uchar_matrix (UINT, UINT);
  INT   **malloc_int_matrix   (UINT, UINT);
}


UCHAR **gif_colormap (octave_value map, INT *colors, BOOL *grey)
{
  UCHAR **cmap;
  INT i;

  if (grey)  *grey = OCT_GREY;
  *colors = (INT) map.rows ();

  if (!map.is_matrix_type () || !map.is_real_type () ||
      (((unsigned long) map.columns ()) != 3) || ((*colors) < 1))
    {
      ::error ("gif_colormap: colormap must be a real N x 3 matrix");
      return NULL;
    }

  Matrix Map = map.matrix_value ();

  cmap = malloc_uchar_matrix (3, (*colors));
  if (cmap)
    {
      for (i = 0; i < (*colors); i++)
	{
	  cmap[0][i] = (UCHAR) (min (max (Map (i, 0), 0), 1) * 255.);
	  cmap[1][i] = (UCHAR) (min (max (Map (i, 1), 0), 1) * 255.);
	  cmap[2][i] = (UCHAR) (min (max (Map (i, 2), 0), 1) * 255.);
	  if (grey)
	    if ((cmap[0][i] != cmap[1][i]) || (cmap[0][i] != cmap[2][i]))
	      *grey = OCT_RGB;
	}

      if (grey && ((*grey) == OCT_GREY) && ((*colors) < 3))
	{
	  if (((*colors) == 1) && ((cmap[0][0] == 0) || (cmap[0][0] == 255)))
	    *grey = OCT_BLACKWHITE;
	  else
	    {
	      if ((cmap[0][0]*cmap[0][1] == 0) &&
		  (cmap[0][0]+cmap[0][1] == 255))
		*grey = OCT_BLACKWHITE;
	    }
	}
    }
  else
    ::error ("gif_colormap: out of memory");

  return cmap;
}


void oct_colormap (Matrix& Map, UCHAR **cmap, INT col_min, INT col_max)
{
  INT i;

  for (i = col_min; i < col_max + 1; i++)
    {
      Map (i - col_min, 0) = ((OCTAVE) cmap[0][i]) / 255.;
      Map (i - col_min, 1) = ((OCTAVE) cmap[1][i]) / 255.;
      Map (i - col_min, 2) = ((OCTAVE) cmap[2][i]) / 255.;
    }

  free (cmap);
}


INT **gif_pixels (octave_value img, INT colors, UINT *nr, UINT *nc)
{
  INT **x;
  INT c;
  UINT i, j;

  *nr = (UINT) img.rows ();
  *nc = (UINT) img.columns ();

  if (colors < 1)
    {
      ::error ("gif_pixels: number of colors must be positive");
      return NULL;
    }

  if (!img.is_matrix_type () || !img.is_real_type () ||
      ((*nc) < 1) || ((*nr) < 1))
    {
      ::error ("gif_pixels: img must be a real matrix");
      return NULL;
    }

  Matrix X = img.matrix_value ();

  x = malloc_int_matrix ((*nr), (*nc));
  if (x)
    {
      for (i = 0; i < (*nr); i++)
	{
	  for (j = 0; j < (*nc); j++)
	    {
	      c = min (max ((INT) (X (i, j) - 1.), 0), colors - 1);
	      x[i][j] = c;
	    }
	}
    }
  else
    ::error ("gif_pixels: out of memory");

  return x;
}


void oct_pixels (Matrix& X, INT **x, INT col_min, UINT nr, UINT nc)
{
  UINT i, j;

  for (i = 0; i < nr; i++)
    for (j = 0; j < nc; j++)
      X (i, j) = ((double) (x[i][j])) - ((double) col_min) + 1.;

  free (x);
}
