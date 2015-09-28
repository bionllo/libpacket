
#ifndef _PACKNODE_H_
#define _PACKNODE_H_

#include "packdata.h"

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
  A wavelet packet tree node

  A description of the wavelet packet algorithm can be found
  in Chapter 8 of <i>Ripples in Mathematics</i> by Jensen and
  la Cour-Harbo.

  For a data set consisting of N data elements, the wavelet packet
  algorithm will build a binary tree with log<sub>2</sub>(N) levels.
  Since it is a binary tree, the number of nodes doubles at each
  level.  At the same time, the amount of data that is stored in
  each node is halved.

  A node will have <i>n</i> data elements.  The two children will
  each have n/2 elements, stored in packnode children.

  A pointer to the data vector for the node and the length of the
  data is set by the class constructor.

  Once the wavelet packet tree is built a cost value is assigned to
  each node in the tree.

 */
template<class T>
class packnode : public packdata<T> {

private:

  /** left child (with N/2) data elements */
  packnode* leftChild;
  /** right child (with N/2) data elements */
  packnode* rightChild;

  /** cost value for this level */
  T costVal;

  /** chosen == true: node is part of the best basis of the
      wavelet transform, otherwise, false. */
  bool chosen;

private:
  /** disallow the copy constructor */
  packnode( const packnode &rhs ) {};
  /** disallow default constructor */
  packnode() {};

public:

  /**
    Packnode constructor
   */
  packnode( T *vec, 
            const size_t n, 
            const transformKind k ) : packdata<T>(vec, n, k)
  {
    leftChild = 0;
    rightChild = 0;
    costVal = 0.0;
    chosen = false;
  }

     
  /** LHS [] operator */
  T &operator[]( const size_t i )
  {
    assert( i < N );
    return data[i];
  }

  /** RHS [] operator */
  T operator[]( const size_t i ) const
  {
    assert( i < N );
    return data[i];
  }


  /** print the cost value */
  void prCost() const
  {
    printf("%7.4f\n", costVal );
  }


  /** if the node is selected as part of the best basis
      function, print it.
   */
  void prBestBasis() const
  {
    for (int i = 0; i < N; i++) {
      printf("%7.4f ", data[i] );
    }
    if (chosen) {
      printf("  *");
    }
    printf("\n");
  } // prBestBasis


  /** set the left child pointer */
  void lhsChild( packnode *l ) { leftChild = l; }
  /** get the left child pointer */
  packnode *lhsChild(void) { return leftChild; }

  /** set the right child pointer */
  void rhsChild( packnode *r ) { rightChild = r; }
  /** get the right child pointer */
  packnode *rhsChild(void) { return rightChild; }

  /** set the cost value for the node */
  void cost( T val ) { costVal = val; }
  /** get the cost value for the node */
  T cost(void) { return costVal; }

  /** the "chosen" flag marks a node for inclusion in
      the best basis set.
   */
  void mark( bool b ) { chosen = b; }
  /** return the value of the "mark" boolean flag */
  bool mark() { return chosen; }

}; // packnode

#endif
