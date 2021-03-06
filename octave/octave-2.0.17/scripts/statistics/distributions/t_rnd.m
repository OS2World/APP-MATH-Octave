## Copyright (C) 1995, 1996, 1997  Kurt Hornik
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

## usage:  t_rnd (n [, r, c])
##
## t_rnd (n) returns a matrix of random samples from the t (Student)
## distribution with n degrees of freedom.  The size of the matrix is
## the size of n.
##
## t_rnd (n, r, c) returns an r by c matrix of random samples from the t
## distribution with n degrees of freedom.  n must be a scalar or of
## size r by c.
  
## Author:  KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  Random deviates from the t distribution

function rnd = t_rnd (n, r, c)

  if (nargin == 3)
    if ( !(is_scalar (r) && (r > 0) && (r == round (r))) )
      error ("t_rnd:  r must be a positive integer");
    endif
    if ( !(is_scalar (c) && (c > 0) && (c == round (c))) )
      error ("t_rnd:  c must be a positive integer");
    endif
    [retval, n] = common_size (n, zeros (r, c));
    if (retval > 0)
      error (strcat("t_rnd:  ",
		    "n must be scalar or of size ",
		    sprintf ("%d by %d", r, c)));
    endif
  elseif (nargin != 1)
    usage ("t_rnd (n [, r, c])");
  endif
  
  [r, c] = size (n);
  s = r * c;
  n = reshape (n, 1, s);
  rnd = zeros (1, s);
  
  k = find (!(n > 0) | !(n < Inf) | !(n == round (n)));
  if any (k)
    rnd(k) = NaN * ones (1, length (k));
  endif
  
  k = find ((n > 0) & (n < Inf) & (n == round (n)));
  if any (k)
    rnd(k) = t_inv (rand (1, length (k)), n(k));
  endif
  
  rnd = reshape (rnd, r, c);
  
endfunction
