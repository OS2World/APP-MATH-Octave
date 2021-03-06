## Copyright (C) 1995, 1996, 1997  Friedrich Leisch
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

## usage:  retval = triangle_sw (n, b)
##
## Triangular spectral window.  Subfunction used for spectral density
## estimation.
  
## Author:  FL <Friedrich.Leisch@ci.tuwien.ac.at>
## Description:  Triangular spectral window

function retval = triangle_sw (n, b)
  
  retval = zeros(n,1);
  retval(1) = 1 / b;

  l = (2:n)' - 1;
  l = 2 * pi * l / n;
  
  retval(2:n) = b * (sin (l / (2*b)) ./ sin (l / 2) ).^2;

endfunction






























