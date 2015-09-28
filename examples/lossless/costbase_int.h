
#ifndef _COSTBASE_INT_H_
#define _COSTBASE_INT_H_

#include "packnode.h"

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


/**
  Base class for objects that define integer costs functions
  for an integer version of the wavelet packet transform.

  The costbase base class provides a constructor that is passed the
  root of an integer wavelet packet tree (which is built by
  packtree_int.cpp).  The integer wavelet packet tree is constructed
  from packnode<int> objects (which are a subclass of the packdata).

  The cost function calculation invoked by the constructor traverses
  the wavelet packet tree (top down) and calls costCalc on each node
  in the tree.  The cost function result is stored in the node.  Note
  that the cost function also calculates a cost value for the original
  data, since in theory the original data may represent the minimal
  representation for the data in terms of the cost function.

  The pure virtual function costCalc, which calculates the cost
  function, must be defined by the subclass.

  A description of the cost functions associated with the wavelet
  packet transform can be found in Chapter 8 of <i>Ripples in
  Mathematics</i> by Jense and la Cour-Harbo.

 */

class costbase_int
{
private:
  /** disallow the copy constructor */
  costbase_int( const costbase_int &rhs ) {}

protected:
  /**
    Recursively traverse the wavelet packet tree and calculate the cost
    function.
    */
  void traverse( packnode<int> *node )
  {
    if (node != 0) {
      int cost = costCalc( node );
      node->cost( cost );
    
      traverse( node->lhsChild() );
      traverse( node->rhsChild() );
    }
  } // traverse 

  /** Cost function to be defined by the subclass */
  virtual int costCalc(packnode<int> *node) = 0;

public:
  /** The default constructor does nothing */
  costbase_int() {}

}; // costbase_int

#endif
