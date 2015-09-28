

#ifndef _PACKTREE_BASE_H_
#define _PACKTREE_BASE_H_


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

#include "packnode.h"
#include "liftbase.h"


/**
  Base class for wavelet packet trees.  Subclasses for this
  class include classes to built wavelet packet trees for
  best basis calculation and wavelet packet trees for
  frequency analysis.

  \author Ian Kaplan

 */
class packtree_base {
protected:
  /** root of the wavelet packet tree */
  packnode<double> *root;

  /** wavelet packet transform object */
  liftbase<packcontainer, double> *waveObj;

  typedef enum { BadPrintKind, 
                 printData, 
                 printCost, 
                 printBestBasis } printKind;

  void breadthFirstPrint(printKind kind);

  void newLevel( packnode<double>* top, bool freqCalc, bool reverse );

public:
  void pr(); 
  /** get the root of the wavelet packet tree */
  packnode<double> *getRoot() { return root; }
}; // packtree_base

#endif
