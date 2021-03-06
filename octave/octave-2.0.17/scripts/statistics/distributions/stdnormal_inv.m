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

## usage:  stdnormal_inv (x)
##
## For each component of x, compute compute the quantile (the inverse of
## the CDF) at x of the standard normal distribution.
  
## Author:  KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  Quantile function of the standard normal distribution
  
function inv = stdnormal_inv (x)
  
  if (nargin != 1)
    usage ("stdnormal_inv (x)");
  endif
  
  inv = sqrt (2) * erfinv (2 * x - 1);
  
endfunction
