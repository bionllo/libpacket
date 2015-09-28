#ifndef _HAAR_INT_H_
#define _HAAR_INT_H_


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

#include "stdio.h"
#include "liftbase.h"


/**
  A lifting scheme version of the Haar integer to integer transform.

  The standard wavelet transform creates real wavelet coefficients,
  even if the input data consists of integers.  This is a problem in
  lossless compression (e.g., lossless image compression) and in other
  compression related algorithm.

  This verson of the Haar wavelet transform takes an data set and
  creates an integer result.  In the case of the Lifting Scheme
  version of the Haar transform, the code is the same as the 
  real version, except that integers are used.

  This algorithm is sometimes called the S-transform in the
  image compression world.
  
  <b>References</b>

  <ol>
  <li>
  <p>
  <i>Wavelet Transforms that Map Integers to Integers</i> by
  A.R. Calderbank, ingrid daubechies, wim weldens and Boon-Lock Yeo,
  August 1996
  </p>
  <p>
  This is the central reference that was used to develop this code.
  Parts 1 and 2 of this paper are for the mathematicially
  sophisticated (which is to say, they are not light reading).
  However, for the implementer, part 3 and part 4 of this paper
  provide excellent coverage of perfectly invertable wavelet
  transforms that map integers to integers.  In fact, part 3 of this
  paper is worth reading in general for its discussion of the wavelet
  Lifting Scheme.
  </p>
  </li>

  <li>
  <p>
  <i>Ripples in Mathematics: the Discrete Wavelet Transform</i> 
  by Arne Jense and Anders la Cour-Harbo, Springer, 2001
  </p>
  <p>
  This book is a good reference for the Lifting Scheme and the
  wavelet transform in general.
  </p>
  </li>

  </ol>

 */

class haar_int : public liftbase<int *, int>
{
public:
  /** the constructor does nothing */
  haar_int() {}
  /** the destructor does nothing */
  ~haar_int() {} 
  /** declare but do not define the copy constructor */
  haar_int( const haar_int &rhs );

protected:

  /**
    Haar wavelet lifting scheme predict step.

    The predict step "predicts" that an odd value will be
    equal to the even value.  The difference between the
    actual value of the odd element and the even element
    are stored in the upper half of the array.

    The predict step is sometime referred to as the high
    pass filter or the wavelet function.

    The integer wavelet transform predict step is the same
    as the standard (real) version of the lifting scheme
    Haar transform.

   */
  void predict( int *& vec, int N, transDirection direction )
  {
    int half = N >> 1;

    for (int i = 0; i < half; i++) {
      int predictVal = vec[i];
      int j = i + half;

      if (direction == forward) {
	vec[j] = vec[j] - predictVal;
      }
      else if (direction == inverse) {
	vec[j] = vec[j] + predictVal;
      }
      else {
	printf("haar_int::predict: bad direction value\n");
      }
    }    
  } // predict

  /**
    Update step of the integer to integer wavelet transform.

    In the Haar transform the update step calculates the low pass
    filter (or average).  For a detailed discussion of this
    algorithm, see 
    <a href="http://www.bearcave.com/misl/misl_tech/wavelets/lifting/basiclift.html">Basic Lifting Scheme Wavelets</a>.

   */
  void update( int *& vec, int N, transDirection direction )
  {
    int half = N >> 1;

    for (int i = 0; i < half; i++) {
      int j = i + half;
      // updateVal = floor( vec[j] / 2.0 )
      int updateVal = vec[j] >> 1;

      if (direction == forward) {
	vec[i] = vec[i] + updateVal;
      }
      else if (direction == inverse) {
	vec[i] = vec[i] - updateVal;
      }
      else {
	printf("update_int: bad direction value\n");
      }
    }    
  } // update

}; // haar_int


#endif
