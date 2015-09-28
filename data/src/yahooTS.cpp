
/** \file

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

#include <errno.h>
#include <stdio.h>
#include <string.h>

#include "yahooTS.h"


/**
  Copy from the input string until either the end of the string
  (e.g., the null) is reached or a comma is found.

  \param line A reference to a pointer to the input string.  This
              pointer is incremented until either the end of string or
              a comma is encountered.  When this function returns
              <i>line</i> will either point to the end of the string
              or a character following a comma.
	      
  \param buf A buffer into which the string will be copied.

  \param bufSize The size of <i>buf</i>.

 */
const char *yahooTS::getStr_( char *&line,
			      char *buf,
			      size_t bufSize ) const
{
  const char *rtnPtr = 0;
  if (line != 0) {
    for (size_t charCnt = 0; charCnt < bufSize-1 && *line != '\0'; charCnt++) {
      if (*line == ',') {
	line++;
	break;
      }
      else {
	buf[charCnt] = *line++;
      }
    }
    
    buf[charCnt] = '\0';
    if (charCnt > 0)
    {
      rtnPtr = buf;
    }
  }
  return rtnPtr;
} // getStr_



/**
  Parse a comma separated line of values into a vector of
  doubles.

  The comma separated values are:

      Date,Open,High,Low,Close,Volume

  The date value is skipped.

  \param line A pointer to a line of Yahoo historical data

  \param vals A vector of doubles that the values in the 
         historical data line will be stored.

  \param n The number of elements in <i>vals</i>

 */
void yahooTS::parseVals_( char *line,
			  double *vals,
			  const size_t n ) const
{
  char buf[128];
  const char *ptr;

  // skip the date
  ptr = getStr_( line, buf, sizeof( buf ) );
  if (ptr == 0) {
    fprintf(stderr, "parseVals: date expected\n" );
    return;
  }

  // get the Open, High, Low, Close and Volume values
  size_t cnt = 0;
  for (dataKind kind = Open; 
       kind <= Volume && cnt < n; 
       kind = (dataKind)((size_t)kind + 1)) {

    ptr = getStr_( line, buf, sizeof( buf ) );
    if (ptr == 0) {
      fprintf(stderr, "parseVals: value expected\n");
      return;
    }

    double v;

    sscanf( buf, "%lf", &v );
    vals[cnt] = v;
    cnt++;
  }

} // parseVals_


/**
  A data line from a Yahoo historical data file consists of
  a set of comma separated values:

<pre>
    date,open,high,low,close,volume
</pre>

  This function is passed a Yahoo data line and a <i>kind</i> value
  which indicates which value to return.  Date is is ignored, so the
  value of <i>kind</i> should be one of: Open, High, Low, Close,
  Volume.
  
 */
const double yahooTS::getValue_( char *line, 
				 const yahooTS::dataKind kind ) const
{
  double retval = 0;

  if (kind > badEnum && kind < lastEnum) {
    const size_t NUM_VALS = 5;
    double vals[ NUM_VALS ];

    parseVals_( line, vals, NUM_VALS );

    size_t ix = (size_t)kind - 1;
    if (ix < NUM_VALS) {
      retval = vals[ix];
    }
  }

  return retval;
} // getValue


/**
  Read a Yahoo equity time series from a file.

  Yahoo allows historical equity data to be downloaded
  in "spread sheet" format.  In this format there is a
  title line, listing the data columns (e.g., date,
  open, high, low, close and volume).  Following the
  title line are comma separated values.  In reading
  this Yahoo data file, the first line is skipped.

  The Yahoo data values are listed from most recent to
  oldest.  In the data vector returned, a[0] will be
  the oldest and a[N-1] will be the most recent.

  \param fileName name of the file containing the time series.
         This file will be prefixed by the path in the class
         variable path_.

  \param a A pointer to a vector of doubles that will
         be initialized with values from <i>fileName</i>.

  \param N Number of doubles that will fit in <i>a</i>
         N is an input/output variable.  The value
	 returned in N will be the actual number of values
	 read.

  \param kind The kind of time series to fetch from
         <i>fileName</i> (e.g., open, high, low, close,
         volume.

  \return If there was no error reading data from <i>fileName</i>
          the function returns a pointer to the initialized
          array (e.g., the argument <i>a</i>).  If the data could
          not be read, a null pointer (0) is returned.

 */
const double *yahooTS::getTS( const char *fileName,
			      double *a,
			      size_t &N,
			      const yahooTS::dataKind kind ) const
{
  const double *rtnPtr = 0;
  char fullPath[512];
  size_t freePath = sizeof( fullPath );
  FILE *fptr;

  if (path_ != 0) {
    strncpy( fullPath, path_, freePath-1 );
    freePath = freePath - strlen( fullPath );
  }
  strncat( fullPath, fileName, freePath-1 );
  fptr = fopen( fullPath, "r" );
  if (fptr != 0) {
    char line[512];
    size_t lineSize = sizeof( line );
    int ix = N-1;

    if (fgets( line, lineSize, fptr ) != 0) {
      rtnPtr = a;
      while (fgets( line, lineSize, fptr ) != 0) {
	if (ix >= 0) {
	  a[ix] = getValue_( line, kind );
	  ix--;
	}
	else {
	  break;
	}
      } // while
    }
    else {
      fprintf(stderr, "getTS: title line expected\n");
    }
    ix++;
    N = N - ix;
  }
  else {
    const char *error = strerror( errno );
    fprintf(stderr, "getTS: Error opening %s: %s\n", fullPath, error );
  }

  return rtnPtr;
} // getTS
