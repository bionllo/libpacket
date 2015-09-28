

#ifndef _PACKDATA_LIST_H_
#define _PACKDATA_LIST_H_

#include "fifo_list.h"
#include "packdata.h"


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

  \author Ian Kaplan

 */


/**

  The packdata_list class is based on a subclass built from a
  FIFO_LIST<packdata *> instance of the FIFO_LIST template.

  The packdata_list is constructed by traversing the wavelet packet
  tree depth first, left to right.  New elements are added to the end
  of the list, ordering the list from front to back.  The resulting
  list reflects the ordering of the wavelet packet tree nodes the make
  up the best basis.

  This class extends the FIFL_LIST instance by adding a print function.

  \author Ian Kaplan

 */
template <class T>
class packdata_list : public FIFO_LIST<packdata<T> *>
{
public:

  /**
    Print the packet data list.  Each list element (which is a wavelet
    packet data set) is printed one element per line.

    */
  void pr()
  {
    handle h;
    packdata<T> *elem;

    for (h = first(); h != 0; h = next( h )) {
      elem = get_item( h );
      elem->pr();
    } // for h
    printf("\n");
  } // pr

}; // packdata_list


#endif
