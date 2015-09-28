
#ifndef _HAAR_CLASSIC_H_
#define _HAAR_CLASSIC_H_

#include <math.h>

#include "liftbase.h"

/** \file

  The documentation in this file is formatted for doxygen
  (see www.doxygen.org)

<h4>
   Copyright and Use
</h4>

<p>
   You may use this source code without limitation and without
   fee as long as you include:
</p>
<blockquote>
     This software was written and is copyrighted by Ian Kaplan, Bear
     Products International, www.bearcave.com, 2001.
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

/**
  A version of the classic Haar wavelet transform


  This particular version of the Haar wavelet transform is frequently
  given as the definition for the Haar transform.  This version differs
  from the lifting scheme version.  In the case of the lifting
  scheme version of the Haar transform, the inverse transform is
  a mirror of the forward transform.  The only difference is that
  the plus and minus operators are interchanged.  This algorithm
  does not have this symmetry.

  For a data set of N elements, a wavelet transform will calculate
  N/2 smoothed values and N/2 difference values.  In wavelet
  terminology the smoothed values are calculated by the scaling
  function and the difference (or coefficient) values are calculated
  by the wavelet function.

  This class implements one version of the Haar wavelet transform.
  This particular version is used in the chapter 8 of <i>Ripples
  in Mathematics</i> by Jensen and la Cour-Harbo to illustrate
  the wavelet packet transform.  I have used it to verify my version
  of the wavelet packet algorithm.

  In the description below, an element a<sub>i</sub> is an even element
  and an element b<sub>i</sub> is an odd element.

  In this version of the Haar wavelet transform the scaling (or
  smoothing) function is

<pre>
     s = (a + b)/2
</pre>

  The wavelet function is
<pre>
     d = (a - b)/2
</pre>

  A lifting scheme expression is used in this implementation.  Here
  the wavelet function is calculated first.  The wavelet results
  overwrite the odd b<sub>i</sub> values.  This means that the
  smoothing function values must be calculated with the result
  of the wavelet function.  To recover the value of b<sub>i</sub>
  we use the expression
<pre>
     b = a - 2d

     s = (a + (a - 2d))/2

     s = (2a - 2d)/2

     s = a - d
</pre>

  The lifting scheme terminology is maintained in the algorithm,
  although it does not fully apply.

  This is a template version of the Haar wavelet.  The template must
  be instantiated with an array or an object that acts like an array.
  Objects that act like arrays define the left hand side and right
  hand side index operators: [].

  See www.bearcave.com for more information on wavelets and the
  wavelet lifting scheme.

  \author Ian Kaplan

 */
template <class T>
class haar_classic : public liftbase<T, double> {

protected:

  /**
    Calculate the Haar wavelet or difference function (high
    pass filter) 
   */
  void predict( T& vec, int N, transDirection direction )
  {
    int half = N >> 1;
    int cnt = 0;

    for (int i = 0; i < half; i++) {
      double predictVal = vec[i];
      int j = i + half;

      if (direction == forward) {
	vec[j] = (predictVal - vec[j] )/2;
      }
      else if (direction == inverse) {
	vec[j] =  predictVal - (2 * vec[j]);
      }
      else {
	printf("haar_classic::predict: bad direction value\n");
      }
    }
  } // predict


  /**
    Calculate the smoothing or scaling function (low pass
    filter)
   */
  void update( T& vec, int N, transDirection direction )
  {
    int half = N >> 1;

    for (int i = 0; i < half; i++) {
      int j = i + half;
      double updateVal = vec[j];

      if (direction == forward) {
	vec[i] = vec[i] - updateVal;
      }
      else if (direction == inverse) {
	vec[i] = vec[i] + updateVal;
      }
      else {
	printf("update: bad direction value\n");
      }
    }
  } // update

}; // haar_classic

#endif
