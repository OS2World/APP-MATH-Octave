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

## usage:  f_cdf (x, m, n)
##
## For each element of x, compute the CDF at x of the F distribution
## with m and n degrees of freedom, i.e., PROB( F(m,n) <= x ).

## Author:  KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  CDF of the F distribution

function cdf = f_cdf (x, m, n)
  
  if (nargin != 3)
    usage ("f_cdf (x, m, n)");
  endif
  
  [retval, x, m, n] = common_size (x, m, n);
  if (retval > 0)
    error ("f_cdf:  x, m and n must be of common size or scalar");
  endif
  
  [r, c] = size (x);
  s = r * c;
  x = reshape (x, 1, s);
  m = reshape (m, 1, s);
  n = reshape (n, 1, s);
  cdf = zeros (1, s);

  k = find (!(m > 0) | !(n > 0) | isnan (x));
  if any (k)
    cdf(k) = NaN * ones (1, length (k));
  endif
  
  k = find ((x == Inf) & (m > 0) & (n > 0));
  if any (k)
    cdf(k) = ones (1, length (k));
  endif
  
  k = find ((x > 0) & (x < Inf) & (m > 0) & (n > 0));
  if any (k)
    cdf(k) = 1 - betai (n(k) / 2, m(k) / 2, ...
	1 ./ (1 + m(k) .* x(k) ./ n(k)));
  endif
  
  ## should we really only allow for positive integer m, n?
  k = find ((m != round (m)) | (n != round (n)));
  if any (k)
    fprintf (stderr, ...
	     "WARNING:  m and n should be positive integers\n");
    cdf(k) = NaN * ones (1, length (k));
  endif
  
  cdf = reshape (cdf, r, c);
  
endfunction
