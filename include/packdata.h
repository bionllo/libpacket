
#ifndef _PACKDATA_H_
#define _PACKDATA_H_


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

#include "blockpool.h"

/**

  The packdata class is a container for the core information
  for a wavelet packet transform node.  This class serves
  as the base class for the packnode class.  It is also
  the element used in the list that results from the wavelet
  packet transform.  This list represents the data in a minimal
  for, relative to the wavelet packet transform cost function.

  \author Ian Kaplan

 */
template <class T>
class packdata {
private:
  /** Disallow the copy constructor */
  packdata( const packdata &rhs ) {}

public:
  typedef enum { BadNodeKind,
                 OriginalData,
                 LowPass,
                 HighPass } transformKind;

protected:
  /** Kind of data: original data, result of the
      low pass filer (wavelet scaling function), result 
      of the high pass filter (wavelet function) */
  transformKind kind;

  /** number of elements at this packet tree level */
  size_t N;

  /** Wavelet packet data */
  T* data;

  /** default constructor */
  packdata() {}

public:
  /**
    \arg vec a pointer to an array of type <i>T</i> elements
    \arg n the size of the array of type <i>T</i> elements
    \arg k the kind of data (e.g., original data, low pass result,
           high pass result.
   */
  packdata( T *vec, const size_t n, const transformKind k )
  {
    data = vec;
    N = n;
    kind = k;
  }

  /** define a virtual destructure which in the base class */
  virtual ~packdata() {}

  /**
    Overload the standard <i>new</i> operator and allocate
    memory from the memory pool.
   */
  
  void *operator new( size_t num_bytes )
  {
    block_pool mem_pool;

    void *mem_addr = mem_pool.pool_alloc( num_bytes );
    return mem_addr;
  } // new
  

  /** print the data */
  void pr() const
  {
    for (int i = 0; i < N; i++) {
      printf("%7.4f ", data[i] );
    }
    printf("\n");
  } // pr


  /** get a pointer to the data */
  const T* getData() { return data; }

  /**
    Return the "kind" of data (e.g., original data,
    low pass result, high pass result).
   */
  const transformKind getKind() { return kind; }

  /** return the length of the array in the packdata object */
  size_t length() { return N; }
}; // packdata 


#endif
