
#ifndef _LINE_INT_H_
#define _LINE_INT_H_



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
  An integer version of the linear interpolation wavelet

  The linear interpolation wavelet uses a predict phase
  that "predicts" that an odd element in the data set
  will line on a line between its two even neighbors.

  This is an integer version of the linear interpolation wavelet.  It
  is interesting to note that unlike the S transform (the integer
  version of the Haar wavelet) or the TS transform (an integer version
  of the CDF(3,1) transform) this algorithm does not preserve
  the mean.  That is, when the transform is calculated, the first
  element of the result array will not be the mean.

 */
template<class T>
class line_int : public liftbase<T, int>
{
public:
  /** the constructor does nothing */
  line_int() {}
  /** the destructor does nothing */
  ~line_int() {}
  /** declare, but do not define the copy constructor */
  line_int( const line_int &rhs );
private:
  /**
    Given y1 at x-coordinate 0 and y2 at x-coordinate
    1, calculate y, at x-coordinate 2.
   */
   int new_n_plus1( int y1, int y2)
   {
     int y = 2 * y2 - y1;
     return y;
   }

  
  /**
    Given a point y1 at x-coordinate 0 and y2 at x-coordinate 1,
    calculate y at x-coordinate -1.
   */
  int new_n_minus1( int y1, int y2)
  {
    int y = 2 * y1 - y2;
    return y;
  }

protected:

  /**
    Predict phase of Lifting Scheme linear interpolation wavelet
    
    The predict step attempts to "predict" the value of an
    odd element from the even elements.  The difference
    between the prediction and the actual element is stored
    as a wavelet coefficient.

    The "predict" step takes place after the split step.  The split
    step will move the odd elements (b<sub>j</sub>) to the second half
    of the array, leaving the even elements (a<sub>i</sub>) in the
    first half

    <pre>
    a<sub>0</sub>, a<sub>1</sub>, a<sub>1</sub>, a<sub>3</sub>, b<sub>0</sub>, b<sub>1</sub>, b<sub>2</sub>, b<sub>2</sub>, 
    </pre>

    The predict step of the line wavelet "predicts" that the
    odd element will be on a line between two even elements.

    <pre>
    b<sub>j+1,i</sub> = b<sub>j,i</sub> - (a<sub>j,i</sub> + a<sub>j,i+1</sub>)/2
    </pre>

    Note that when we get to the end of the data series the odd
    element is the last element in the data series (remember, wavelet
    algorithms work on data series with 2<sup>n</sup> elements).  Here
    we "predict" that the odd element will be on a line that runs
    through the last two even elements.  This can be calculated by
    assuming that the last two even elements are located at x-axis
    coordinates 0 and 1, respectively.  The odd element will be at 2.
    The <tt>new_n_plus1()</tt> function is called to do this simple
    calculation.

    Note that in the case where (N == 2), the algorithm becomes
    the same as the Haar wavelet.  We "predict" that the odd value
    vec[1] will be the same as the even value, vec[0].

   */
  void predict( T & vec, int N, transDirection direction )
  {
    int half = N >> 1;
    int predictVal;

    for (int i = 0; i < half; i++) {
      int j = i + half;
      if (i < half-1) {
	predictVal = (int)((((float)vec[i] + (float)vec[i+1])/2.0) + 0.5);
      }
      else if (N == 2) {
	predictVal = vec[0];
      }
      else {
	// i == half-1
	// Calculate the last "odd" prediction
	int n_plus1 = new_n_plus1( vec[i-1], vec[i] );
	predictVal = (int)((((float)vec[i] + (float)n_plus1)/2.0) + 0.5);
      }

      if (direction == forward) {
	vec[j] = vec[j] - predictVal;
      }
      else if (direction == inverse) {
	vec[j] = vec[j] + predictVal;
      }
      else {
	printf("line::predict: bad direction value\n");
      }
    }
  } // predict


   /**
    Update step of the linear interpolation wavelet

    The predict phase works on the odd elements in the second
    half of the array.  The update phase works on the even
    elements in the first half of the array.  The update
    phase attempts to preserve the average.  After the update
    phase is completed the average of the even elements should
    be approximately the same as the average of the input data
    set from the previous iteration.  The result of the update
    phase becomes the input for the next iteration.

    In a Haar wavelet the average that replaces the even element is
    calculated as the average of the even element and its neighboring
    odd element (e.g., its odd neighbor before the split).  In the
    lifting scheme version of the Haar wavelet the odd element has
    been overwritten by the difference between the odd element and
    its even neighbor.  In calculating the average (to replace the
    even element) the value of the odd element can be recovered via
    a simple algebraic manipulation.

    In the line wavelet the odd element has been replaced by the
    difference between the odd element and the mid-point of its two
    even neighbors.  Recovering the value of the odd element to
    calculate the average is not as simple in this case.

    The value that is added to the even element to preserve the
    average is calculated by the equation shown below.  This equation
    is given in Wim Sweldens' journal articles and his tutorial
    (<i>Building Your Own Wavelets at Home</i>) and in <i>Ripples in
    Mathematics</i>.  A somewhat more complete derivation of this
    equation is provided in <i>Ripples in Mathematics</i> by A. Jensen
    and A. la Cour-Harbo, Springer, 2001.

    The equation used to calculate the average is shown below for a
    given iteratin <i>i</i>.  Note that the predict phase has already
    completed, so the odd values belong to iteration <i>i+1</i>.

    <pre>
  even<sub>i+1,j</sub> = even<sub>i,j</sub> op (odd<sub>i+1,k-1</sub> + odd<sub>i+1,k</sub>)/4
    </pre>

    This version of the line wavelet code implements an integer
    version of linear interpolating wavelet.  This versoin comes from
    the paper <i>Wavelet Transforms that Map Integers to Integers</i>
    by A.R. Calderbank, ingrid daubechies, wim weldens and Boon-Lock
    Yeo, August 1996

    This is the central reference that was used to develop this code.
    Parts 1 and 2 of this paper are for the mathematicially
    sophisticated (which is to say, they are not light reading).
    However, for the implementer, part 3 and part 4 of this paper
    provide excellent coverage of perfectly invertable wavelet
    transforms that map integers to integers.  In fact, part 3 of this
    paper is worth reading in general for its discussion of the wavelet
    Lifting Scheme.

    The value added (or subtracted) from the even<sub>i,j</sub>
    (depending on whether the forward or inverse transform is being
    calculated) is calculated from odd<sub>i+1,k-1</sub> and
    odd<sub>i+1,k</sub> from the predict step.  This means that there
    is missing value at the start of the set of odd elements (e.g., i
    = 0, j == half).  This missing value assumed to line on a line
    with the first two odd elements.

    Because interpolated values are used, the average is not
    perfectly maintained.

  */
  void update( T & vec, int N, transDirection direction )
  {
    int half = N >> 1;

    for (int i = 0; i < half; i++) {
      int j = i + half;
      int val;

      if (i == 0 && N == 2) {
	val = (int)(((float)vec[j]/2.0) + 0.5);
      }
      else if (i == 0 && N > 2) {
	int v_n_minus_1 = new_n_minus1( vec[j], vec[j+1] );
	val = (int)((((float)v_n_minus_1 + (float)vec[j])/4.0) + 0.5);
      }
      else {
	val = (int)((((float)vec[j-1] + (float)vec[j])/4.0) + 0.5);
      }
      if (direction == forward) {
	vec[i] = vec[i] + val;
      }
      else if (direction == inverse) {
	vec[i] = vec[i] - val;
      }
      else {
	printf("update: bad direction value\n");
      }
    } // for    
  } // update


}; // line_int


#endif
