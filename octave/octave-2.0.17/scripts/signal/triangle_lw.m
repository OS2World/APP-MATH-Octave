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

## usage:  retval = triangle_lw (n, b)
##
## Triangular lag window. Subfunction used for spectral density
## estimation.
  
## Author:  FL <Friedrich.Leisch@ci.tuwien.ac.at>
## Description:  Triangular lag window
  
function retval = triangle_lw (n, b)
  
  retval = 1 - (0 : n-1)' * b;
  retval = max ([retval'; (zeros (1, n))])';

endfunction

