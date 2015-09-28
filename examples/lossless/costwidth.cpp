
/** \file

  The documentation in this file is formatted for doxygen
  (see www.doxygen.org).

<h4>
   Copyright and Use
</h4>


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

#include "costwidth.h"
#include "support.h"

int 
costwidth::costCalc( packnode<int> *root )
{
  assert( root != 0 );

  size_t N = root->length();
  const int *a = root->getData();

  int width = support::vecWidth( a, N );

  return width;
} // costCalc
