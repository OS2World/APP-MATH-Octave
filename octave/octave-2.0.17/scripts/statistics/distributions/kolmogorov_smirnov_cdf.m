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

## usage:  kolmogorov_smirnov_cdf (x [, tol])
##
## Returns the CDF at x of the Kolmogorov-Smirnov distribution,
## i.e. Q(x) = sum_{k=-\infty}^\infty (-1)^k exp(-2 k^2 x^2), x > 0.
##
## The optional tol specifies the precision up to which the series
## should be evaluated;  the default is tol = eps.
  
## Author:  KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  CDF of the Kolmogorov-Smirnov distribution

function cdf = kolmogorov_smirnov_cdf (x, tol)
  
  if (nargin < 1 || nargin > 2)
    usage ("kolmogorov_smirnov_cdf (x [, tol])");
  endif

  if (nargin == 1)
    tol = eps;
  else 
    if (!is_scalar (tol) || !(tol > 0))
      error (["kolmogorov_smirnov_cdf:  ", ...
	      "tol has to be a positive scalar."]);
    endif
  endif

  [nr, nc] = size(x);
  if (min (nr, nc) == 0)
    error ("kolmogorov_smirnov_cdf:  x must not be empty.");
  endif

  n   = nr * nc;
  x   = reshape (x, 1, n);
  cdf = zeros (1, n);
  ind = find (x > 0);
  if (length (ind) > 0)
    y   = x(ind);
    K   = ceil( sqrt( - log (tol) / 2 ) / min (y) );
    k   = (1:K)';
    A   = exp( - 2 * k.^2 * y.^2 );
    odd = find (rem (k, 2) == 1);
    A(odd, :) = -A(odd, :);
    cdf(ind) = 1 + 2 * sum (A);
  endif

  cdf = reshape (cdf, nr, nc);
  
endfunction
