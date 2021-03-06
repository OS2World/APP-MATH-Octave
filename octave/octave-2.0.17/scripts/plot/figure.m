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

## usage: figure (n)
##
## Set the current plot window to plot window N.  This function
## currently requires X11 and a recent version of gnuplot.

## Author: jwe

function f = figure (n)

  global __FIGURE_LIST__ = create_set (0);
  global __FIGURE_CALLED__ = 0;

  if (nargin == 0)
    f = max (__FIGURE_LIST__) + 1;
  else
    f = n;
  endif

  if (nargin < 2)
    if (gnuplot_has_frames)
      if (! isempty (getenv ("DISPLAY")))
	oneplot ();
	__FIGURE_LIST__ = union (__FIGURE_LIST__, f);
	eval (sprintf ("gset term x11 %d\n", f));
      else
	error ("figure: requires X11 and valid DISPLAY");
      endif
    else
      error ("figure: gnuplot doesn't appear to support this feature");
    endif
  elseif (rem (nargin, 2) == 0)
    if (! __FIGURE_CALLED__)
      __FIGURE_CALLED__ = 1;
      warning ("figure: setting figure properties is unsupported");
    endif
  else
    usage ("figure (n)");
  endif

endfunction
