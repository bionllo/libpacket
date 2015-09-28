
/** \file


  This file contains the class functions for a "pool" based memory
  allocator.  These functions manage the block chain that makes up the
  memory pool and allocates memory from the individual blocks.  The
  entire pool is freed at once.

  The pool allocator was originally written as a low level allocation
  package.  The original package queries the system to find the page
  size and allocation block size.  This version has been simplified,
  but some of the low level nature remains.

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
#include <stdlib.h>

#include "blockpool.h"


size_t block_pool::alloc_gran = (size_t)block_pool::page_size;
block_pool::block_chain *block_pool::current_block = 0;
block_pool::block_chain *block_pool::block_list_start = 0;



/**
   block_pool::init_pool
  
   This function is automatically called to initialize the block_pool
   object when the "current_block" pointer is null.

*/  
void block_pool::init_pool( void )
{

  block_chain *new_link;

  new_link = new_block( alloc_gran );
  block_list_start = new_link;
  current_block = new_link;
} /* init_pool */




/**
   new_block

   The new_block function is the "root" memory allocator for the
   block_pool object.  The amount of memory allocated is rounded up to
   the next "block_size" boundary.  Both the block_chain structure and
   the allocatible memory are allocated from a single block that is a
   multiple of the page size.  This should avoid fragmentation in the
   system memory allocator.

   The "page" referenced here is a virtual memory page.  This version
   of the code does not actually check the system page size, but assumes
   a 4Kb page.

*/  
block_pool::block_chain *block_pool::new_block( size_t block_size )
{
  const size_t max_block_size = max_block_multiple * page_size;
  block_chain *new_link = 0;
  size_t alloc_amt, total_alloc;

  // add in the memory needed for the block_chain structure
  total_alloc = block_size + sizeof(block_chain);
  if (total_alloc < alloc_gran)
      alloc_amt = alloc_gran;
  else { 
      // its larger than the minimum allocation granularity, so round
      // up the the nearest page.
      alloc_amt = ((total_alloc + (page_size-1))/page_size) * page_size;
  }

  if (alloc_amt <= max_block_size) {

    /* Allocate memory for both the block_chain structure and the memory 
       block */
    new_link = (block_chain *)MemAlloc( alloc_amt );

    // The new memory block starts after the block_chain structure
    Chain_block(new_link) = (void *)(((size_t)new_link) + sizeof(block_chain));

    assert( alloc_amt >= block_size );

    Chain_bytes_used(new_link) = 0;
    Chain_block_size(new_link) = alloc_amt - sizeof(block_chain);
    Chain_next(new_link) = 0;
  }
  else {
    printf("block_pool::new_block: allocation request too large\n");
  }

  return new_link;
} // block_chain::new_block



/**

  block_pool::add_block Add a new memory block to the memory pool.
  This function is called when the amount of memory requested by
  pool_alloc will not fit in the current block.

*/
void *block_pool::add_block( size_t block_size )
{
  block_chain *block = 0;
  block_chain *last_block;

  last_block = current_block;
  block = new_block( block_size );
  Chain_next(current_block) = block;
  current_block = block;

  return (void *)block;
} // block_chain::add_block




/**
   pool_alloc

   Allocate memory from the memory pool.  The pool_alloc and free_pool
   functions do memory allocation and deallocation.

   This function is called to allocate memory from the memory pool.
   If there is enough free memory in the current block to satisify the
   memory request, memory is allocated from the current block and the
   amount of free memory is updated.  If the current block does not
   have enough memory, add_block is called to allocate a new memory
   block which will be large enough.

*/
void *block_pool::pool_alloc( size_t num_bytes )
{
  const size_t align = sizeof( size_t );
  void *addr = 0;
  size_t amt_free;

  /* the number of bytes allocated must be a multiple of the align
     size */
  num_bytes = ((num_bytes + (align-1))/align) * align;

  if (current_block == 0) {
    init_pool();
  }

  amt_free = Chain_block_size(current_block) - Chain_bytes_used(current_block);
  
  if (num_bytes > amt_free) {
    if (add_block( num_bytes ) != 0) {
      amt_free = Chain_block_size(current_block);
    }
  }

  if (amt_free >= num_bytes) {
    addr = (void *)((size_t)Chain_block(current_block) + Chain_bytes_used(current_block));
    Chain_bytes_used(current_block) += num_bytes;
  }
  else {
    printf("block_pool::block_alloc: allocation error\n");
    exit(1);
  }
  return addr;
} // block_pool::pool_alloc



/**

   block_pool::free_pool
  
   Walk through the block chain and deallocate the blocks.
   Note that the block chain structures and the allocatible
   memory is contained within a single allocated block.
   The block_chain structure is at the start of this block
   so passing its address to the memory deallocation function
   deallocates both the block chain structure and the allocatible
   memory

*/  
void block_pool::free_pool(void)
{
  block_chain *tmp;

  while (block_list_start != 0) {
    tmp = block_list_start;
    block_list_start = Chain_next(block_list_start);
    MemFree( (void *)tmp );
  }
} // free_pool



/**
   print_block_pool_info
  
   Print information about the block pool

*/
void block_pool::print_block_pool_info( FILE *fp /*= stdout */)
{
  size_t total_allocated = 0;
  size_t total_unused = 0;
  block_chain *ptr = block_list_start;

  fprintf(fp, "Minimum memory allocation size: %d\n", alloc_gran );
  fprintf(fp, "Page size: %d\n", (size_t)page_size );
  fprintf(fp, "[block size, bytes_used]\n");
  while (ptr != 0) {
    fprintf(fp, "[%4d, %4d]", Chain_block_size(ptr), Chain_bytes_used(ptr));
    total_allocated += Chain_bytes_used(ptr);
    total_unused += (Chain_block_size(ptr) - Chain_bytes_used(ptr));
    if (Chain_next(ptr) != 0) {
      fprintf(fp, ", ");
    }
    else {
      fprintf(fp, "\n");
    }
    ptr = Chain_next(ptr);
  } // while
  fprintf(fp, "Total allocated = %5d, total unused = %3d\n", total_allocated,
	 total_unused );
}

