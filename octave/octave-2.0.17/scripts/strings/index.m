## Copyright (C) 1996 Kurt Hornik
##
## This file is part of Octave.
##
## Octave is free software; you can redistribute it and/or modify it
## under the terms of the GNU General Public License as published by
## the Free Software Foundation; either version 2, or (at your option)
## any later version.
##
## Octave is distributed in the hope that it will be useful, but
## WITHOUT ANY WARRANTY; without even the implied warranty of
## MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
## General Public License for more details.
##
## You should have received a copy of the GNU General Public License
## along with Octave; see the file COPYING.  If not, write to the Free
## Software Foundation, 59 Temple Place - Suite 330, Boston, MA
## 02111-1307, USA.

## usage:  index (s, t)
##
## Returns the position of the first occurence of the string T in the
## string S or 0 if no occurence is found.
##
## NOTE: this function does not work for arrays of strings.

## Author: Kurt Hornik <Kurt.Hornik@ci.tuwien.ac.at>
## Adapted-By: jwe

function n = index (s, t)

  ## This is patterned after the AWK function of the same name.

  if (nargin != 2)
    usage ("index (s, t)");
  endif

  n = 0;

  if (isstr (s) && isstr (t))

    [nr_s, l_s] = size (s);
    [nr_t, l_t] = size (t);

    if (nr_s != 1 || nr_t != 1)
      error ("index: arguments cannot be string arrays");
    endif

    if (l_t <= l_s)
      tmp = l_s - l_t + 1;
      for idx = 1 : tmp
	if (strcmp (substr (s, idx, l_t), t))
	  n = idx;
	  return;
	endif
      endfor
    endif

  else
    error ("index: expecting string arguments");
  endif

endfunction
