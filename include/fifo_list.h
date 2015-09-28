
#ifndef FIFO_LIST_H
#define FIFO_LIST_H

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
   template class FIFO_LIST

   This is a generic list type for a list that has both a head and a
   tail pointer.  In this list, items are added to the tail.  When
   read from the front of the list, items will be read in a first-in,
   first-out order (FIFO).  The template should be instantiated with a
   scalar type, like an integer or a pointer to a larger type (e.g., a
   string or a structure).  For example

<pre>
      FIFO_LIST<char *> list;
      FIFO_LIST<my_struct *> list;
      FIFO_LIST<my_class *> list;
</pre>


  The list "links" are allocated in a memory pool.  This allocation
  is handled by the local version of new in list_type.  The memory
  allocated for the list_type objects will be deallocated when
  the memory pool is freed.

*/

template <class T>
class FIFO_LIST
{
public:
    /**
      List backbone class
     */
    class list_type {
    public:
        /** data element */
        T data;
	/** pointer to the next element */
        list_type *next;
        /** override the default new operator to allocate
           list_type objects from the a memory pool */
        void *operator new(size_t num_bytes)
        {
          block_pool mem_pool;

          void *mem_addr = mem_pool.pool_alloc( num_bytes );
          return mem_addr;        
        } // new
    };  // class list_type

private:
  /** list head */
  list_type *list;
  /** list tail (items are added to the tail) */
  list_type *tail;

public:
  /** define a handle type to abstract the list_type type */
  typedef list_type *handle;

public:
  /** class constructor */
  FIFO_LIST(void) 
  { 
    list = 0;
    tail = 0;
  }

  /** default destructor does nothing */
  ~FIFO_LIST(void) {}


  /** deallocate the list */
  void dealloc(void)
  {
    while ( remove() != 0 )
      /* nada */;
  } // dealloc


  /** add an element to the FIFO list */
  void add( T data )
  {
    list_type *t;
    
    t = new list_type();
    t->data = data;
    t->next = 0;
    if (list == 0) {
      list = t;
      tail = t;
    }
    else {
      tail->next = t;
      tail = t;
    }
  }  // add


  /** reverse the list */
  void reverse(void)
  {
    list_type *elem, *prev, *next;

    prev = 0;
    next = 0;

    tail = list;
    for (elem = list; elem != 0; prev = elem, elem = next) {
      next = elem->next;
      elem->next = prev;
    } // for 
    list = prev;
  }  // reverse


  /** return the lenght of the list */
  size_t length(void)
  {
      list_type *elem;
      size_t cnt = 0;

      for (elem = list; elem != 0; elem = elem->next)
          cnt++;
      return cnt;
  }  // lenght

  /**
     remove

     Remove an element from the start of the list and return the first
     element of the remaining list.

     This function relies on the fact that the list elements were
     allocated from a pool.  The memory for these elements will be
     recovered when the pool is deallocated.

     */
  handle remove(void)
  {
    list_type *t;

    if (list != 0) {
      t = list;
      list = t->next;
      // no delete t;
    }

    if (list == 0)
        tail = 0;

    return list;
  } // remove


  /** given a handle, return the associated data item */
  T get_item( handle h)
  {

    return h->data;
  } // get_item


  /** get the first element from the list */
  handle first(void)
  {
    return list;
  } // first


  /** return the last element in the list */
  handle last(void)
  {
    return tail;
  } // last


  /** iterator to get the next element */
  handle next(handle h)  
  {
    list_type *next = 0;

    if (h != 0) {
        next = h->next;
    }

    return next;
  } // next
  
};  // template class FIFO_LIST


#endif

