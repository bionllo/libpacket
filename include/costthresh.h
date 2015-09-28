
#ifndef _COSTTHRESH_H_
#define _COSTTHRESH_H_

#include "costbase.h"
#include "packnode.h"

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



/**
  The costthresh class implements a wavelet packet transform
  cost function which counts the values in the data set that
  are greater than a threshold value.  This threshold value
  is initialized in the constructor.

  The costbase class provides the wavelet packet tree traversal
  function which calls the subclass's cost function.

  \author Ian Kaplan

 */
class costthresh : public costbase 
{
private:
  /** cost threshold */
  double thresh;

  /** absolute value */
  double abs(const double x)
  {
    double retval = x;
    if (retval < 0.0)
      retval = -retval;
    return retval;
  } // abs

protected:

  /**
    This is a simple threshold calculation.  The costCalc
    function returns the number of node data values whose
    absolute value is greater than the threshold.
   */
  double costCalc(packnode<double> *node)
  {
    double count = 0.0;
    if (node != 0) {
      size_t len = node->length();
      for (int i = 0; i < len; i++) {
	if (abs((*node)[i]) > thresh) {
	  count = count + 1.0;
	}
      }
    }
    return count;
  } // costCalc

public:
  /** class constructor: calculate the wavelet packet cost
      function using a simple threshold, t. */
  costthresh(packnode<double> *node, double t ) 
  { 
    thresh = t; 
    traverse( node );
  }
}; // costthresh

#endif
