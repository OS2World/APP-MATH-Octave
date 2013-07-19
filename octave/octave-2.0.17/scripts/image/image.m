## Copyright (C) 1996 John W. Eaton
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## Display an octave image matrix.
##
## image (x) displays a matrix as a color image. The elements of x are
## indices into the current colormap and should have values between 1
## and the length of the colormap.
##
## image (x, zoom) changes the zoom factor.  The default value is 1.
##
## SEE ALSO: imshow, imagesc, colormap.## SEE ALSO: imshow, imagesc, colormap.

## Author: Tony Richardson <amr@mpl.ucsd.edu>
## Created: July 1994
## Adapted-By: jwe
## Modified by Klaus Gebhardt, 1996 - 1997

function image (x, zoom)

  if (nargin == 0)
    ## Load Bobbie Jo Richardson (Born 3/16/94)
    [x, map] = loadimage ("default.img");
  elseif (nargin == 1)
    zoom = 1;
  elseif (nargin > 2)
    usage ("image (matrix, [zoom])");
  endif

  pnm_name = tmpnam ();

  saveimage (pnm_name, x, "pnm");

  ## Start the viewer.
  i = findstr (pnm_name, "/");
  pnm_name (i) = "\\";

  system (sprintf ("oct-view %s %f", pnm_name, zoom));

endfunction
