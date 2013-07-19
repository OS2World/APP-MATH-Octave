// img_dec.cc: Decoding Octaves IMG format
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

DEFUN_DLD (img_dec, args, ,
  "img_dec (FILE)\n\
\n\
Decode Octaves IMG format.")
{
  octave_value_list retval;
  retval (1) = -1.0;
  retval (0) = -1.0;

  char name[32];
  char type[32];
  char rows[32];
  char cols[32];

  int nargin = args.length ();

  if (nargin == 1)
    {
      if (!args(0).is_string ())
	{
	  ::error ("img_dec: first argument must be a string");
	  return retval;
	}

      string filename = args(0).string_value ();

      FILE *fp = fopen (filename.c_str (), "r");

      if (fp)
	{
	  INT map_nr = -1;
	  INT map_nc = -1;

	  fgets (name, 32, fp);
	  fgets (type, 32, fp);
	  fgets (rows, 8, fp);
	  fscanf (fp, "%i\n", &map_nr);
	  fgets (cols, 11, fp);
	  fscanf (fp, "%i\n", &map_nc);

	  if ((map_nc != 3) || (map_nr < 1))
	    {
	      ::error ("img_dec: colormap must be a real N x 3 matrix");
	      fclose (fp);
	      return retval;
	    }

	  INT c, d;
	  OCTAVE x;
	  Matrix Map (map_nr, map_nc);

	  for (c = 0; c < map_nr; c++)
	    {
	      for (d = 0; d < map_nc; d++)
		{
		  fscanf (fp, "%lf ", &x);
		  Map(c, d) = x;
		}
	    }

	  UINT img_nr = 0;
	  UINT img_nc = 0;

	  fgets (name, 32, fp);
	  fgets (type, 32, fp);
	  fgets (rows, 8, fp);
	  fscanf (fp, "%i\n", &img_nr);
	  fgets (cols, 11, fp);
	  fscanf (fp, "%i\n", &img_nc);

	  if ((img_nc < 1) || (img_nr < 1))
	    {
	      ::error ("img_dec: image must be a real matrix");
	      fclose (fp);
	      return retval;
	    }

	  UINT i, j;
	  Matrix X (img_nr, img_nc);

	  for (i = 0; i < img_nr; i++)
	    {
	      for (j = 0; j < img_nc; j++)
		{
		  fscanf (fp, "%lf", &x);
		  X (i, j) = x;
		}
	    }

	  fclose (fp);

	  retval(1) = Map;
	  retval(0) = X;
	}
      else
	::error ("img_dec: unable to open \"%s\"", filename.c_str ());
    }
  else
    print_usage ("img_dec");

  return retval;
}
