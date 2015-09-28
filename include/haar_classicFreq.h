
#ifndef _HAAR_CLASSICFREQ_H_
#define _HAAR_CLASSICFREQ_H_

#include "haar_classic.h"

/**
  haar_classicFreq

  An extension of the "Haar classic" algorithm for frequency
  analysis.


  The haar_classicFreq template extends the haar_classic
  templace.  The haar_classic template implements what
  I call the "Haar classic" algorithm.  Here the high
  pass filter (wavelet) is

<pre>
     b'<sub>i</sub> = (a<sub>i</sub> - b<sub>i</i>)/2
</pre>
  
  where a<sub>i</sub> is an even element and b<sub>i</sub> is it odd
  element neighbor (of course using a lifting scheme style
  implementation, the even elements are moved to the first half of the
  array and the odd elements are moved to the second half.

  In the Haar classic algorithm the high pass result is placed
  in the upper half of the array.  In the haar_classicFreq version
  the result is placed in the lower half of the array and the
  equation becomes

<pre>
     a'<sub>i</sub> = (a<sub>i</sub> - b<sub>i</i>)/2
</pre>

  The "Haar classic" low pass filter (smoothing function) is
  
<pre>
     a'<sub>i</sub> = (a<sub>i</sub> + b<sub>i</sub>)/2
</pre>

  In the Haar classic algorithm the low pass filter result is placed
  in the lower half of the array. In the haar_classicFreq version the
  result is placed in the upper half of the array and the equation
  becomes

<pre>
     b'<sub>i</sub> = (a<sub>i</sub> + b<sub>i</sub>)/2
</pre>

  The calculation of the high pass filter is done first and
  overwrites the even elements (e.g., a'<sub>i</sub>).  To
  recover the a<sub>i</sub> values given a'<sub>i</sub> and
  b<sub>i</sub>:

<pre>
     a'<sub>i</sub> = (a<sub>i</sub> - b<sub>i</i>)/2
     2 * a'<sub>i</sub> = a<sub>i</sub> - b<sub>i</i>
     a<sub>i</sub> = 2 * a'<sub>i</sub> + b<sub>i</i>
</pre>

  Substituting this into the equation for calculating
  b'<sub>i</i> we get:

<pre>
     b'<sub>i</sub> = ((2 * a'<sub>i</sub> + b<sub>i</i>) + b<sub>i</sub>)/2
     b'<sub>i</sub> = (2 * a'<sub>i</sub> + 2 * b<sub>i</sub>)/2
     b'<sub>i</sub> = a'<sub>i</sub> + b<sub>i</sub>
</pre>

  These equations differ from the Haar classic forward transform
  equations.

  \author Ian Kaplan

 */
template <class T>
class haar_classicFreq : public haar_classic<T> {
protected:

  /**
    In the standard wavelet transform, the high pass filter is applied
    to the upper half of the array (this is the predict phase, in
    lifting scheme terminology).

    In the reverse transform step, which is used for wavelet frequency
    analysis, the high pass filter is applied to the lower half of the
    array.

   */
  void predictRev( T& vec, int N, transDirection direction ) 
  {
    int half = N >> 1;
    int cnt = 0;

    for (int i = 0; i < half; i++) {
      int j = i + half;

      if (direction == forward) {
	vec[i] = (vec[i] - vec[j] )/2;
      }
      else if (direction == inverse) {
	vec[i] =  (2 * vec[i]) + vec[j];
      }
      else {
	printf("predictRev: bad direction value\n");
      }
    }
  } // predictRev


  /**
    Reverse low pass filter

    In the standard wavelet transform the low pass filter is
    applied to the data set (consisting of N elements) and
    the result is placed in the lower half of the array
    (the lower N/2 elements).

    In the reverse transform step, which is used for wavelet
    frequency analysis, the result of the low pass filter
    is placed in the upper half of the array (the upper
    N/2 elements).

    */
  void updateRev( T& vec, int N, transDirection direction )
  {
    int half = N >> 1;

    for (int i = 0; i < half; i++) {
      int j = i + half;

      if (direction == forward) {
	vec[j] = vec[j] + vec[i];
      }
      else if (direction == inverse) {
	vec[j] = vec[j] - vec[i];
      }
      else {
	printf("updateRev: bad direction value\n");
      }
    }
  } // updateRev

public:

  /**

    One forward step of the reverse Haar classic transform, where the
    results for the high and low pass filters are reversed.

   */
  void forwardStepRev( T& vec, const int n )
  {
    split( vec, n ); 
    predictRev( vec, n, forward );
    updateRev( vec, n, forward );
  }

  /**

    One inverse step of the reverse Haar classic transform, where the
    results for the high and low pass filters are reversed.

  */
  void inverseStepRev( T& vec, const int n )
  {
    updateRev( vec, n, inverse );
    predictRev( vec, n, inverse );
    merge( vec, n );
  }
  
}; // haar_classicFreq


#endif
