
#ifndef _LIST_H_
#define _LIST_H_


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
   template class LIST

   This is a generic list type.  It should be instantiated
   with a scalar type, like an integer or a pointer to
   a larger type (e.g., a string or a structure).  For example

<pre>   
      LIST<char *> list;
      LIST<my_struct *> list;
</pre>

  The list "links" are allocated in a memory pool.  This allocation
  is handled by the local version of new in list_type.  The memory
  allocated for the list_type objects will be deallocated when
  the memory pool is freed.

*/

template <class T>
class LIST
{
public:
    /**
      List backbone class
     */
    class list_type {
    public:
        /** list data element */
        T data;
	/** pointer to the next element in the list */
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

public:  
  /** define a handle type to abstract the list_type type */
  typedef list_type *handle;

public:
  /** class constructor */
  LIST(void) 
  { 
    list = 0;
  }


  /** The copy constructor copies the list pointer */
  LIST( const LIST<T> &rhs )
  {
    list = rhs.list;
  }

  /** destructor does nothing */
  ~LIST() {} 


  /** deallocate the list */
  void dealloc(void)
  {
    while ( remove() != 0 )
      /* nada */;
  } // dealloc


  /** Add an element to the list */
  void add( T data )
  {
    list_type *t;
    
    t = new list_type();
    t->data = data;
    t->next = 0;
    if (list == 0) {
      list = t;
    }
    else {
      t->next = list;
      list = t;
    }
  }  // add


  /** reverse the list */
  void reverse(void)
  {
    list_type *revlist = 0;
    list_type *next;

    for (list_type *t = list; t != 0; t = next) {
        next = t->next;
        t->next = revlist;
        revlist = t;
    }
    list = revlist;
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


  /** iterator to get the next element */
  handle next(handle h)  
  {
    list_type *next = 0;

    if (h != 0) {
        next = h->next;
    }

    return next;
  } // next
  
};  // template class LIST


#endif

