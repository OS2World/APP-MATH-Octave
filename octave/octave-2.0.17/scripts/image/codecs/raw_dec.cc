// raw_dec.cc: Decode raw image files
// (c) George White, 1997
// (c) Klaus Gebhardt, 1997
// George White <aa056@chebucto.ns.ca>
// Klaus Gebhardt <gebhardt@crunch.ikp.physik.th-darmstdt.de>

#include <octave/config.h>

#include <iostream.h>

#include <octave/lo-utils.h>
#include <octave/lo-ieee.h>
#include <octave/mx-base.h>
#include <octave/str-vec.h>

#include <octave/defun-dld.h>
#include <octave/error.h>
#include <octave/gripes.h>
#include <octave/help.h>
#include <octave/mappers.h>
#include <octave/oct-fstrm.h>
#include <octave/oct-iostrm.h>
#include <octave/oct-map.h>
#include <octave/oct-obj.h>
#include <octave/oct-prcstrm.h>
#include <octave/oct-stream.h>
#include <octave/oct-strstrm.h>
#include <octave/ops.h>
#include <octave/ov-base.h>
#include <octave/ov-typeinfo.h>
#include <octave/ov.h>
#include <octave/ov-base.h>
#include <octave/ov-re-mat.h>
#include <octave/pager.h>
#include <octave/pr-output.h>
#include <octave/symtab.h>
#include <octave/variables.h>

#include <octave/oct-img.h>

#ifdef __EMX__
static __inline__ unsigned short int
swapbytes(unsigned short int x)
{
  register unsigned short int tmp __asm__ ("ax") = x;
  __asm__ __volatile__ ("xchgb %%al,%%ah\n\t"   /* swap bytes */
                : "=a" (tmp) : "a" (tmp));
  return(tmp);
}
#else
static __inline__ unsigned short int
swapbytes(unsigned short int x)
{
	return ( x<<8 ) | ( x >>8 );
}
#endif

DEFUN_DLD (raw_dec, args, ,
  "raw_dec nh rows cols bpp (FILE)\n\
\n\
Decode raw image formats.\n\
  nh -- number of header bytes\n\
  rows -- number of Y-pixels\n\
  cols -- number of X-pixels\n\
  bpp -- bits per pixel (8 or [-]16)\n")
{
  octave_value_list retval;
  retval (1) = -1.0;
  retval (0) = -1.0;

  UINT nh, rows, cols;
  int bpp;
  int nargin = args.length ();

  if (nargin == 5)
    {
      if (!args(0).is_real_scalar ())
	{
	  ::error ("raw_dec: first argument (header bytes) must be a number");
	  return retval;
	}
      nh = (unsigned int) args(0).double_value ();
      if (!args(1).is_real_scalar ())
	{
	  ::error ("raw_dec: second argument (rows) must be a number");
	  return retval;
	}
      rows = (unsigned int) args(1).double_value ();
      if (!args(2).is_real_scalar ())
	{
	  ::error ("raw_dec: third argument (cols) must be a number");
	  return retval;
	}
      cols = (unsigned int) args(2).double_value ();
      if (!args(3).is_real_scalar ())
	{
	  ::error ("raw_dec: forth argument (bpp) must be a number");
	  return retval;
	}
      bpp = (int) args(3).double_value ();
      if ( !((bpp == 8)||(bpp==16)||(bpp==-16)) )
        {
	  ::error ("raw_dec: forth argument (bpp) must be 8 or [-]16");
	  return retval;
	}

      if (!args(4).is_string ())
	{
	  ::error ("raw_dec: last argument must be a string");
	  return retval;
	}

      string filename = args(4).string_value ();

      FILE *fp = fopen (filename.c_str (), "r");

// dummy color map:
      if (fp)
	{
	  INT map_nr = 1;
	  INT map_nc = 3;

	  INT c, d;
	  OCTAVE x;
	  Matrix Map (map_nr, map_nc);

	  for (c = 0; c < map_nr; c++)
	    {
	      for (d = 0; d < map_nc; d++)
		{
		  Map(c, d) = 0;
		}
	    }

	  if ((cols < 1) || (rows < 1))
	    {
	      ::error ("raw_dec: image must be a real matrix");
	      fclose (fp);
	      return retval;
	    }

	  UINT i, j;
          unsigned char uc;
          unsigned short int us;
	  Matrix X (rows, cols);

          if ( nh > 0 )
		fseek(fp, (long)nh, SEEK_SET);

	  for (i = 0; i < rows; i++)
	    {
	      for (j = 0; j < cols; j++)
		{
		  if ( bpp == 8 )
                     {
                       fread((char *)&uc, 1, 1, fp);
                       X (i, j) = (double) uc;
                     }
                  else /* |bpp| = 16 */
                     {
                       fread((char*)&us, 2, 1, fp);
                       if ( bpp == -16 )
		          X (i, j) = (double) (short int) swapbytes(us);
                       else
  		          X (i, j) = (double) (short int) us;
                     }
		}
	    }

	  fclose (fp);

	  retval (1) = Map;
	  retval (0) = X;
	}
      else
	::error ("raw_dec: unable to open \"%s\"", filename.c_str ());
    }
  else
    print_usage ("raw_dec");

  return retval;
}
