
#ifndef _YAHOOTS_H_
#define _YAHOOTS_H_



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


/** Process historical equity (stock) data downloaded from
  finance.yahoo.com.  The data is downloaded in "spread sheet" format
  from the historical data page.  There is probably some limitation on
  using this data (e.g., no commercial use and no resale) so use at
  your own risk.

  The format of the file is ASCII.  The first line lists the title
  for each of the fields in the file.  The titles and the fields
  are comma separated.

  This class is specific to the data format that was downloaded
  from Yahoo at the time.  More general code could be written to
  easily account for changing formats.  However, I just wanted 
  to extract the data.

  The Yahoo data has two places of accuracy, presumably reflecting
  decimalization.  The equity time series are adjusted for splits
  and dividends, backward in time from the most recent time in the
  time series.  This can cause problems over long periods of time
  since at some point a stock that pays dividends will pay all of
  it's worth out in dividends and the value will become negative
  (as a result, a reinvest is a better choice).

  The format for the data is:

     &lt;title line&gt;
     &lt;time series line&gt;+

  (e.g,. a titled followed by one or more time series lines).

  The title line consists of six comma separated strings
  (e.g., "Date,Open,High,Low,Close,Volume").  Time time
  series lines have the values suggested in the title.
  For my current purposes I am not interested in date values,
  so these are ignored.  All values are returned as vectors
  of doubles, although volume is an unsigned integer value.

 */
class yahooTS
{
private:
  const char *path_;

public:

  typedef enum { badEnum,
		 Open,
		 High,
		 Low,
		 Close,
		 Volume,
		 lastEnum } dataKind;

  yahooTS() 
  { 
    path_ = 0;
  };
  yahooTS( const char *p ) : path_(p) {}
  
  const double *getTS( const char *fileName,
		       double *a, 
		       size_t &N, 
		       dataKind kind ) const;

  void path( const char *p ) { path_ = p; }
  const char *path() { return path_; }

private:
  const char *getStr_( char *&line,
		       char *buf,
		       size_t bufSize ) const;

  void parseVals_( char *line,
		   double *vals,
		   const size_t n ) const;

  const double getValue_( char *line, 
			  const yahooTS::dataKind kind ) const;

}; // yahooTS


#endif
