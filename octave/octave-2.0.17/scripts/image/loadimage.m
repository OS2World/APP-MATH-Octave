## Copyright (C) 1996 Klaus Gebhardt
##
## Load an image file.
##
## [X, map] = loadimage (img_file, fmt) loads an image and its associated
## color map from file img_file.
##
## SEE ALSO: saveimage, load, save

## Author: Klaus Gebhardt <gebhardt@crunch.ikp.physik.th-darmstadt.de>
## Created: November 1996

function [X, map] = loadimage (filename, fmt)

  if (nargin != 1 && nargin != 2)
    usage ("loadimage (filename[, fmt])");
  elseif (! isstr (filename))
    error ("loadimage: expecting filename as a string");
  endif

  if (nargin == 1)
    fmt = "img";
  endif

  if (! isstr (fmt))
    error ("loadimage: expecting fmt as a string");
  endif

  file = file_in_path (IMAGEPATH, filename);

  if (isempty (file))
    error ("loadimage: unable to find image file");
  endif

  eval (sprintf ("[X, map] = %s_dec (file);", fmt));

endfunction
