
#ifndef _INVPACKTREE_INT_H_
#define _INVPACKTREE_INt_H_


/** \file 

  The documentation in this file is formatted for doxygen
  (see www.doxygen.org).

<h4>
   Copyright and Use
</h4>

<p>
   You may use this source code without limitation and without
   fee as long as you include:
</p>
<blockquote>
     This software was written and is copyrighted by Ian Kaplan, Bear
     Products International, www.bearcave.com, 2002.
</blockquote>
<p>
   This software is provided "as is", without any warranty or
   claim as to its usefulness.  Anyone who uses this source code
   uses it at their own risk.  Nor is any support provided by
   Ian Kaplan and Bear Products International.
<p>
   Please send any bug fixes or suggested source changes to:
<pre>
     iank@bearcave.com
</pre>

  @author Ian Kaplan

 */


#include "liftbase.h"
#include "list.h"
#include "packcontainer_int.h"
#include "packdata.h"
#include "packdata_list.h"


/**
  Inverse wavelet packet transform

  The invpacktree_int constructor is passed a packdata_list object and a
  wavelet transform object.  It calculates the inverse wavelet 
  packet transform, using the data in the packdata_list object
  and the inverse wavelet transform step function of the wavelet
  transform object.  The best basis data is destroyed in calculating
  the inverse transform.

  The packdata_list object contains the "best basis" result from a
  wavelet packet transform.  The wavelet packet transform should have
  been calculated with the same wavelet transform as the object passed
  to this constructor.

  After the constructor completes, the data result can be
  obtained by calling the getData() function.

  The wavelet transforms used by this object are all derived
  from the liftbase class and are "lifting scheme" wavelet
  transforms.

  I have found the wavelet literature difficult, in general.  When it
  comes to the wavelet packet transform I have found most of it
  impossible to understand.  Impossible, that it until I got the book
  <i>Ripples in Mathematics</i> by Jensen and la Cour-Harbo, Springer
  Verlag, 2001.  The wavelet packet transform, for which this class
  is the inverse, is heavily based on Chapter 8 of <i>Ripples in 
  Mathematics</i>.

  I have found very little material on the actual implementation of
  the inverse wavelet packet transform.  This algorithm is my own
  design.

 */
class invpacktree_int {
private:
  /** wavelet transform object */
  liftbase<packcontainer_int, int> *waveObj;
  /** disallow the copy constructor */
  invpacktree_int( const invpacktree_int &rhs ) {}

  /** inverse wavelet packet transform calculation stack */
  LIST<packcontainer_int *> stack;

  /** pointer to the inverse transform result */
  const int *data;
  /** length of data */
  size_t N;

private:
  void new_level( packdata<int> *elem );
  void add_elem( packdata<int> *elem );
  void reduce();

public:
  invpacktree_int( packdata_list<int> &list, 
		   liftbase<packcontainer_int, int> *w );
  /** The destructor does nothing */
  ~invpacktree_int() {}

  /** Get the result of the inverse packet transform */
  const int *getData() { return data; }
  void pr();
}; // invpacktree_int

#endif
