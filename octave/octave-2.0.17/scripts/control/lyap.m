## Copyright (C) 1996 John W. Eaton
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

## Usage: x = lyap (a, b, c)
##        x = lyap (a, c)
##
## If (a, b, c) are specified, then lyap returns the solution of the
## Sylvester equation
##
##   a x + x b + c = 0
##
## If only (a, c) are specified, then lyap returns the solution of the
## Lyapunov equation
##
##   a x + x a' + c = 0
##
## If b is not square, then lyap returns the solution of either
##
##   a' x + x a + b' b = 0
##
## or
##
##   a x + x a' + b b' = 0
##
## whichever is appropriate.
##
## Solves by using the Bartels-Stewart algorithm (1972).

## Author: A. S. Hodel <scotte@eng.auburn.edu>
## Created: August 1993
## Adapted-By: jwe

function x = lyap (a, b, c)

  if (nargin != 3 && nargin != 2)
    usage ("lyap (a, b, c)  or  lyap (a, c)");
  endif

  if ((n = is_square(a)) == 0)
    error ("lyap: a is not square");
  endif

  if (nargin == 2)

    ## Transform Lyapunov equation to Sylvester equation form.

    if ((m = is_square (b)) == 0)
      if ((m = rows (b)) == n)

	## solve a x + x a' + b b' = 0

	b = b * b';
        c = b;
        b = a';

      else

	## Try to solve a'x + x a + b' b = 0.

	m = columns (b);
	c = b' * b;
        b = a;
        a = a';

      endif

      if (m != n)
	error ("lyap: a, b not conformably dimensioned");
      endif

    else

      ## If you share your installation of Octave with others, you
      ## shouldn't remove the following code.  Instead, users should
      ## add
      ##
      ##   global __disable_lyap_interface_change_warning__;
      ##
      ## to their personal ~/.octaverc files.  This warning will likely
      ## disappear for good in some future version of Octave.

      if (! exist ("__disable_lyap_interface_change_warning__"))
	global __disable_lyap_interface_change_warning__;
	warning ("for compatibility with Matlab, lyap now computes");
	warning ("");
	warning ("  A*X + X*A' + C = 0");
	warning ("");
	warning ("instead of");
	warning ("");
	warning ("  A'*X + X*A + C = 0");
	warning ("");
	warning ("as in previous versions");
	warning ("");
	warning ("to completely disable this message, put")
	warning ("")
	warning ("  global __disable_lyap_interface_change_warning__;");
	warning ("")
	warning ("in your ~/.octaverc file")
      endif

      c = b;
      b = a';

    endif

  else

    ## Check dimensions.

    if ((m = is_square (b)) == 0)
      error ("lyap: b must be square in a sylvester equation");
    endif

    [n1, m1] = size(c);

    if (n != n1 || m != m1)
      error("lyap: a, b, c not conformably dimensioned");
    endif
  endif

  ## Call octave built-in function.

  x = syl (a, b, c);

endfunction
