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

## usage:  iqr (x)
##
## If x is a vector, return the interquartile range, i.e., the
## difference between the upper and lower quartile, of the input data.
##
## If x is a matrix, do the above for each column of x.
  
## Author KH <Kurt.Hornik@ci.tuwien.ac.at>
## Description:  Interquartile range

function y = iqr (x)
  
  if (nargin != 1)
    usage ("iqr (x)");
  endif

  y = empirical_inv (3/4, x) - empirical_inv (1/4, x);
  
endfunction