
#ifndef _QUEUE_H_
#define _QUEUE_H_

#include "fifo_list.h"
#include "packnode.h"

/** \file

  This file defines a queue (FIFO) object for use in 
  breadth first tree traversal of a wavelet packet tree.

  See chapter 8 of <i>Ripples in Mathematics</i> by 
  Jensen and la Cour-Harbo for a description of
  the wavelet packet algorithm.

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
  A queue element.  This class stores a pointer to a wavelet
  packet tree node (a packnode) and the indentation (in spaces)
  to be used when printing the tree.

  \author Ian Kaplan

 */
template <class T>
class queueElem
{
private:
  /** disallow the default constructor */
  queueElem() {}
public:
  packnode<T> *node;
  size_t indent;

  /** Initialize the queue element */
  queueElem( packnode<T> *n, size_t i )
  {
    node = n;
    indent = i;
  }

  /** allocate queueElem objects from a memory pool */
  void *operator new(size_t num_bytes)
  {
    block_pool mem_pool;

    void *mem_addr = mem_pool.pool_alloc( num_bytes );
    return mem_addr;	  
  } // new
}; // queueElem


/**
  The queue class extends the FIFO_LIST template (which
  in this case is instantiated with queueElem *.  This
  class is designed to support breadth first printing
  of a wavelet packet tree.

  \author Ian Kaplan

 */
template <class T>
class queue : protected FIFO_LIST<queueElem<T> *> 
{
public:
  /** Get the first element in the queue */
  queueElem<T> *queueStart()
  {
    handle h = first();
    queueElem<T> *elem = get_item( h );
    return elem;
  } // queueStart

  /**
    Remove the element at the start of the list.  This function does
    not actually call delete to recover the object.  It relies on the
    fact that the FIFO_LIST template uses pool allocation and the
    memory will be recovered when the pool is deallocated.
   */
  void deleteStart() 
  {
    handle h = first();
    if (h != 0) {
      queueElem<T> *elem = get_item( h );
      remove();
      // no delete elem;
    }
  } // deleteStart

  /** Add an element to the queue */
  void addQueue(packnode<T> *node, size_t indent )
  {
    queueElem<T> *elem = new queueElem<T>(node, indent);
    add( elem );
  } // addQueue

  /** return true if the queue is empty */
  bool queueEmpty() { return (first() == 0); }
}; // queue

#endif
