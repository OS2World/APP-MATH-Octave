// pnm_dec.cc: Reading PNM files
// (c) Klaus Gebhardt, 1997

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

DEFUN_DLD (pnm_dec, args, ,
  "pnm_dec (FILE)\n\
\n\
Decode PNM format file FILE.")
{
  octave_value_list retval;
  retval (1) = -1.0;
  retval (0) = -1.0;

  UCHAR **cmap;
  INT **x;
  INT col_min, col_max;
  UINT i, j, nr, nc;

  int nargin = args.length ();

  if (nargin == 1)
    {
      if (!args(0).is_string ())
	{
	  ::error ("pnm_dec: first argument must be a string");
	  return retval;
	}

      string filename = args(0).string_value ();

      FILE *fp = fopen (filename.c_str (), "rb");

      if (fp)
	{
	  if (PNM_Decode (fp, &nc, &nr, &col_min, &col_max, &cmap, &x) != 0)
	    {
	      fclose (fp);
	      ::error ("pnm_dec: error reading PNM");
	      return retval;
	    }

          Matrix Map (col_max - col_min + 1, 3);
	  oct_colormap (Map, cmap, col_min, col_max);
          retval (1) = Map;

	  Matrix X (nr, nc);
	  oct_pixels (X, x, col_min, nr, nc);
	  retval (0) = X;

	  fclose (fp);
	}
      else
	::error ("pnm_dec: unable to open \"%s\"", filename.c_str ());
    }
  else
    print_usage ("pnm_dec");

  return retval;
}
