
/** \file

  This file contains code to test wavelet frequency analysis via
  wavelet packets.  See
  <a href="http://www.bearcave.com/misl/misl_tech/wavelets/packfreq/index.html">http://www.bearcave.com/misl/misl_tech/wavelets/packfreq/index.html</a>

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

#include <assert.h>
#include <stdio.h>

#include "packcontainer.h"
#include "haar_classicFreq.h"
#include "daub.h"
#include "packfreq.h"


/**
  Some small data sets
 */

/**/
double data[] = { 32.0, 10.0, 20.0, 38.0, 37.0, 28.0, 38.0, 34.0,
                  18.0, 24.0, 18.0, 9.0, 23.0, 24.0, 28.0, 34.0 };
/**/

/*
double data[] = { 56.0, 40.0, 8.0, 24.0, 48.0, 48.0, 40.0, 16.0 };
*/



/** \function
  Generate a signal composed of a sum of sine waves. 
  The sine waves summed are in decreasing magnitude and
  decreasing frequency.
 */
void gen_freqMix( double *vecX, double *vecY, size_t N)
{
  const double PI = 3.1415926535897932384626433832795;
  const double range = 2 * PI;
  const double incr = range / (double)N;

  double point = 0.0;
  int i;
  for (i = 0; i < N; i++) {
    vecX[i] = point;
    vecY[i] = 4 * sin( 64 * point ) + 
              2 * sin( 32 * point ) + 
              1 * sin( 16 * point ) +
            0.5 * sin( 8  * point );
    // printf("%7.4g, %7.4g\n", vecX[i], vecY[i] );
    point = point + incr;
  }
} // gen_freqMix



/** \function
  Generate a signal composed of one or more sine waves.
 */
void gen_sinCombo( double *vecX, double *vecY, size_t N )
{
  const double PI = 3.1415926535897932384626433832795;
  const double range = 8 * PI;
  const double incr = range / (double)N;

  double point = 0.0;

  int stepCnt = 0;
  int i;
  for (i = 0; i < N; i++) {
    vecX[i] = point;
    // vecY[i] = sin( 64 * point ) + sin( 32 * point ) + sin( 16 * point ); 
    // vecY[i] = sin( 16 * PI * point ) + sin( 4 * PI * point ); 
    vecY[i] = sin( 4 * PI * point ); 

    // printf("x[%2d] = %7.4g, y[%2d] = %7.4g\n", i, vecX[i], i, vecY[i] );
    // printf("%7.4g, %7.4g\n", vecX[i], vecY[i] );
    // printf("%d, %7.4g\n", i, vecY[i] );

    point = point + incr;
  }
}


/** \function
  Generate a signal that increases in frequency by steps.
 */
void gen_steps( double *vecX, double *vecY, size_t N, size_t steps )
{
  const double PI = 3.1415926535897932384626433832795;
  const double range = 32 * PI;
  const double incr = range / (double)N;

  double point = 0.0;

  double mult = 1;
  const size_t stepWidth = N / steps;
  int stepCnt = 0;
  int i;
  for (i = 0; i < N; i++) {
    vecX[i] = point;
    vecY[i] = sin( mult * point ); 

    // printf("x[%2d] = %7.4g, y[%2d] = %7.4g\n", i, vecX[i], i, vecY[i] );
    // printf("%7.4g, %7.4g\n", vecX[i], vecY[i] );
    // printf("%d, %7.4g\n", i, vecY[i] );

    point = point + incr;
    stepCnt++;
    if (stepCnt == stepWidth) {
      mult = mult + (PI/2.0);
      stepCnt = 0;
    }
  }
}


/** \function
  Generate a linear chirp signal
 */
void gen_chirp( double *vecX, double *vecY, size_t N )
{
  const double PI = 3.1415926535897932384626433832795;
  const double range = 2;
  // const double range = 16 * PI;
  const double incr = range / (double)N;

  double point = 0.0;

  int i;
  for (i = 0; i < N; i++) {
    vecX[i] = point;
    vecY[i] = sin( 128 * PI * point * point );
    // printf("x[%2d] = %7.4g, y[%2d] = %7.4g\n", i, vecX[i], i, vecY[i] );
    // printf("%7.4g, %7.4g\n", vecX[i], vecY[i] );
    // printf("%d, %7.4g\n", i, vecY[i] );
    point = point + incr;
  }
}



/** \function
  
  Print a vector of doubles whose length is len.

 */
void prCoords( double *vecX, double *vecY, size_t len )
{
  for (int i = 0; i < len; i++) {
    printf("%7.4f  %7.4f\n", vecX[i], vecY[i] );
  }
}


/** \function
  Print a vector of doubles
 */
void prVec( double *vec, size_t len )
{
  for (int i = 0; i < len; i++) {
    printf("%4d  %7.4f\n", i, vec[i] );
  }
} // prVec


/** \function 

  The entry point for code to test the wavelet packet transform.

  The code in main provides a simple example of how to call the
  wavelet packet transform code.

*/
int
main()
{
  const size_t N = 1024;
  // const size_t N = sizeof( data ) / sizeof( double );
  double vecX[N], vecY[N];

  // gen_chirp( vecX, vecY, N );
  // gen_steps( vecX, vecY, N, 8 );
  gen_freqMix( vecX, vecY, N );
  // gen_sinCombo( vecX, vecY, N );
  // prVec( vecY, N );
  // prCoords( vecX, vecY, N );

  // The "Haar" classic transform
  haar_classicFreq<packcontainer> h;
  // Daubechies<packcontainer> h;

  // calculate the wavelet packet tree, using the wavelet transform h
  packfreq tree( vecY, N, &h );
  // packfreq tree( data, N, &h );
  
  // tree.pr();
  // printf("\n");

  tree.getLevel( 5 );

  tree.plotMat(N);

  // free the memory pool
  block_pool mem_pool;
  mem_pool.free_pool();

  return 0;
}
