
#ifndef _TS_TRANS_INT_H_
#define _TS_TRANS_INT_H_



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
#include "haar_int.h"


/**
  An integer version of the TS transform (an extended S, or Haar
  transform).

  The TS transform is an extension of integer version of the Haar
  trasform (which is sometimes referred the S-transform in the image
  processing literature.  The TS transform is an integer version of
  the so called Cohen-Daubechies-Feaveau (3,1) transform.  Here the
  (3,1) refer to 3 vanishing moments for the wavelet function and 1
  vanishing moment for the scaling function.

  The equations for the lifting scheme version of the forward
  TS transform are shown below.  As with all lifting scheme
  algorithms, the inverse transform exchanges addition and
  subtraction operators.

  The TS transform and the S transform are the same in the first two
  steps (the first predict and update steps).  An average
  interpolation step is added to the S (Haar) transform.

  <pre>
  d(1)<sub>1,i</sub> = s<sub>0,2i+1</sub> - s<sub>0,2i</sub>
  
  s<sub>1,i</sub> = s<sub>0,2i</sub> + floor( d(1)<sub>1,i</sub>/2 )

  d<sub>1,i</sub> = d(1)<sub>1,i</sub> + floor((s<sub>1,i-1</sub> - s<sub>1,i+1</sub>)/4.0 + 0.5)
  </pre>

  This notation and the algorithm implemented here is taken directly
  from <i>Wavelet Transformst that Map Integers to Integers</i> by
  A.R. Calderbank, Ingrid Daugbechies, Wim Sweldens, and Boon-Lock
  Yeo, 1996

  The mathematical structure is reflected in the class structure.
  Here the ts_trans_int class extends the haar_int class.  The
  haar_int class provide the <tt>predict()</tt> and <tt>update</tt>
  functions.  An additional <tt>predict2()</tt> function is added
  that implements interpolation step.

  Since an extra step has been added, the <tt>forwardStep</tt> and
  <tt>inverseStep</tt> functions in the base class (<tt>liftbase</tt>)
  are over-ridden by ts_trans_int.

  <b>A brief note on vanishing moments</b>

  This algorithm is commonly known as the CDF (3,1) wavelet
  algorithm.  As noted above, these numbers refer to the vanishing
  moments.  I have never found a definition of "vanishing moment" that
  made intuitive sense to me, at least when applied to wavelet basis
  functions.  

  As I understand the definition, a vanishing moment is a region over
  which the integral is zero.  So the area of sin(x) in the region
  0..2Pi has an integral of zero, since the region between 0..Pi
  results in a positive integral and the area between Pi..2Pi results
  in the same integeral value, but with a negative sign.  The sum of
  these two regions is zero.  If a wavelet were constructed from the
  sine function, such that

  <pre>
  x = {0..2Pi}      : y = sin(x)
  x = anything else : y = 0
  </pre>

  This would be a wavelet with "compact support" (it is a defined for
  a finite region, 0..2Pi) and one vanishing moment.

 */
// class ts_trans_int : public liftbase<int *, int>
class ts_trans_int : public haar_int
{
public:
  /** the constructor does nothing */
  ts_trans_int() {}
  /** the destructor does nothing */
  ~ts_trans_int() {}
  /** declare, but do not define the copy constructor */
  ts_trans_int( const ts_trans_int &rhs );

private:
  /**
    Calculate the element s<sub>1,i+1</sub>.

    The low pass half of the array is in the array index range
    {0..half-1}.  In the equation below, when <tt>i = half-1</tt>
    a non-existent element at i+1 is needed.  This function
    calculates this element from s[half-2] and s[half-1].

    <pre>
    d<sub>1,i</sub> = d(1)<sub>1,i</sub> + floor((s<sub>1,i-1</sub> - s<sub>1,i+1</sub>)/4.0 + 0.5)
    </pre>

    Here the non-existent element s[half] is assumed to lie on
    the line from s[half-2] and s[half-1].  We pretend that
    s[half-2] has the x-coordinate of 0 and s[half-1] has
    an x-coordinate of 1.  We then need to calculate the
    y value at the x-coordinate 2.  

    The "two-point equation" for a line is used for this
    calculation, where we are trying to find the value 
    of y, given
   
     <pre>
     .          y<sub>2</sub> - y<sub>1</sub>
     (y - y<sub>1</sub>) = -------- (x - x<sub>1</sub>)
     .          x<sub>2</sub> - x<sub>1</sub>
     </pre>

     Solving for y

     <pre>
     .    y<sub>2</sub> - y<sub>1</sub>
     y = -------- (x - x<sub>1</sub>) + y<sub>1</sub>
     .    x<sub>2</sub> - x<sub>1</sub>
     </pre>

     where

    <pre>
    x<sub>1</sub> = 0
    x<sub>2</sub> = 1
    y<sub>1</sub> = s[half-2]
    y<sub>2</sub> = s[half-1]
    x = 2
    </pre>

    Substituting in these values we get

     <pre>
     y = 2*y<sub>2</sub> - y<sub>1</sub>
     </pre>

   */
  int new_n_plus1( int y1, int y2)
  {
    int y = 2 * y2 - y1;
    return y;
  }

  /**
    In the function <tt>new_n_plus1</tt> a point beyond the end
    of the low pass filter array is calculated.  Here
    a point beyond the beginning of the array is calculated.
    
    <pre>
    x<sub>1</sub> = 0
    x<sub>2</sub> = 1
    y<sub>1</sub> = s[0]
    y<sub>2</sub> = s[1]
    x = -1
    </pre>

    When these values are plugged into the
    two point equation we get

    <pre>
    y = 2 * y<sub>1</sub> - y<sub>2</sub>
    </pre>

   */
  int new_n_minus1( int y1, int y2)
  {
    int y = 2 * y1 - y2;
    return y;
  }

protected:

  /**
    The predict interpolation step.

    Note that special cases exist at the start and end of the
    array.  A special case also exists for N = 2, where the
    calculation is the same as the Haar wavelet (e.g., no
    interpolation factor is added in).

   */
  void predict2( int *& vec, int N, transDirection direction )
  {
    int half = N >> 1;
    int predictVal;

    for (int i = 0; i < half; i++) {
      int j = i + half;
      int y_n_plus1;
      int y_n_minus1;

      if (N == 2) {
	y_n_minus1 = vec[0];
	y_n_plus1 = vec[0];
      }
      else if (i == 0) {
	y_n_minus1 = new_n_minus1( vec[0], vec[1] );
	y_n_plus1 = vec[1];
      }
      else if (i < half-1) {
	y_n_minus1 = vec[i-1];
	y_n_plus1  = vec[i+1];
      }
      else { // i == half-1
	y_n_minus1 = vec[i-1];
	y_n_plus1  = new_n_plus1( vec[i-1], vec[i] );
      }

      predictVal = (int)( (((float)y_n_minus1 - (float)y_n_plus1)/4.0) + 0.5 );

      if (direction == forward) {
	vec[j] = vec[j] + predictVal;
      }
      else if (direction == inverse) {
	vec[j] = vec[j] - predictVal;
      }
      else {
	printf("haar_int::predict: bad direction value\n");
      }
    }      
  } // predict2

public:
  /**
    One TS transform forward step.  This extends the S transform
    (Haar integer transform) with the predict2 step.
   */
  void forwardStep( int *& vec, const int n )
  {
    split( vec, n );
    predict( vec, n, forward );
    update( vec, n, forward );
    predict2( vec, n, forward );
  } // forwardStep

  /**
    One TS transform inverse step.  This extends the S transform
    (Haar integer transform) with the predict2 step.
   */
  void inverseStep( int *& vec, const int n )
  {
    predict2( vec, n, inverse );
    update( vec, n, inverse );
    predict( vec, n, inverse );
    merge( vec, n );
  } // inverseStep

}; // ts_trans_int


#endif
