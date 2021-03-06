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

#if !defined (octave_idx_vector_h)
#define octave_idx_vector_h 1

#if defined (__GNUG__)
#pragma interface
#endif

class ostream;
class ColumnVector;
class Matrix;
class Range;

class
idx_vector
{
private:

  class
  idx_vector_rep
  {
  public:

    idx_vector_rep (void)
      {
	colon = 0;
	len = 0;
	num_zeros = 0;
	num_ones = 0;
	one_zero = 0;
	orig_nr = 0;
	orig_nc = 0;
	initialized = 0;
	frozen = 0;
	colon_equiv_checked = 0;
	colon_equiv = 0;
	data = 0;
      }

    idx_vector_rep (const ColumnVector& v);

    idx_vector_rep (const Matrix& m);

    idx_vector_rep (const Range& r);

    idx_vector_rep (double d);

    idx_vector_rep (char c);

    idx_vector_rep (const idx_vector_rep& a);

    ~idx_vector_rep (void) { delete [] data; }

    idx_vector_rep& operator = (const idx_vector_rep& a);

    int ok (void) { return initialized; }

    int capacity (void) const { return len; }
    int length (int colon_len) const { return colon ? colon_len : len; }

    int elem (int n) const { return colon ? n : data[n]; }

    int checkelem (int n) const;
    int operator () (int n) const { return checkelem (n); }

    int max (void) const { return max_val; }
    int min (void) const { return min_val; }

    int one_zero_only (void) const { return one_zero; }
    int zeros_count (void) const { return num_zeros; }
    int ones_count (void) const { return num_ones; }

    int is_colon (void) const { return colon; }
    int is_colon_equiv (int n, int sort_uniq);

    void sort (bool uniq);

    int orig_rows (void) const { return orig_nr; }
    int orig_columns (void) const { return orig_nc; }

    // other stuff

    void shorten (int n); // Unsafe.  Avoid at all cost.

    int freeze (int z_len, const char *tag, int prefer_zero_one,
		int resize_ok);

    // i/o

    ostream& print (ostream& os) const;

    int *data;
    int len;
    int num_zeros;
    int num_ones;
    int max_val;
    int min_val;
    int orig_nr;
    int orig_nc;
    int count;
    int frozen_at_z_len;
    int frozen_len;
    unsigned int colon : 1;
    unsigned int one_zero : 1;
    unsigned int initialized : 1;
    unsigned int frozen : 1;
    unsigned int colon_equiv_checked : 1;
    unsigned int colon_equiv : 1;

    void init_state (void);

    void maybe_convert_one_zero_to_idx (int z_len, int prefer_zero_one);
  };

public:

  idx_vector (void)
    {
      rep = new idx_vector_rep ();
      rep->count = 1;
    }

  idx_vector (const ColumnVector& v)
    {
      rep = new idx_vector_rep (v);
      rep->count = 1;
    }

  idx_vector (const Matrix& m)
    {
      rep = new idx_vector_rep (m);
      rep->count = 1;
    }

  idx_vector (const Range& r)
    {
      rep = new idx_vector_rep (r);
      rep->count = 1;
    }

  idx_vector (double d)
    {
      rep = new idx_vector_rep (d);
      rep->count = 1;
    }

  idx_vector (char c)
    {
      rep = new idx_vector_rep (c);
      rep->count = 1;
    }

  idx_vector (const idx_vector& a)
    {
      rep = a.rep;
      rep->count++;
    }

  ~idx_vector (void)
    {
      if (--rep->count <= 0)
	delete rep;
    }

  idx_vector& operator = (const idx_vector& a)
    {
      if (this != &a)
	{
	  if (--rep->count <= 0)
	    delete rep;

	  rep = a.rep;
	  rep->count++;
	}
      return *this;
    }

  operator void * () const { return (void *) rep->ok (); }

  int capacity (void) const { return rep->capacity (); }
  int length (int cl) const { return rep->length (cl); }

  int elem (int n) const { return rep->elem (n); }
  int checkelem (int n) const { return rep->checkelem (n); }
  int operator () (int n) const { return rep->operator () (n); }

  int max (void) const { return rep->max (); }
  int min (void) const { return rep->min (); }

  int one_zero_only (void) const { return rep->one_zero_only (); }
  int zeros_count (void) const { return rep->zeros_count (); }
  int ones_count (void) const { return rep->ones_count (); }

  int is_colon (void) const { return rep->is_colon (); }
  int is_colon_equiv (int n, int sort_uniq = 0) const
    { return rep->is_colon_equiv (n, sort_uniq); }

  void sort (bool uniq = false) { rep->sort (uniq); }

  int orig_rows (void) const { return rep->orig_rows (); }
  int orig_columns (void) const { return rep->orig_columns (); }

  int orig_empty (void) const
    {
      return (! is_colon ()
	      && (orig_rows () == 0 || orig_columns () == 0));
    }

// Unsafe.  Avoid at all cost.
  void shorten (int n) { rep->shorten (n); }

// i/o

  int freeze (int z_len, const char *tag, int prefer_zero_one = 0,
	      int resize_ok = 0)
    { return rep->freeze (z_len, tag, prefer_zero_one, resize_ok); }

  ostream& print (ostream& os) const { return rep->print (os); }

  friend ostream& operator << (ostream& os, const idx_vector& a)
    { return a.print (os); }

  void maybe_convert_one_zero_to_idx (int z_len, int prefer_zero_one = 0)
    { rep->maybe_convert_one_zero_to_idx (z_len, prefer_zero_one); }

private:

  idx_vector_rep *rep;

  void init_state (void) { rep->init_state (); }
};

#endif

/*
;;; Local Variables: ***
;;; mode: C++ ***
;;; End: ***
*/
