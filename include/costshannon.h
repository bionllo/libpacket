
#ifndef _COSTSHANNON_H_
#define _COSTSHANNON_H_

#include "costbase.h"
#include "packnode.h"

/** \file

  The documentation in this file is formatted for doxygen
  (see www.doxygen.org).


  <b>Copyright and Use</b>

   You may use this source code without limitation and without
   fee as long as you include:

<blockquote>
     This software was written and is copyrighted by Ian Kaplan, Bear
     Products International, www.bearcave.com, 2002.
</blockquote>

   This software is provided "as is", without any warranty or
   claim as to its usefulness.  Anyone who uses this source code
   uses it at their own risk.  Nor is any support provided by
   Ian Kaplan and Bear Products International.

   Please send any bug fixes or suggested source changes to:

<pre>
     iank@bearcave.com
</pre>

  @author Ian Kaplan

 */


/**
  The costshannon class extends the abstract class
  costbase with a concrete implementation of the
  costCalc function that implements the a modified
  version of the Shannon entropy function as a cost
  function.

  \author Ian Kaplan
  
 */
class costshannon : public costbase
{
protected:
  double costCalc( packnode<double> *node);

public:

  /** Calculate a modified version of the the Shannon entropy cost
      function for the wavelet packet tree, filling in the cost value
      at each node */
  costshannon( packnode<double> *node ) { traverse( node ); }
};

#endif
