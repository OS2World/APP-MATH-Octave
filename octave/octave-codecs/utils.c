/*
*******************************************************************************
** Utilities used for reading and writing of image files                     **
** (c) 1997, Klaus Gebhardt                                                  **
*******************************************************************************
*/

#include <stdlib.h>
#include <stdio.h>

#include "oct-img.h"


UCHAR **malloc_uchar_matrix (UINT m, UINT n)
{
  UINT i;
  UCHAR **p;

  p = (UCHAR **) malloc (m * sizeof (UCHAR *) + m * n * sizeof (UCHAR));
  if (p)
    {
      p[0] = (UCHAR *) (p + m);
      for (i = 1; i < m; i++)  p[i] = p[i-1] + n;
    }

  return p;
}


INT **malloc_int_matrix (UINT m, UINT n)
{
  UINT i;
  INT **p;

  p = (INT **) malloc (m * sizeof (INT *) + m * n * sizeof (INT));
  if (p)
    {
      p[0] = (INT *) (p + m);
      for (i = 1; i < m; i++)  p[i] = p[i-1] + n;
    }

  return p;
}


SHORT MaxValtoBits (INT maxval)
{
  if (maxval <= 1)            return 1;
  else if (maxval <= 3)       return 2;
  else if (maxval <= 7)       return 3;
  else if (maxval <= 15)      return 4;
  else if (maxval <= 31)      return 5;
  else if (maxval <= 63)      return 6;
  else if (maxval <= 127)     return 7;
  else if (maxval <= 255)     return 8;
  else if (maxval <= 511)     return 9;
  else if (maxval <= 1023)    return 10;
  else if (maxval <= 2047)    return 11;
  else if (maxval <= 4095)    return 12;
  else if (maxval <= 8191)    return 13;
  else if (maxval <= 16383)   return 14;
  else if (maxval <= 32767)   return 15;
  else if (maxval <= 65535L)  return 16;
  else                        return -1;
}


UINT SkipBytes (FILE *fp, UINT skip)
{
  if (fseek (fp, (long) skip, SEEK_CUR) != 0)  return 0;
  return skip;
}


UINT PutByte (FILE *fp, UCHAR c)
{
  if (fputc (c, fp) == EOF)  return 0;
  return 1;
}

UINT PutLShort (FILE *fp, SHORT s)
{
  if (fputc ((s & 0xff), fp) == EOF)       return 0;
  if (fputc ((s >> 8) & 0xff, fp) == EOF)  return 0;
  return 2;
}

UINT PutLLong (FILE *fp, INT i)
{
  if (fputc((i & 0xff), fp) == EOF)        return 0;
  if (fputc((i >> 8) & 0xff, fp) == EOF)   return 0;
  if (fputc((i >> 16) & 0xff, fp) == EOF)  return 0;
  if (fputc((i >> 24) & 0xff, fp) == EOF)  return 0;
  return 4;
}


UINT GetByte (FILE *fp, UCHAR *c)
{
  if (fread (c, 1, 1, fp) != 1)  return 0;
  return 1;
}

UINT GetLShort (FILE *fp, SHORT *s)
{
  UCHAR c;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *s = c & 0xff;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *s |= (c & 0xff) << 8;
  return 2;
}

UINT GetBShort (FILE *fp, SHORT *s)
{
  UCHAR c;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *s = (c & 0xff) << 8;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *s |= c & 0xff;
  return 2;
}

UINT GetLLong (FILE *fp, INT *i)
{
  UCHAR c;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *i = c & 0xff;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *i |= (c & 0xff) << 8;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *i |= (c & 0xff) << 16;
  if (fread (&c, 1, 1, fp) != 1)  return 0;
  *i |= (c & 0xff) << 24;
  return 4;
}

static UINT _scanuchar (FILE *fp, UCHAR *c)
{
  int ich;
  char ch;

  if ((ich = getc (fp)) == EOF )  return 0;
  ch = (char) ich;

  if (ch == '#')
    do
      {
	if ((ich = getc (fp)) == EOF)  return 0;
	ch = (char) ich;
      }
    while ((ch != '\n') && (ch != '\r'));

  *c = ch;
  return 1;
}

UINT ScanUCHAR (FILE *fp, UCHAR *c)
{
  UCHAR ch;

  do
    if (_scanuchar (fp, &ch) == 0)  return 0;
  while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

  if ((ch < '0' || ch > '9'))  return 0;

  *c = 0;
  do
    {
      *c = *c * 10 + ch - ((UCHAR) '0');
      if (_scanuchar (fp, &ch) == 0)  return 0;
    }
  while ((ch >= '0') && (ch <= '9'));

  return 1;
}

UINT ScanUINT (FILE *fp, UINT *c)
{
  UCHAR ch;

  do
    if (_scanuchar (fp, &ch) == 0)  return 0;
  while ( ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r' );

  if ((ch < '0' || ch > '9'))  return 0;

  *c = 0;
  do
    {
      *c = *c * 10 + ch - ((UCHAR) '0');
      if (_scanuchar (fp, &ch) == 0)  return 0;
    }
  while ((ch >= '0') && (ch <= '9'));

  return 1;
}

typedef struct
{
  INT c;
  INT n;
  void *next;
}
TOC;

UCHAR **compute_colormap (INT **pixels, UINT x, UINT y, INT *col_max)
{
  TOC first = { -1, 0, NULL };
  TOC *last  = NULL;
  TOC *p, *q;
  UINT i, j;
  INT d, n = 0;
  UCHAR **cm = NULL;

  *col_max = 0;

  for (i = 0; i < x; i++)
    {
      for (j = 0; j < y; j++)
	{
	  d = pixels[i][j];
	  p = &first;

	  while ((p->next) && (((TOC *) (p->next))->c < d))
	    p = (TOC *) p->next;

	  if ((p->next) && (((TOC *) (p->next))->c == d))
	    pixels[i][j] = ((TOC *) (p->next))->n;
	  else
	    {
	      if ((q = malloc (sizeof(TOC))) == NULL)  goto end;

	      q->c = d;             q->n = n;
	      q->next = p->next;    p->next = q;
	      pixels[i][j] = n;     n++;
	    }
	}
    }

  if ((cm = malloc_uchar_matrix (3, (UINT) n)) == NULL)  goto end;

  *col_max = n - 1;
  p = first.next;

  while (p)
    {
      n = p->n;
      d = p->c;    cm[2][n] = ((UCHAR) d) % 256;
      d = d >> 8;  cm[1][n] = ((UCHAR) d) % 256;
      d = d >> 8;  cm[0][n] = ((UCHAR) d) % 256;
      p = (TOC *) p->next;
    }

end:
  p = first.next;

  while (p)
    {
      q = (TOC *) p->next;
      free (p);
      p = q;
    }

  return cm;
}
