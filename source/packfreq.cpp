
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
#include <assert.h>
#include <stdio.h>
#include <math.h>

#include "packfreq.h"




/** 

  Construct a wavelet packet tree from a vector of double values.
  The size of the vector, which must be a power of two, is passed in
  N.  A pointer to a wavelet Lifting Scheme object is passed in
  <i>w</i>.  The wavelet Lifting Scheme object is used to calculate
  the wavelet transform step which is applied at each level (where
  level > 0) of the wavelet packet tree.

  The first level (level 0) of the wavelet packet tree contains
  the original data set.

  The "newLevel" function is defined in the base class.  This
  function is used to calculate both the standard and the
  modified wavelet packet trees.  The modified wavelet packet
  tree is calculated by this class and is used for Wavelet
  frequency analysis.

  The wavelet transform step uses two filters <b>H</b> and <b>G</b> to
  calculate the low pass (scaling function) and the high pass (wavelet
  function).  In the standard transform the low pass result is always
  placed in the lower half of the array and the high pass result is
  placed in the upper half of the array.  Each of these two half
  arrays form the input for the next step.  In the modified transform
  calculated by this class, the right child uses the standard
  algorithm where <b>H</b> and <b>G</b> results are in the upper and
  lower halves of the result array.  However, the left child inverts
  the locations of the <b>H</b> and <b>G</b> results, placing the
  result of the <b>G</b> filter in the lower half and the result
  of the <b>H</b> filter in the upper half.

  The two boolean flags that are passed to the newLevel function
  determine whether a frequency ordered wavelet packet tree
  is calculated and whether the location of the filter results
  is inverted.

  \arg vec An array of double values on which the wavelet packet
           transform is calculated.
  \arg N The number of elements in the input array
  \arg w A pointer to the the wavelet transform object to use 
         in calculating the wavelet packet transform.

 */
packfreq::packfreq( const double *vec, 
		    const size_t N, 
		    liftbase<packcontainer, double> *w )
{
  waveObj = w;

  block_pool mem_pool;
  double *vecCopy = (double *)mem_pool.pool_alloc( N * sizeof( double ) );

  for (int i = 0; i < N; i++) {
    vecCopy[i] = vec[i];
  }

  root = new packnode<double>( vecCopy, N, packnode<double>::OriginalData );
  root->mark( true );
  //
  // The first level uses the standard wavelet calculation, so
  // reverse = false
  //              freqCalc, reverse
  newLevel( root, true,     false );
} // packfreq



/**
  Traverse the tree, from left to right, and add
  a node at <i>level</i> to the level basis matrix.
  
  Levels are numbered from 0 at the root.
 */
void packfreq::findLevel( packnode<double>* top, 
			  size_t cur_level,
			  const size_t level )
{
  if (top != 0) {
    if (cur_level == level) {
      mat.append(top);
    }
    else {
      findLevel( top->lhsChild(), cur_level+1, level );
      findLevel( top->rhsChild(), cur_level+1, level );
    }
  }
} // findLevel



/**
  Build a level basis matrix

  The "level basis" is a horizontal slice through the wavelet packet
  tree.  Here the levels are numbered from zero at root (which
  contains the original data).  The most useful basis for frequency
  analysis is a square matrix.  If the original data set consists of
  1024 elements, level 5 of the tree consists of 32 tree nodes
  containing 32 elements each.

<pre>
     <b>number of elements</b>
     <b>in a tree node</b>    <b>level</b> <b>number of nodes</b>
           1024                  0                 1
            512                  1                 2
            256                  2                 4
            128                  3                 8
             64                  4                16
             32                  5                32
</pre>

  The level basis is built from left to right, in the horizontal slice
  through the tree.  The left most node contains the lowest frequency
  band.

 */
void packfreq::getLevel( const size_t level )
{
  findLevel( root, 0, level );
} // getLevel



/**

  Print out the level basis matrix so that it can be plotted as a
  three dimensional surface.

  The level basis for a given level is constructed by the getLevel()
  function.

  In the discussion of this function below, we will consider the
  case where the modified wavelet packet tree was built from a
  data set of 1024 elements and the level basis was at level
  5, resulting in a matrix of 32 nodes, each of which has
  32 values.

  mat[0] is the left most node (and lowest frequency band), mat[31] is
  the right most (and highest frequency band).  The values
  (*mat[0])[0] ... (*mat[0])[31] represent frequency values at
  particular time intervals.

  If we use two variables, x and y, to index the matrix then
  (*mat[y])[x] defines a time/frequency plane, where the x-index
  is time and the y-index is frequency.  The value at (*mat[y])[x]
  is the magnitude a particular time/frequency point.

  In plotting a surface from this data, time or frequency are plotted
  on the x or y axis.  The magnitude at (*mat[y])[x] is plotted on the
  Z-axis.  Following <i>Ripples in Mathematics</i> by Jensen and
  la-Cour Harbo (Springer Verlag, 2001), the calculation for the
  z-axis value is:

<pre>
      m = (*mat)[y][x]
      z_val = log( 1 + val^2 )  
</pre>

  Where <i>log</i> is the natural log.

  This function has been used to generate data for gnuPlot surface
  plots.

  Whether frequency is plotted on the x-axis or the y-axis depends
  on the nature of the data.  If a signal line sin(x) is analyzed,
  the frequency is constant.  To avoid the ridge produced by the
  sin(x) frequency obscuring the surface, it is better to plot
  time on the y-axis and frequency on the x-axis.

  In the case of a constantly changing frequency (like the linear
  chirp), time is plotted on the x-axis and frequency is plotted
  on the y-axis.

  Another wrinkle involves the values plotted on the x and y axis.
  The simplest plot of time (say on the x-axis) would consist of the
  32 time regions (e.g., (*mat[y][x])).  Similarly, frequency would
  consist of 32 frequency regions (on the y-axis).  This would
  yield a surface numbered from 0..31 on both the x and y axis.

  The axis labels can be scaled to represent the frequency and
  time ranges.

  This function was used to generate different plots, so various parts
  are commented out, making it a bit of a hack.

 */
void packfreq::plotMat(const size_t N)
{
  size_t num_y = mat.length();
  const double incr = (double)N / (double)num_y;
  if (num_y > 0) {
    size_t num_x = mat[0]->length();

    double freq_start = 0.0;
    for (size_t y = 0; y < num_y; y++) {
      double time_start = 0.0;
      for (size_t x = 0; x < num_x; x++) {
	double val = (*mat[y])[ x ];
	// plot time on the x, frequency on y
	// printf(" %d  %d  %7.4f\n", x, y, log(1+(val*val)) );

	// plot frequency on x, time on y
	printf(" %d  %d  %7.4f\n", y, x, log(1+(val*val)) );

	// plot actual frequency and time values,
	// with frequency on x and time on y
	// printf(" %7.4f  %7.4f  %7.4f\n", 
	//        freq_start, time_start, log(1+(val*val)) );

	time_start = time_start + incr;
      }
      freq_start = freq_start + incr;
      printf("\n");
    }
  }
} // plotMat



/**
  Print the contents of the level basis matrix
 */
void packfreq::prMat()
{
  size_t num_y = mat.length();
  if (num_y > 0) {
    size_t num_x = mat[0]->length();
    for (size_t y = num_y-1; y >= 0; y--) {
      for (size_t x = 0; x < num_x; x++) {
	printf(" %7.4f ", (*mat[y])[ x ] );
      }
      printf("\n");
      fflush(stdout);
    }
  }
} // prMat


