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

#if !defined (octave_ComplexRowVector_h)
#define octave_ComplexRowVector_h 1

#if defined (__GNUG__)
#pragma interface
#endif

#include "MArray.h"

#include "mx-defs.h"

class ComplexRowVector : public MArray<Complex>
{
friend class ComplexColumnVector;

public:

  ComplexRowVector (void) : MArray<Complex> () { }
  ComplexRowVector (int n) : MArray<Complex> (n) { }
  ComplexRowVector (int n, const Complex& val) : MArray<Complex> (n, val) { }
  ComplexRowVector (const RowVector& a);
  ComplexRowVector (const MArray<Complex>& a) : MArray<Complex> (a) { }
  ComplexRowVector (const ComplexRowVector& a) : MArray<Complex> (a) { }

  ComplexRowVector& operator = (const ComplexRowVector& a)
    {
      MArray<Complex>::operator = (a);
      return *this;
    }

  bool operator == (const ComplexRowVector& a) const;
  bool operator != (const ComplexRowVector& a) const;

  // destructive insert/delete/reorder operations

  ComplexRowVector& insert (const RowVector& a, int c);
  ComplexRowVector& insert (const ComplexRowVector& a, int c);

  ComplexRowVector& fill (double val);
  ComplexRowVector& fill (const Complex& val);
  ComplexRowVector& fill (double val, int c1, int c2);
  ComplexRowVector& fill (const Complex& val, int c1, int c2);

  ComplexRowVector append (const RowVector& a) const;
  ComplexRowVector append (const ComplexRowVector& a) const;

  ComplexColumnVector hermitian (void) const;  // complex conjugate transpose.
  ComplexColumnVector transpose (void) const;

  friend ComplexRowVector conj (const ComplexRowVector& a);

  // resize is the destructive equivalent for this one

  ComplexRowVector extract (int c1, int c2) const;

  // row vector by row vector -> row vector operations

  ComplexRowVector& operator += (const RowVector& a);
  ComplexRowVector& operator -= (const RowVector& a);

  ComplexRowVector& operator += (const ComplexRowVector& a);
  ComplexRowVector& operator -= (const ComplexRowVector& a);

  // row vector by scalar -> row vector operations

  friend ComplexRowVector operator + (const ComplexRowVector& a, double s);
  friend ComplexRowVector operator - (const ComplexRowVector& a, double s);
  friend ComplexRowVector operator * (const ComplexRowVector& a, double s);
  friend ComplexRowVector operator / (const ComplexRowVector& a, double s);

  friend ComplexRowVector operator + (const RowVector& a, const Complex& s);
  friend ComplexRowVector operator - (const RowVector& a, const Complex& s);
  friend ComplexRowVector operator * (const RowVector& a, const Complex& s);
  friend ComplexRowVector operator / (const RowVector& a, const Complex& s);

  // scalar by row vector -> row vector operations

  friend ComplexRowVector operator + (double s, const ComplexRowVector& a);
  friend ComplexRowVector operator - (double s, const ComplexRowVector& a);
  friend ComplexRowVector operator * (double s, const ComplexRowVector& a);
  friend ComplexRowVector operator / (double s, const ComplexRowVector& a);

  friend ComplexRowVector operator + (const Complex& s, const RowVector& a);
  friend ComplexRowVector operator - (const Complex& s, const RowVector& a);
  friend ComplexRowVector operator * (const Complex& s, const RowVector& a);
  friend ComplexRowVector operator / (const Complex& s, const RowVector& a);

  // row vector by matrix -> row vector

  friend ComplexRowVector operator * (const ComplexRowVector& a,
				      const ComplexMatrix& b);

  friend ComplexRowVector operator * (const RowVector& a,
				      const ComplexMatrix& b);

  // row vector by row vector -> row vector operations

  friend ComplexRowVector operator + (const ComplexRowVector& a,
				      const RowVector& b);
  friend ComplexRowVector operator - (const ComplexRowVector& a,
				      const RowVector& b);

  friend ComplexRowVector operator + (const RowVector& a,
				      const ComplexRowVector& b);
  friend ComplexRowVector operator - (const RowVector& a,
				      const ComplexRowVector& b);

  friend ComplexRowVector product (const ComplexRowVector& a,
				   const RowVector& b);
  friend ComplexRowVector quotient (const ComplexRowVector& a,
				    const RowVector& b);

  friend ComplexRowVector product (const RowVector& a,
				   const ComplexRowVector& b);
  friend ComplexRowVector quotient (const RowVector& a,
				    const ComplexRowVector& b);

  // other operations

  ComplexRowVector map (c_c_Mapper f) const;
  RowVector map (d_c_Mapper f) const;

  ComplexRowVector& apply (c_c_Mapper f);

  Complex min (void) const;
  Complex max (void) const;

  // i/o

  friend ostream& operator << (ostream& os, const ComplexRowVector& a);
  friend istream& operator >> (istream& is, ComplexRowVector& a);

private:

  ComplexRowVector (Complex *d, int l) : MArray<Complex> (d, l) { }
};

// row vector by column vector -> scalar

Complex operator * (const ComplexRowVector& a, const ColumnVector& b);

Complex operator * (const ComplexRowVector& a, const ComplexColumnVector& b);

// other operations

ComplexRowVector linspace (const Complex& x1, const Complex& x2, int n);

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
