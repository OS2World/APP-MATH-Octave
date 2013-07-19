// bmp_enc.cc: Writing an OS/2 bitmap file.
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

DEFUN_DLD (bmp_enc, args, ,
  "bmp_enc (FILE, COLORMAP, IMG [,TYPE])\n\
\n\
Write the image in OS/2 or Windows bitmap format to the file FILE.")
{
  double retval = -1.0;

  UINT nr, nc;
  INT colors;
  UCHAR **cm;
  INT **x;
  INT type = BMP_OS2;

  int nargin = args.length ();

  if (nargin == 4)
    {
      if (!args(3).is_string ())
	{
	  ::error ("bmp_enc: fourth argument must be a string");
	  return retval;
	}
      
      if (args(3).string_value () == "windows")   type = BMP_WIN;
      else if (args(3).string_value () == "os2")  type = BMP_OS2;
      else
	{
	  ::error ("bmp_enc: fourth argument must be \"windows\" or \"os2\"");
	  return retval;
	}
    }

  if ((nargin == 3) || (nargin == 4))
    {
      if (!args(0).is_string ())
	{
	  ::error ("bmp_enc: first argument must be a string");
	  return retval;
	}

      string filename = args(0).string_value ();

      FILE *fp = fopen (filename.c_str (), "wb");

      if (fp)
	{
	  cm = gif_colormap (args(1), &colors, NULL);
	  if (!cm)
	    {
	      fclose (fp);
	      ::error ("bmp_enc: error calculating colormap");
	      return retval;
	    }

	  x = gif_pixels (args(2), colors, &nr, &nc);
	  if (!x)
	    {
	      free (cm);
	      fclose (fp);
	      ::error ("bmp_enc: error calculating pixels");
	      return retval;
	    }

          if (BMP_Encode (fp, type, nc, nr, x, colors, cm) != 0)
	    {
	      free (cm);
	      free (x);
	      fclose (fp);
	      ::error ("bmp_enc: error writing bitmap");
	      return retval;
	    }
#
	  free (cm);
	  free (x);
	  fclose (fp);

	  retval = 0.0;
	}
      else
	::error ("bmp_enc: unable to open \"%s\"", filename.c_str ());
    }
  else
    print_usage ("bmp_enc");

  return retval;
}
