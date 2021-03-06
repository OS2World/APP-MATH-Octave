## Copyright (C) 1995, 1996  Kurt Hornik
## 
## This program is free software; you can redistribute it and/or modify
## it under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
## 
## This program is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details. 
## 
## You should have received a copy of the GNU General Public License
## along with this file.  If not, write to the Free Software Foundation,
## 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

## usage: y = shift (x, b)
##
## If x is a vector, perform a circular shift of length b of the
## elements of x.
##
## If x is a matrix, do the same for each column of x.

## Author: AW <Andreas.Weingessel@ci.tuwien.ac.at>
## Created: 14 September 1994
## Adapted-By: jwe

function y = shift (x, b)
  
  if (nargin != 2)
    error ("usage: shift (X, b)");
  endif

  [nr, nc] = size (x);
  
  if (nr == 0 || nc == 0)
    error ("shift: x must not be empty");
  elseif (nr == 1)
    x = x.';
    nr = nc;
    nc = 0;
  endif

  if (! (is_scalar (b) && b == round (b)))
    error ("shift: b must be an integer");
  endif

  if (b >= 0)
    b = rem (b, nr);
    y = [(x (nr-b+1:nr, :)); (x (1:nr-b, :))];
  elseif (b < 0)
    b = rem (abs (b), nr);
    y = [(x (b+1:nr, :)); (x (1:b, :))];
  endif

  if (nc == 0)
    y = reshape (y, 1, nr);
  endif

endfunction
