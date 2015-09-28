
#ifndef _BLOCKPOOL_H_
#define _BLOCKPOOL_H_

/** \file
  
  The documentation in this file is formatted for doxygen
  (see www.doxygen.org).

  <b>Copyright and Use</b>

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


#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

/**
 
  This class supports memory pool allocation.  A memory pool is
  allocated in blocks and smaller chunks of memory are allocated from
  these blocks.  Instead of calling a set of class destructors (which
  is time consuming) the memory pool allows all the objects allocated
  in the memory pool to be destroyed at once.  This provide a sort of
  "poorman's garbage collector.  Deallocating memory in a single place
  simplifies the software structure, since allocation can be scattered
  throughout the code without worry about deallocation.

  This is a simplified version of a low level memory allocator that
  can be used to create multiple memory pools.  In this class
  the class variables are static and shared by all instances of the
  class.  This allows the class to be declared locally to allocate
  memory, but the state remains global.  This makes the local
  instance a window into the global state.  The limitation is that
  only one memory pool can be used.

  Originally written November, 1996<br>
  Revised for the wavelet packet transform code March 2002

  \author Ian Kaplan

*/

class block_pool {
public: // typedefs and variables
  /** the largest block of memory that can be allocated is the
     page_size * max_block_multiple */
  typedef enum { one_kay = 1024,
		 page_size = (4 * one_kay),  /* 4 Kb */
		 max_block_multiple = 256,   /* 1 Mb */
		 last_enum
	       } bogus;

  /** typedef for memory block chain */
  typedef struct block_chain_struct {
    /** pointer to the current block */
    void *block;

    /** number of bytes used in the block */
    size_t bytes_used;

    /** total block size */
    size_t block_size;
       
    /** pointer to the next block */
    block_chain_struct *next_block;
  } block_chain;

private:
  /** allocation granularity */
  static size_t alloc_gran;

  /** start of the block list for this pool */
  static block_chain *block_list_start; 

  /** current block memory is being allocated from */
  static block_chain *current_block;    


private: // class functions
  block_chain *new_block( size_t block_size );
  void *add_block( size_t block_size );
  void init_pool(void);


protected:
  /**
    Allocate memory using <i>calloc</i>.  The POSIX calloc function
    sets the memory to zero (in contrast to <i>malloc</i> which
    allocates the memory without initializing it.

<pre>
      #include <stdlib>
      void *calloc( size_t num, size_t size );
</pre>
   
    The arguments to calloc are:

      <b>num</b>: number of elements<br>
      <b>size</b>: size of the elements in bytes

    The argument to MemAlloc is the size, in bytes
    to allocate.

   */
  virtual void *MemAlloc( size_t n_bytes )
  {
    void *rtn = calloc( n_bytes, 1 );
    return rtn;
  }
  /**
    Free memory that has been allocated with MemAlloc
   */
  virtual void MemFree( void *addr )
  {
    free( addr );
  }


public: // class functions
  /** constructor does nothing, since all the class variables
      are static */
  block_pool(void) {}

  void free_pool(void);
  void *pool_alloc( size_t block_size );
  void print_block_pool_info( FILE *fp = stdout );

}; // class block_pool



/* Macros for block_chain pointers */

/** return the memory block */
#define Chain_block(p)            ((p)->block)

/** return the number of bytes used */
#define Chain_bytes_used(p)       ((p)->bytes_used)

/** return the size of the memory block */
#define Chain_block_size(p)       ((p)->block_size)

/** return the next block_chain structure in the list */
#define Chain_next(p)             ((p)->next_block)


#endif
