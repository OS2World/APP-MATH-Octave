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

## usage:  binomial_rnd (n, p [, r, c])
##
## binomial_rnd (n, p) returns a matrix of random samples from the
## binomial distribution with parameters n and p.  The size of the
## matrix is the common size of n and p.
##
## binomial_rnd (n, p, r, c) returns an r by c matrix of random samples
## from the binomial distribution with parameters n and p. Both n and p
## must be scalar or of size r by c.
  
## Author:  KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  Random deviates from the binomial distribution

function rnd = binomial_rnd (n, p, r, c)

  if (nargin == 4)
    if ( !(is_scalar (r) && (r > 0) && (r == round (r))) )
      error ("binomial_rnd:  r must be a positive integer");
    endif
    if ( !(is_scalar (c) && (c > 0) && (c == round (c))) )
      error ("binomial_rnd:  c must be a positive integer");
    endif
    [retval, n, p] = common_size (n, p, zeros (r, c));
    if (retval > 0)
      error (strcat("binomial_rnd:  ",
		    "n and p must be scalar or of size ",
		    sprintf ("%d by %d", r, c)));
    endif
  elseif (nargin == 2)
    [retval, n, p] = common_size (n, p);
    if (retval > 0)
      error ("binomial_rnd:  n and p must be of common size or scalar");
    endif
  else
    usage ("binomial_rnd (n, p [, r, c])");
  endif

  [r, c] = size (n);
  s = r * c;
  n = reshape (n, 1, s);
  p = reshape (p, 1, s);
  rnd = zeros (1, s);
  
  k = find (!(n > 0) | !(n < Inf) | !(n == round (n)) |
	    !(p <= 0) | !(p >= 1));
  if any (k)
    rnd(k) = NaN * ones (1, length (k));
  endif
  
  k = find ((n > 0) & (n < Inf) & (n == round (n)) & (p >= 0) & (p <= 1));
  if any (k)
    N = max (n(k));
    L = length (k);
    tmp = rand (N, L);
    ind = (1 : N)' * ones (1, L);
    rnd(k) = sum ((tmp < ones (N, 1) * p(k)) &
		  (ind <= ones (N, 1) * n(k)));
  endif
  
  rnd = reshape (rnd, r, c);
  
endfunction