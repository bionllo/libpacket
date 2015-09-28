
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "costshannon.h"


/**

  An implementation of a modified version of the Shannon entropy
  function as a wavelet packet cost function.  This is described
  in section 8.3.2 of <i>Ripples in Mathematics</i> by 
  Jensen and la Cour-Harbo.

  The log function here is the natural log (sometimes denoted as
  ln()).  Note that the result of the entropy function is always
  negative.

 */
double costshannon::costCalc( packnode<double> *node )
{
  assert( node != 0 );

  size_t len = node->length();
  const double *a = node->getData();

  double sum = 0.0;
  for (int i = 0; i < len; i++) {
    double val = 0.0;
    if (a[i] != 0.0) {
      double square = a[i] * a[i];
      val = square * log( square );
    }
    sum = sum + val;
  }

  return -sum;
} // costshannon
