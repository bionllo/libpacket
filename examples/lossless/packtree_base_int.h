

#ifndef _PACKTREE_BASE_INT_H_
#define _PACKTREE_BASE_INT_H_


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


#include "packnode.h"
#include "liftbase.h"
#include "packcontainer_int.h"

/**
  This is an integer version of the wavelet packet tree base
  class.  This class can be used for both compression and
  time/frequency analysis of an integer time series.  However,
  this version was developed for lossless integer compression.
  
  This class may be subclassed for classes that apply the
  wavelet best basis algorithm or for time/frequency 
  wavelet packet trees.

 */
class packtree_base_int {
protected:
  /** root of the wavelet packet tree */
  packnode<int> *root;

  /** wavelet packet transform object */
  liftbase<packcontainer_int, int> *waveObj;

  typedef enum { BadPrintKind, 
                 printData, 
                 printCost, 
                 printBestBasis } printKind;

  void breadthFirstPrint(printKind kind);

  void newLevel( packnode<int>* top, bool freqCalc, bool reverse );

public:
  void pr(); 
  /** get the root of the wavelet packet tree */
  packnode<int> *getRoot() { return root; }
}; // packtree_base_int

#endif
