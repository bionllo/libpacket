
#ifndef _GROW_ARRAY_H_
#define _GROW_ARRAY_H_

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

#include "blockpool.h"

/**

   This file defines an array template class that will grow as
   elements are added to the end of the array.  This is similar
   to the STL <vector> class.

   This array class is designed for dense arrays where the all
   elements of the array are used.

   Usage:

     - Elements are added to the end of the array via the append
       function.

     - Elements in the array can be accessed via the [] operator.

     - If the elements in the array are dynamicly allocated, the
       user is responsible for deallocating these elements.

  A doubling algorithm is used when the data size is expanded because
  it minimizes the amount of copying that must be done.  The array
  will quickly grow to a the size needed to accomodate the data set
  and no more copying will be necessary.  For large arrays there is the
  drawback that more memory may be allocated than is needed, since
  the amount of memory used grows exponentially.

  \author Ian Kaplan

 */
template <class T>
class GrowableArray {
private:
  typedef enum { StartArraySize = 128 } bogus;
  /** number of data elements */
  size_t num_elem;
  /** Array size (always <= num_elem) */
  size_t array_size;
  T *pArray;

private:
  /**
     twice
    
     Double the amount of memory allocated for the array.
     Return true if memory allocation succeeded, false
     otherwise.
  */
  bool twice()
  {
    bool rslt;
    T *old_array = pArray;
    size_t new_size = array_size * 2;
    
    pArray = new T [ new_size ];
    if (pArray != 0) {
      rslt = true;
      for (int i = 0; i < array_size; i++) {
	pArray[i] = old_array[i];
      }
    
      delete [] old_array;
    
      array_size = new_size;
    }
    else {
      rslt = false;
    }

    return rslt;
  } // twice


public:
  GrowableArray()
  {
    pArray = new T[ StartArraySize ];
    num_elem = 0;
    array_size = StartArraySize;
  } // GrowableArray constructor

  /** destructor */
  ~GrowableArray()
  {
    if (pArray != NULL) {
      delete [] pArray;
    }
  } // GrowableArray destructor

  /** Length of the data (which is not necessarily the same
      as the length of the internal array */
  const size_t length(void) const { return num_elem; }

  /** set array to zero length */
  void set_to_zero() 
  {
    num_elem = 0;
  }

  /** LHS [] operator */
  T &operator[](const size_t i)
  {
    assert( i < num_elem );
    return pArray[ i ];
  }

  /** RHS [] operator */
  T operator[](const size_t i ) const
  {
    assert( i < num_elem );
    return pArray[ i ];
  }

  /** Get a pointer to the internal data array */
  const T *getData() const { return pArray; }

  /** append an item to the end of the array */
  void append( T item )
  {

    if (num_elem == array_size) {
      bool allocOK = twice();
      assert( allocOK );
    }

    pArray[ num_elem ] = item;
    num_elem++;
  } // append


  /**
     expand
    
     Expand the number of array data slots by "amount" elements.
     Note that "array_size" is the total amount of storage available
     for data slots.  "num_elem" is the number of data slots.
     The bounds over which the array can be indexed is governed
     by num_elem.  Note that after expand() is called the new
     data elements can be read, but their value is undefined until
     they are initialized.
  */
  void expand( size_t amount )
  {
    bool allocOK = true;

    while (allocOK && num_elem + amount >= array_size) {
      allocOK = twice();
      assert( allocOK );
    }
    num_elem += amount;
  } // expand


  /** Remove one item from the end of the array. */
  void remove(void)
  {
    if (num_elem > 0)
      num_elem--;
  }

}; // GrowableArray

#endif
