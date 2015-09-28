
#ifndef _PACKFREQ_H_
#define _PACKFREQ_H_


#include "packnode.h"
#include "packcontainer.h"
#include "packtree_base.h"
#include "liftbase.h"
#include "grow_array.h"


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

  Build a wavelet packet tree for frequency analysis.

  Wavelet frequency analysis uses a modified wavelet packet tree.
  Horizontal slices through the modified wavelet packet tree (the so
  called "level basis") are ordered in increasing frequency regions.

  This class and the standard wavelet packet tree class are derived
  from the same base class.  The standard wavelet class includes
  functions to calculate a minimal data representation relative
  to a cost function.

  The constructor for this class is passed two arguments:

  <ol>
  <li>
  A vector of doubles containing the data set (e.g., the signal).
  The length of the vector must be a power of two.
  </li>
  <li>
  A pointer to a wavelet lifting scheme class that will be 
  used in calculating the wavelet transform step.
  </li>
  </ol>

  If the vector passed to the constructor contains N double values, the
  result of the constructor will be a wavelet packet tree with
  log<sub>2</sub>(N) levels.

  \author Ian Kaplan

 */
class packfreq : public packtree_base {
private:
  /** Level basis matrix */
  GrowableArray<packnode<double> *> mat;

  void findLevel( packnode<double>* top, 
		  size_t cur_level, 
		  const size_t level );

protected:
  /** disallow the copy constructor */
  packfreq( const packfreq &rhs ) {};
  /** disallow the default constructor */
  packfreq() {};

public:
  packfreq( const double *vec, 
            const size_t n, 
            liftbase<packcontainer, double> *w ); 

  /** destructor does nothing */
  ~packfreq() {}

  void getLevel( const size_t level );

  void plotMat(const size_t N);

  void prMat();
}; // packfreq

#endif
