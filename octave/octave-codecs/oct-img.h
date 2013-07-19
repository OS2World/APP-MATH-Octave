/*
*******************************************************************************
** Header for Octave/2 Image processing                                      **
** (c) 1997, Klaus Gebhardt, 1997 - 1999                                     **
*******************************************************************************
*/

#ifndef _OCT_IMG_H
#define _OCT_IMG_H

typedef int           BOOL;    /* Boolean type   */
typedef unsigned char UCHAR;   /* Colormaps      */
typedef short         SHORT;   /* Bits per Pixel */
typedef int           INT;     /* Colours        */
typedef unsigned int  UINT;    /* Sizes          */
typedef double        OCTAVE;  /* Octaves type   */

#define OCT_BLACKWHITE 0
#define OCT_GREY       1
#define OCT_RGB        2

#if defined (__cplusplus)
#include <octave/config.h>
#include <octave/variables.h>

UCHAR **gif_colormap (octave_value, INT *, BOOL *);
INT   **gif_pixels   (octave_value, INT, UINT *, UINT *);

void oct_colormap (Matrix&, UCHAR **, INT, INT);
void oct_pixels   (Matrix&, INT **, INT, UINT, UINT);
#endif

#if defined (__cplusplus)
extern "C" {
#endif

#include <stdio.h>

#ifndef max
#define max(a,b)             (((a) > (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b)             (((a) < (b)) ? (a) : (b))
#endif

  /* Windows und OS/2 Bitmaps */
#define BMP_MAXCOLORS  256
#define BMP_WIN          1
#define BMP_OS2          2

  BOOL BMP_Encode (FILE *, INT, UINT, UINT, INT **, INT, UCHAR **);
  BOOL BMP_Decode (FILE *, UINT *, UINT *, INT *, INT *, UCHAR ***, INT ***);

  /* PNM format */
#define PPM_MAXVAL   255
#define PPM_TYPE       'P' * 256 + '3'
#define RPPM_TYPE      'P' * 256 + '6'
#define PGM_MAXVAL   255
#define PGM_TYPE       'P' * 256 + '2'
#define RPGM_TYPE      'P' * 256 + '5'
#define PBM_MAXVAL     1
#define PBM_TYPE       'P' * 256 + '1'
#define RPBM_TYPE      'P' * 256 + '4'

  BOOL PNM_Encode (FILE *, INT, UINT, UINT, INT **, INT, UCHAR **);
  BOOL PNM_Decode (FILE *, UINT *, UINT *, INT *, INT *, UCHAR ***, INT ***);

  /* Postscript */
  BOOL PS_Encode (FILE *, INT, UINT, UINT, INT **, INT, UCHAR **);
#if defined (__cplusplus)
}
#endif

#endif
