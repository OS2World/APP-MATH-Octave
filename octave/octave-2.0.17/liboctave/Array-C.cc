/*

Copyright (C) 1996 John W. Eaton

This file is part of Octave.

Octave is free software; you can redistribute it and/or modify it
under the terms of the GNU General Public License as published by the
Free Software Foundation; either version 2, or (at your option) any
later version.

Octave is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
for more details.

You should have received a copy of the GNU General Public License
along with Octave; see the file COPYING.  If not, write to the Free
Software Foundation, 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.

*/

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

// Instantiate Arrays of Complex values.

#include "oct-cmplx.h"

#include "Array.h"
#include "Array.cc"

template class Array<Complex>;

template int assign (Array<Complex>&, const Array<Complex>&);
template int assign (Array<Complex>&, const Array<double>&);
template int assign (Array<Complex>&, const Array<int>&);
template int assign (Array<Complex>&, const Array<short>&);
template int assign (Array<Complex>&, const Array<char>&);

#include "Array2.h"
#include "Array2.cc"

template class Array2<Complex>;

template int assign (Array2<Complex>&, const Array2<Complex>&);
template int assign (Array2<Complex>&, const Array2<double>&);
template int assign (Array2<Complex>&, const Array2<int>&);
template int assign (Array2<Complex>&, const Array2<short>&);
template int assign (Array2<Complex>&, const Array2<char>&);

#include "DiagArray2.h"
#include "DiagArray2.cc"

template class DiagArray2<Complex>;

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
