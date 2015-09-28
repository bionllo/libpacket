
#include "support.h"


/** 

  Represent a real value with three fractional digits in integer
  form.  To accomplish this the real number is rounded and then
  multiplied by 1000.0.

  The rounding used is so proper rounding:  

  <ul>
  <li>
  If the digit in the fourth
  place is greater than 5, the digit in the third place is incremented
  by one.  
  </li>
  <li>
  If the digit in the fourth place is less than 5, the third digit
  is left unchanged.  
  </li>
  <li>
  If the digit in the fourth place is 5 and the digit in the third
  place is odd, the third digit is incremented.
  </li>
  </ul>

  Proper rounding is unbiased (that is, in a random sample of numbers
  the same number of values are rounded up as are rounded down).
  
  Some examples:

  <pre>
     12.4567 is rounded to 12.457 and converted to the integer
     12457.

     42.1234 is rounded to 42.123 and converted to the integer
     42123.

     127.1235 is rounded to 127.124 and converted to the integer
     127124.
  </pre>

  While this function does what I intended, it does seem to do it
  in a lot of operations.  There is probably a faster way to do this
  by directly manipulating IEEE floating point.  While this might
  be faster, it is more complex.

  */ 
int support::roundVal_( const double val )
{ 
  int intPart =  static_cast<int>(val); 
  double fracPart = val - intPart; 
  int threeDigits = fracPart * 1000.0; 
  int fourDigits = fracPart * 10000.0; 
  int forthDigit = fourDigits % 10; 
  int thirdDigit = threeDigits % 10;

  double roundVal = 0.001;
  if (forthDigit < 5) {
    roundVal = 0.0;
  }
  else if (forthDigit == 5) {
    if ((thirdDigit & 0x1) == 0) {
      roundVal = 0.0;
    }
  }
  double newVal = val + roundVal;
  double intRslt = newVal * 1000.0;
  return intRslt;
} // roundVal_


/**
  Round an array of doubles to three decimal places and multiply
  by 1000, resulting in an integer that reflects the double
  value.

  For equity time series this kind of rounding is useful for
  "old style" pre-decimalization time series where the fractional
  values represent increments by 1/16.  This kind of rounding is
  also useful for adjusted time series (e.g., the time series
  is adjusted by a split and/or dividends are reinvested).

  Since this function results in an integer vector with integer
  values reflecting three decimal places, this function should
  not be used for decimalized values (which only have two 
  decimal places).


 */
void support::roundToInt( int *intVec, 
			  const double *realVec, 
			  const size_t len )
{
  if (intVec != 0 && realVec != 0) {
    for (size_t i = 0; i < len; i++) {
      intVec[i] = roundVal_( realVec[i] );
    }
  }
} // roundToInt


/**
  Convert a decimalized array of doubles (e.g., where there
  are two base ten fractional digits (e.g., 6.02, 3.14, 1.15)
  into integer form.  This is done my multiplying by 100 and
  truncating.  No rounding is done, since this function 
  assumes that here are only two significant fractional digits.
  The <tt>roundToInt</tt> function can be used for rounding.
 */
void support::decimalToInt( int *intVec, 
			    const double *realVec, 
			    const size_t len )
{
  if (intVec != 0 && realVec != 0) {
    for (size_t i = 0; i < len; i++) {
      intVec[i] = (int)(realVec[i] * 100.0);
    }
  }
} // roundToInt




size_t support::nearestPower2Width_( size_t val )
{
  size_t width = 0;
  if (val > 0) {
    width = 1;
    size_t power = 1;
    while (power < val && width < 32) {
      power = power << 1;
      width++;
    }
  }
    
  return width;
} // nearestPower2Width_


/**

  Calculate the number of bits needed to represent an integer
  value.  A sign bit is added, so that positive numbers always
  have a leading zero and negative numbers have a leading one.

*/
size_t support::valWidth( const int val )
{
  size_t wholeNum = (val < 0) ? -val : val;
  size_t width = 1 + nearestPower2Width_( wholeNum );

  return width;
} // valWidth


/**

  Calculate the number of bits needed to represent an unsigned
  value.  

*/
size_t support::UnsignedValWidth( const size_t val )
{
  size_t width = nearestPower2Width_( val );

  return width;
} // valWidth


/**
  Calculate the minimum number of bits needed to represent the
  values in an integer vector.
 */
size_t support::vecWidth( const int *vec, const size_t N )
{
  size_t totalWidth = 0;
  if (vec != 0) {
    for (size_t i = 0; i < N; i++) {
      totalWidth += valWidth( vec[i] );
    }
  }
  return totalWidth;
} // vecWidth( int *)
