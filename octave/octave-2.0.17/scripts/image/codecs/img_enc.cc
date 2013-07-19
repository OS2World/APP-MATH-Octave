// img_enc.cc: Writing in Octaves IMG format
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

DEFUN_DLD (img_enc, args, ,
  "img_enc (FILE, COLORMAP, IMG)\n\
\n\
Write the image in Octaves IMG format to the file FILE.")
{
  double retval = -1.0;

  int nargin = args.length ();

  if (nargin == 3)
    {
      if (!args(0).is_string ())
	{
	  ::error ("img_enc: first argument must be a string");
	  return retval;
	}

      string filename = args(0).string_value ();

      FILE *fp = fopen (filename.c_str (), "w");

      if (fp)
	{
	  octave_value map = args(1);
	  INT map_nr = map.rows ();
	  INT map_nc = map.columns ();

	  if (!map.is_matrix_type () || !map.is_real_type () ||
	      (map_nc != 3) || (map_nr < 1))
	    {
	      ::error ("img_enc: colormap must be a real N x 3 matrix");
	      return retval;
	    }

	  octave_value img = args(2);
	  UINT img_nr = img.rows ();
	  UINT img_nc = img.columns ();

	  if (!img.is_matrix_type () || !img.is_real_type () ||
	      (img_nc < 1) || (img_nr < 1))
	    {
	      ::error ("img_enc: img must be a real matrix");
	      return retval;
	    }

	  INT c;
	  Matrix Map = map.matrix_value ();

	  fprintf (fp, "# name: map\n");
	  fprintf (fp, "# name: matrix\n");
	  fprintf (fp, "# rows: %i\n",    map_nr);
	  fprintf (fp, "# columns: %i", map_nc);

	  for (c = 0; c < map_nr; c++)
	    fprintf(fp, "\n%7.5f %7.5f %7.5f",
		    Map (c, 0), Map (c, 1), Map (c, 2));

	  UINT i, j, k;
	  Matrix X   = img.matrix_value ();

	  fprintf (fp, "\n# name: X");
	  fprintf (fp, "\n# name: matrix");
	  fprintf (fp, "\n# rows: %i",    img_nr);
	  fprintf (fp, "\n# columns: %i", img_nc);

	  for (i = 0; i < img_nr; i++)
	    {
	      k = 0;

	      for (j=0; j<img_nc; j++)
		{
		  if (k == 0)     fprintf (fp, "\n");
		  if (++k == 20)  k = 0;
		  fprintf(fp, "%3.0f ", X (i, j));
		}
	    }

	  fclose (fp);

	  retval = 0.0;
	}
      else
	::error ("img_enc: unable to open \"%s\"", filename.c_str ());
    }
  else
    print_usage ("img_enc");

  return retval;
}
