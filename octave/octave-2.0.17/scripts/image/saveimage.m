## Copyright (C) 1996 Klaus Gebhardt
##
## Save a matrix to disk in image format.
##
## saveimage (filename, x) saves matrix x to file filename in octaves
## image format.  The current colormap is saved in the file also.
##
## saveimage (filename, x, format, map) saves the image along with the
## specified colormap in the specified format.
##

## Author: Klaus Gebhardt <gebhardt@crunch.ikp.physik.th-darmstadt.de>
## Created: November 1996

function saveimage (filename, img, img_form, map, opt)

  if (nargin < 2 || nargin > 5)
    usage ("saveimage (filename, matrix, [format, [colormap, [opt]]])");
  endif

  if (nargin < 4)
    map = colormap ();
  endif

  if (nargin < 3)
    img_form = "img";
  elseif (! isstr (img_form))
    error ("image format specification must be a string");
  endif

  if (! is_matrix (img))
    warning ("image variable is not a matrix");
  endif

  if (! isstr (filename))
    error ("file name must be a string");
  endif

  if (nargin < 5)
    eval (sprintf ("%s_enc (filename, map, img);", img_form));
  else
    eval (sprintf ("%s_enc (filename, map, img, opt);", img_form));
  endif

endfunction
