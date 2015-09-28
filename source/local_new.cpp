
#include <stdio.h>
#include <stdlib.h>

/** \file

  This file contains overrides for the global new and delete
  operators.  These exist to assure that only the pool allocation
  functions are called.  If you see a message from one of these
  functions then something other than pool allocation is taking
  place.

  This file is only used for testing.  If you don't want to do this
  check you can remove this file from the software build.

 */

void *operator new( size_t num_bytes )
{
  printf("global operator new\n");
  void *rtn = malloc( num_bytes );
  return rtn;
} // new


void *operator new[]( size_t num_bytes )
{
  printf("global operator new []\n");
  void *rtn = malloc( num_bytes );
  return rtn;
}


void operator delete( void *addr )
{
  printf("global operator delete\n");
  free( addr );
}


void operator delete[](void *addr )
{
  printf("global operator delete []\n");
  free( addr );
}
