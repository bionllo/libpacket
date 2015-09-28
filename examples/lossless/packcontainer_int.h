
#ifndef _PACKCONTAINER_INT_H_
#define _PACKCONTAINER_INT_H_

#include "packnode.h"

/**

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
  A container for use when calculating a packet wavelet tree.

  By overriding the LHS and RHS versions of the [] operator,
  packcontainer class allows a block of data to be treated as an array
  while a wavelet transform step is being calculated.  After the
  wavelet transform step the data can be referenced as a left hand
  side half (the result of the wavelet scaling function) and a right
  hand side half (the result of the wavelet function).  By allowing
  the two halves of the array to be referenced, copying is avoided.

 */
class packcontainer_int {
private:
  /** number of elements at this packet tree level */
  size_t N;
  /** left (low pass) half of the packcontainer_int data */
  int* lhs;
  /** right (high pass) half of the packnode data */
  int* rhs;

private:
  /** disallow the copy constructor */
  packcontainer_int( const packcontainer_int &rhs ) {};
  /** disallow default constructor */
  packcontainer_int() {};

public:

  /**

    This version of the packcontainer object is used as a container
    in calculating the forward wavelet packet transform for an integer
    data set.

    The packcontainer_int constructor is passed a pointer to a packnode
    object.  As the wavelet packet tree is being built this packnode
    object contain the data from the previous level.

    The length of the packnode object is N.  The constructor will
    dynamically allocate two vectors (lhs and rhs), each with N/2
    elements.  After the wavelet transform step has been calculated on
    the packcontainer_int object, the lhs vector will contain the wavelet
    scaling function, or low pass, result.  The rhs vector will contain
    the wavelet function, or high pass, result.  These will be used
    to construct two new packnode objects which will be children
    of the object passed to the constructor.

   */
  packcontainer_int( packnode<int>* node )
  {
    assert( node != 0 );
    N = node->length();
    assert( N > 1 );

    size_t half = N >> 1;
    block_pool mem_pool;
    size_t num_bytes = half * sizeof(int);

    lhs = (int *)mem_pool.pool_alloc( num_bytes );
    rhs = (int *)mem_pool.pool_alloc( num_bytes );

    for (size_t i = 0; i < N; i++) {
      (*this)[i] = (*node)[i];
    }
  } // packcontainer_int


  /** 

    This version is used when calculating the inverse wavelet packet
    transform.  The constructor is passed the size of the container
    (or at least the size that the container will be, when the left
    and right hand size arrays are initialized).  The lhs and rhs
    arrays are then initialized and the inverse transform step
    is calculated.

  */
  packcontainer_int( size_t n )
  {
    N = n;
    lhs = 0;
    rhs = 0;
  }

  /**
    This is a local implementation of new.  When new
    is applied to a packcontainer_int object, memory will
    be allocated from a memory pool, rather than from
    the system memory pool.
   */
  void *operator new( unsigned int num_bytes )
  {
    block_pool mem_pool;

    void *mem_addr = mem_pool.pool_alloc( num_bytes );
    return mem_addr;
  } // new


  /** LHS [] operator */
  int &operator[]( const size_t i )
  {
    assert( i < N );
    size_t half = N >> 1;

    if (i < half)
      return lhs[i];
    else {
      return rhs[i-half];
    }
  }

  /** RHS [] operator */
  int operator[]( const size_t i ) const
  {
    assert( i < N );
    size_t half = N >> 1;

    if (i < half)
      return lhs[i];
    else {
      return rhs[i-half];
    }
  }

  /** return the left hand size array */
  int* lhsData() { return lhs; }
  /** return the right hand size array */
  int* rhsData() { return rhs; }

  /** set the left hand size array */
  void lhsData(int* l) { lhs = l; }
  /** set the right hand size array */
  void rhsData(int* r) { rhs = r; }

  /** return the length of the data in the packet
      container.  Note that this length is 
      the length of the rhs plus the length of
      the lhs arrays.
  */
  size_t length() { return N; }

}; // packcontainer_int

#endif
