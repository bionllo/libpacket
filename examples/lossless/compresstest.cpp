
/** \file

  This file contains code to test various lossless compression
  algorithms on financial time series (e.g., stock close price).

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


#include <stdio.h>

#include "invpacktree_int.h"
#include "packtree_int.h"
#include "support.h"
#include "delta.h"
#include "haar_int.h"
#include "ts_trans_int.h"
#include "line_int.h"
#include "costwidth.h"
#include "yahooTS.h"


/**

  Make a copy of an integer array.  Note that the function
  allocates memory, but does not deallocate it.
 */
int *copy( int *intVec, const size_t N )
{
  int *newVec = new int[ N ];

  for (size_t i = 0; i < N; i++)
  {
    newVec[i] = intVec[i];
  }

  return newVec;
} // copy


/**
  Compare two integer arrays of size N.  Return true if they are
  equal, false otherwise.
 */
bool compare( const int *v1, const int *v2, const size_t N )
{
  bool rslt = true;

  for (size_t i = 0; i < N; i++) 
  {
    if (v1[i] != v2[i])
    {
      rslt = false;
      break;
    }
  }

  return rslt;
} // compare



/**
  Calculate the number of bits needed to represent the result
  of the wavelet packet transform.

  The result of this version of the wavelet packet transform 
  is a list of vectors, where each vector represents a best basis
  fit for a particular region of the signal.

  The result returned here is an obvious lower bound, since in 
  practice a vector would consist of a fixed set of values.  Also
  the length of the vector would have to be included, along
  with the width the the length values.
 */
size_t calcPacketWidth( packdata_list<int> &bestBasisList )
{
  packdata_list<int>::handle h;
  size_t totalWidth = 0;

  for (h = bestBasisList.first(); h != 0; h = bestBasisList.next( h )) {
    packdata<int> *node = bestBasisList.get_item( h );
    const size_t len = node->length();
    const int *vec = node->getData();
    int nodeWidth = support::vecWidth( vec, len );
    totalWidth += nodeWidth;
  }

  return totalWidth;
} // calcPacketWidth



/**
  Wavelet packet calculation

  The function returns the minimal total number of bits needed
  to represent the data when compressed using the wavelet
  packet algorithm.

 */
size_t packet_calc( const int *intVec, 
		    const int *copyVec, 
		    const int N )
{
  // The "line" wavelet transform (e.g., line with slope)
  line_int<packcontainer_int> line;

  // calculate the wavelet packet tree, using the line wavelet transform
  packtree_int tree( intVec, N, &line );

  // get the root of the wavelet packet transform tree
  packnode<int> *treeRoot = tree.getRoot();

  // Assign a cost on the basis of bit width
  costwidth cost( treeRoot );

  // Calculate the "best basis" function from the tree
  tree.bestBasis();

  // Check that the best basis function succeeded.  That is,
  // that the best basis function does not include the 
  // original data.
  
  if (! tree.bestBasisOK()) {
    printf("Best basis calculation failed\n");
  }

  // Get the best basis list.  This will be a list of
  // nodes consisting of the best basis set.  This set is
  // obtained by traversing the tree, top down, left to
  // right.
  packdata_list<int> bestBasis = tree.getBestBasisList();

  // Sum the cost values (width for each node in the best basis
  // list
  size_t width = calcPacketWidth( bestBasis );

  invpacktree_int invtree( bestBasis, &line );

  const int *invRslt = invtree.getData();

  bool isEqual = compare( invRslt, copyVec, N );
  if (! isEqual)
    printf("Wavelet packet inverse is wrong\n");

  return width;
} // packet_calc


/**

  Calculate the number of bits needed to represent the data after
  "delta" compression.  Delta compression stores the difference
  between value s<sub>i-1</sub> and s<sub>i</sub>.

 */
size_t delta_calc( int *intVec, 
		   const int *copyVec, 
		   const int N )
{
  delta_trans<int> delta;

  delta.forward( intVec, N );

  const size_t deltaWidth = support::vecWidth( intVec, N );

  delta.inverse( intVec, N);

  bool isEqual = compare( intVec, copyVec, N);
  if (! isEqual)
    printf("Delta compression inverse failed\n");

  return deltaWidth;
} // delta_calc


/**
  Calculate the number of bits needed to represent the data
  after wavelet compression wavelet compression.

  \param intVec A pointer to an array of integers
  
  \param copyVec A pointer to a copy of the integer array.
         This is used to verify that the inverse(forward(intVec))
         transform yields the original data.

  \param N the size of intVec and copyVec

  \param w a pointer to a wavelet class

 */
size_t wave_calc( int *intVec, 
		  const int *copyVec, 
		  const int N,
		  liftbase<int *, int> *w )
{
  w->forwardTrans( intVec, N );
  
  const size_t waveWidth = support::vecWidth( intVec, N );

  w->inverseTrans( intVec, N );

  bool isEqual = compare( intVec, copyVec, N);
  if (! isEqual)
    printf("Line wavelet inverse is wrong\n");

  return waveWidth;
} // wave_calc



/**
  Read in a set of equity time series file.  Calculate the
  number of bits needed to represent the data without
  compression and after wavelet and wavelet packet
  compression.
 */
int main()
{
  const char *files[] = { "aa",    // Alcoa Aluminium
			  "amat",  // Applied Materials
			  "ba",    // Boeing
			  "cof",   // Capital One
			  "ge",    // General Electric
			  "ibm",   // IBM Corp.
			  "intc",  // Intel
			  "mmm",   // 3M
			  "mrk",   // Merck
			  "wmt",   // Wal-Mart
			  0        // The null pointer
			};

  const size_t N = 512;
  double realVec[ N ];
  int intVec[ N ];

  // an instance of yahooTS with the path to the data directory
  const char *dataDirPath = "..\\data\\equities\\";
  yahooTS ts( dataDirPath );

  printf("Equity Uncompressed  delta  Haar  line  TS    wavelet packet (line)\n");


  for (size_t i = 0; files[i] != 0; i++) {
    
    size_t n = N;
    if (! ts.getTS( files[i], realVec, n, yahooTS::Close )) {
      break;
    }

    if (n != N) {
      printf("Error: %d out of %d data elements read\n", n, N );
      break;
    }
    
    support::decimalToInt( intVec, realVec, N );
    
    int *copyVec = copy( intVec, N );
    
    const size_t beforeWidth = support::vecWidth( intVec, N );
    
    const size_t deltaWidth = delta_calc( intVec, copyVec, N );
    
    haar_int haar;
    const size_t haarWidth = wave_calc( intVec, copyVec, N, &haar );
    
    line_int<int *> line;
    const size_t lineWidth = wave_calc( intVec, copyVec, N, &line );
    
    ts_trans_int ts_trans;
    const size_t tsTransWidth = wave_calc( intVec, 
					   copyVec, 
					   N, 
					   &ts_trans);
    
    const size_t packetWidth = packet_calc( intVec, copyVec, N );
    
    printf("  %4s       %4d    %4d   %4d  %4d  %d      %4d\n", 
	   files[i], beforeWidth, deltaWidth, haarWidth, lineWidth, tsTransWidth, packetWidth );
    
  }
  return 0;
}
