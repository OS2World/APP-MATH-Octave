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

## usage: loglog (x, y)
##        loglog (x1, y1, x2, y2, ...)
##        loglog (x, y, fmt)
##
## Make a 2D plot of y versus x using a log scale for the x axis.
##
## See the help message for the plot command for a description of how
## the arguments are interpreted.
##
## See also: plot, semilogy, loglog, polar, mesh, contour, bar, stairs,
##           gplot, gsplot, replot, xlabel, ylabel, title

## Author: jwe

function loglog (...)

  ## XXX FIXME XXX -- these plot states should really just be set
  ## temporarily, probably inside an unwind_protect block, but there is
  ## no way to determine their current values.

  gset logscale x;
  gset logscale y;
  gset nopolar;

  __plt__ ("loglog", all_va_args);

endfunction
