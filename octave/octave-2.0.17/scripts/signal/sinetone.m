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

## usage:  sinetone (freq [, rate [, sec [, ampl]]])
##
## Returns a sinetone of frequency freq with length of sec seconds at
## sampling rate rate and with amplitude ampl.  freq and ampl may be
## vectors of common size.
##
## Defaults are rate = 8000, sec = 1 and ampl = 64.
    
## Author:  FL <Friedrich.Leisch@ci.tuwien.ac.at>
## Description:  Compute a sine tone
  
function retval = sinetone (f, r, s, a)
  
  if (nargin == 1)
    r = 8000;
    s = 1;
    a = 64;
  elseif (nargin == 2)
    s = 1;
    a = 64;
  elseif (nargin == 3)
    a = 64;
  elseif ((nargin < 1) || (nargin > 4))
    usage ("sinetone (freq [, rate [, sec [, ampl]]])");
  endif
  
  [err, f, a] = common_size (f, a);
  if (err || ! is_vector (f))
    error ("sinetone:  freq and ampl must be vectors of common size");
  endif
  
  if !(is_scalar (r) && is_scalar (s))
    error ("sinetone:  rate and sec must be scalars");
  endif

  n = length (f);

  retval = zeros (r * s, n);
  for k = 1:n
    retval (:, k) = a(k) * sin (2 * pi * (1:r*s) / r * f(k))';
  endfor
  
endfunction

