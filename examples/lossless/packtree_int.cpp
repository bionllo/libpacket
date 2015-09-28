
/** \file

  The documentation in this file is formatted for doxygen
  (see www.doxyeng.org).

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

#include "packcontainer_int.h"
#include "packtree_int.h"


/**
  Construct a wavelet packet tree from a vector of integer
  values.  The size of the vector, which must be a power
  of two, is passed in N.  A wavelet Lifting Scheme object
  is passed in <i>w</i>.  This object is used to calculate
  the wavelet transform step which is applied at each level
  (where level > 0) of the wavelet packet tree.

  The first level (level 0) of the wavelet packet tree contains
  the original data set.

  \arg vec An array of integer values on which the wavelet packet
           transform is calculated.
  \arg N The number of elements in the input array
  \arg w A pointer to the the wavelet transform object to use 
         in calculating the wavelet packet transform.

 */
packtree_int::packtree_int( const int *vec, 
			    const size_t N, 
			    liftbase<packcontainer_int, int> *w )
{
  waveObj = w;

  block_pool mem_pool;
  int *vecCopy = (int *)mem_pool.pool_alloc( N * sizeof( int ) );

  for (int i = 0; i < N; i++) {
    vecCopy[i] = vec[i];
  }

  root = new packnode<int>( vecCopy, N, packnode<int>::OriginalData );
  root->mark( true );
  newLevel( root, false, false ); 
} // packtree_int



/**

  The best basis algorithm selects the nodes nearest the tree root for
  the best basis set.  These nodes are "marked" true with a boolean
  flag.

  The best basis algorithm outlined in <i>Ripples in Mathematics</i>
  sets any marks in nodes that are below a marked node in the tree
  (nodes which are in a subtree of a marked node) to false.  However,
  this is unnecessary when the best basis set is collected, since the
  algorithm uses top down tree traversal and stops at marked node.

  A problem does arise when the entire tree is printed to show the
  nodes that are marked as part of the best basis set.  In this case
  the result may appear wrong, since child nodes of a best basis node
  are marked.  This function does a top down traversal setting the 
  "marks" on these child nodes to false.

 */
void packtree_int::cleanTree(packnode<int> *top, bool removeMark )
{
  if (top != 0) {
    if (removeMark) {
      if (top->mark()) {
        top->mark( false );
      }
    }
    else {
      // if a mark is found, then set all the "marks" below that
      // point to false (e.g., remove the marks).
      if (top->mark()) {
        removeMark = true;
      }
    }
    cleanTree( top->lhsChild(), removeMark );
    cleanTree( top->rhsChild(), removeMark );
  }
} // cleanTree



/**
  Print the wavelet packet tree cost values in breadth first
  order.
 */
void packtree_int::prCost()
{
  if (root != 0) {
    breadthFirstPrint(printCost);
  }
}


/**
  Print the wavelet packet tree, showing the nodes
  that have been selected by the "best basis" algorithm.
 */
void packtree_int::prBestBasis()
{
  if (root != 0) {
    cleanTree( root, false );
    breadthFirstPrint(printBestBasis);
  }
} // prBestBasis


/** 

  Walk the wavelet packet tree and apply the "best basis" algorithm
  described in Chapter 8 of <i>Ripples in Mathematics</i>.  Nodes that
  are "marked" become part of the best basis, which is a minimal
  representation of the data in terms of the cost function.

 */
int packtree_int::bestBasisWalk( packnode<int> *top )
{
  int cost = 0;

  if (top != 0) {
    packnode<int> *lhs = top->lhsChild();
    packnode<int> *rhs = top->rhsChild();

    if (lhs == 0 && rhs == 0) { // we've reached a leaf
      cost = top->cost();
    }
    else if (lhs != 0 && rhs != 0) {

      int lhsCost = bestBasisWalk( lhs );
      int rhsCost = bestBasisWalk( rhs );

      int v1 = top->cost();
      int v2 = lhsCost + rhsCost;

      if (v1 <= v2) {
        top->mark( true );
        lhs->mark( false );
        rhs->mark( false );
      }
      else { // v1 > v2
        top->cost( v2 );
      }
      cost = top->cost();
    }
    else {
      // The tree does not seem to be a full binary tree
      // Something has gone badly wrong.
      assert( false );
    }
  }

  return cost;
} // bestBasicWalk


/**
  Calculate the wavelet packet "best basis"

 */
void packtree_int::bestBasis()
{
  bestBasisWalk( root );
} // bestBasis


/**

  Recursively traverse the wavelet packet tree and check that the best
  basis result is correct.  That is, that the best basis has been
  calculated and that it does not include the orignal data set.
  The best basis includes the original data set with the packnode
  ofr the original data set is marked.

  This algorithm makes use of two class global variables.  There may
  be a purely recursive, way to do this without these global class
  variables, but these variables make the algorithm much easier.
  The variables are initialized by the calling function
  bestBasisOK().

 */
void packtree_int::checkBestBasis( packnode<int> *top )
{
  if (top != 0) {
    if (top->mark()) {
      foundBestBasisVal = true;
      if (top->getKind() == packdata<int>::OriginalData) {
        foundOriginalData = true;
      }
    }
    if (!foundOriginalData) {
      checkBestBasis( top->lhsChild() );
    }
    if (!foundOriginalData) {
      checkBestBasis( top->rhsChild() );
    }
  }
}  // checkBestBasis



/**
  Return true is be best basis has been calculated properly,
  return false if the best basis has not been calculated or
  it was not calculated properly.

  The best basis is calculated in reference to a particular
  cost function.  A particular cost function will not always
  result in a data set which differs from the original data
  set.  If this is the case, the "best basis" result will
  include the original data.

 */
bool packtree_int::bestBasisOK()
{
  foundOriginalData = false;
  foundBestBasisVal = false;
  checkBestBasis( root );

  bool rslt = (foundBestBasisVal && (!foundOriginalData));

  return rslt;
} // bestBasicOK


/**
  Traverse the tree from the top down and add the best basis
  nodes to the best basis list.

  Note that the list object is simply a package for a scalar
  value, the pointer to the head of the list.  So it can be
  passed by value without incurring a cost greater than 
  passing a pointer (e.g., pass by reference).
 */
void packtree_int::buildBestBasisList( packnode<int> *top, 
				       packdata_list<int> &list )
{
  if (top != 0) {
    if (top->mark()) {
      list.add( top );
    }
    else {
      buildBestBasisList( top->lhsChild(), list );
      buildBestBasisList( top->rhsChild(), list );
    }
  }
} // buildBestBasisList



/** 
  Return a list consisting of the best basis packdata values.

 */
packdata_list<int> packtree_int::getBestBasisList()
{
  packdata_list<int> list;

  buildBestBasisList( root, list );
  return list;
} // getBestBasisList
