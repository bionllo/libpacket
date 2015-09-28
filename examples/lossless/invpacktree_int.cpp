

#include <assert.h>
#include <stdio.h>

#include "blockpool.h"
#include "invpacktree_int.h"

/** \file

  For additional information on this implementation of the inverse
  wavelet packet transform see
  http://www.bearcave.com/misl/misl_tech/wavelets/packet/index.html 

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


/**

  Allocate a new level in the wavelet packet tree that is being
  constructed.  The new "level" is built from a packcontainer_int object.
  The left child will be the elem argument.  The length of the data in
  the new level will be twice that of lower level (of which elem is
  half).
  
 */
void invpacktree_int::new_level( packdata<int> *elem )
{
  size_t half = elem->length();
  size_t n = half * 2;

  packcontainer_int *container = new packcontainer_int( n );
  container->lhsData( (int *)elem->getData() );
  stack.add( container );
} // new_level



/**

  At this point the Top Of Stack (TOS) packcontainer_int object should
  have both a right and a left array.  Calculate an inverse wavelet
  transform step on the packcontainer_int object.  The packcontainer_int
  object allows the right and left hand side arrays to be treated as
  one array

  If the current top of stack is twice the size of the inverse wavelet
  transform step result, the result becomes the right hand size of
  top of stack packcontainer_int and reduce is called recursively.

  If the TOS is empty or it is not twice the size of the inverse
  transform result, a new packcontainer_int will be pushed on the stack.
  The left hand size will be the transform result.

 */
void invpacktree_int::reduce()
{
  LIST<packcontainer_int *>::handle h;
  h = stack.first();
  packcontainer_int *tos = stack.get_item( h );

  assert( tos->lhsData() != 0 && tos->rhsData() != 0 );

  /** Remove the linked list element that used to contain tos
      (e.g., pop the linked list element off).  Note that this
      leaves the tos object unchanged (e.g., it does not delete
      it).
  */
  stack.remove();

  size_t n = tos->length();
  // calculate the inverse wavelet transform step
  waveObj->inverseStep( (*tos), n );

  // copy the result of the inverse wavelet transform
  // into a new data array.
  block_pool mem_pool;

  int *vec = (int *)mem_pool.pool_alloc( n * sizeof( int ) );
  for (int i = 0; i < n; i++) {
    vec[i] = (*tos)[i];
  }

  if (stack.first() != 0) {
    h = stack.first();
    packcontainer_int *tos = stack.get_item( h );

    if (tos->length() == n*2) {
      tos->rhsData( vec );
      reduce();
    }
    else {
      assert( tos->length() > n*2 );
      packcontainer_int *container = new packcontainer_int( n*2 );
      container->lhsData( vec );
      stack.add( container );
    }  // else
  }
  else {
    // the stack is empty
    packcontainer_int *container = new packcontainer_int( n*2 );
    container->lhsData( vec );
    stack.add( container );  
  }
} // reduce



/**
  Add a packdata element to the inverse wavelet packet transform
  calculation.

  A packdata element contains the wavelet packet data items from
  one level of the wavelet packet transform tree.

  The inverse wavelet packet transform calculation uses a stack.  Each
  level in the stack consists of a packcontainer_int object.  This object
  consists of left and right hand size arrays.  When both these arrays
  are present, they can be treated as one contiguous array (courtesy
  of C++ operator overloading).  The size of the packcontainer_int object
  is twice the size of its left and right hand size arrays.

  If the stack is empty, a new level is created.  A packetcontainer
  object can be viewed as a binary tree element.  The left hand
  size is filled in first.

  If the stack is not empty and the packcontainer_int object on the
  top of stack (TOS) is twice the size of the elem argument,
  then the array contained in the elem argument is added to
  the TOS element and reduce is called to calculate a step
  of the inverse wavelet transform.

  If the TOS element is greater than twice the size of elem
  then a new level is added.

 */
void invpacktree_int::add_elem( packdata<int> *elem )
{
  assert( elem != 0 );

  if (stack.first() == 0) {
    new_level( elem );
  }
  else {
    size_t half = elem->length();
    size_t n = half * 2;
    LIST<packcontainer_int *>::handle h;
    h = stack.first();
    packcontainer_int *tos = stack.get_item( h );

    if (tos->length() == n) {
      assert( tos->rhsData() == 0);
      tos->rhsData( (int *)elem->getData() );
      reduce();
    }
    else if (tos->length() > n) {
      new_level( elem );
    }
    else {
      printf("add_elem: the size of the TOS elem is wrong\n");
    }
  } // else
} // add_elem


/**
  This constructor calculates the inverse wavelet packet transform.
  The constructor is passed a packdata_list object, which contains
  the "best basis" result of the wavelet packet transform.  The 
  liftbase template argument is a pointer to a wavelet transform
  function.  This wavelet transform must be the same function that
  was used to calculate the packet transform.

 */
invpacktree_int::invpacktree_int( packdata_list<int> &list, 
				  liftbase<packcontainer_int, int> *w )
{
  data = 0;
  N = 0;
  waveObj = w;

  // Traverse the "best basis" list and calculate the inverse
  // wavelet packet transform.
  packdata_list<int>::handle h;
  for (h = list.first(); h != 0; h = list.next( h )) {
    packdata<int> *elem = list.get_item( h );
    add_elem( elem );
  } // for

  LIST<packcontainer_int *>::handle tosHandle;
  tosHandle = stack.first();
  packcontainer_int *tos = stack.get_item( tosHandle );

  if (tos != 0) {
    size_t len = tos->length();
    N = len/2;

    data = tos->lhsData();
    stack.remove();
  }
} // invpacktree_int


/**
  print the result of the inverse wavelet packet transform
 */
void invpacktree_int::pr()
{
  if (data != 0) {
    for (int i = 0; i < N; i++) {
      printf("%7.4f ", data[i] );
    }
    printf("\n");
  }
} // pr
