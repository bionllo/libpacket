
#ifndef _SUPPORT_H_
#define _SUPPORT_H_


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
  Various support functions for compression and bit width estimation.
  These functions are the object oriented version of global functions.
  They are all pure functions (e.g., no state in the function or
  in the class) and static.

  <b>Bit width calculation</b>

  In compression applications an attempt is made to represent a data
  set in a minimal number of bits.  For these applications it must
  also be possible to decompress the data.  If the values in the
  data set are stored in bit fields of different widths, information
  about these fields must be stored as well.

  Unlike compression algorithms, where decompresson must be taken
  into account, this code simple calculates the sum of the bit
  fields needed to represent a set of signed integer values.
  This is useful for estimating how closely a wavelet transform
  approximated a data set.

  <b>Double to integer conversion</b>

  The compression algorith uses integer to integer lossless
  wavelet transforms.  Since the financial data I am interested
  in is in real form, it needs to be converted to integer.
  This is done preserving three fractional digits.

  The constructors and destructor are declared but not defined,
  which will result in a link error if an instance of this class
  is created (which is not the intent in the design).
 */
class support
{
private:
  static size_t nearestPower2Width_( size_t val );
  static int roundVal_( const double val );

public:
  /** declare but do not define the constructor */
  support();
  /** declare but do not define the destructor */
  ~support();
  /** declare but never define copy constructor */
  support( const support &rhs );

  static size_t valWidth( const int val );

  size_t UnsignedValWidth( const size_t val );

  static size_t vecWidth( const int *vec, 
				const size_t N );
  static void roundToInt( int *intVec, 
			  const double *realVec, 
			  const size_t len );

  static void decimalToInt( int *intVec, 
			    const double *realVec, 
			    const size_t len );
}; // support

#endif
