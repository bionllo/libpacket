

#include "queue.h"
#include "packnode.h"
#include "packcontainer_int.h"
#include "packtree_base_int.h"


/**

  Add a new level to the wavelet packet tree.  Wavelet packet trees
  are data structures that support a variety of applications.  

  If the reverse argument is true, the locations of the high pass and
  low pass filter results in the wavelet calculation will be
  reversed.  This is used in building a wavelet packet tree for
  frequency analysis.

  The <i>top</i> packnode object contains data from the previous
  level.  If this is the first level in the tree, <i>top</i> will
  contain the input data set.

  A packcontainer_int object is created with the <i>top</i> data.  The
  packcontainer_int constructor will allocate new storage and copy the
  data into this storage.  If the packcontainer_int object consists of N
  elements, then there will be N/2 elements on the left and N/2 or the
  right.  The object allows the lhs and rhs data to be accessed as one
  array.

  A wavelet transform step is calculated on the N element data set.
  This results in N/2 values from the wavelet scaling function (low
  pass function) and N/2 values from the wavelet function (high pass
  function).  These values are used to create two new packnode objects
  which become children of <i>top</i>.  Sub-trees for the new packnode
  objects are recursively calculated.

  As the tree is constructed, the leaves of the tree are marked with a
  boolean flag in preparation for calculating the "best basis"
  representation for the data.  See the algorithm outlined in section
  8.2.2 of "Ripples in Mathematics" by Jensen and la Cour-Harbo

  The wavelet packet tree form that is used for wavelet packet
  frequency analysis is described in section 9.3 (figure 9.14)
  ofg "Ripples in Mathematics".  

 */
void packtree_base_int::newLevel( packnode<int>* top, 
			      bool freqCalc, 
			      bool reverse )
{
  if (top != 0) {
    const size_t len = top->length();
    if (len > 1) {
      // Create a new wavelet packet container for use in
      // calculating the wavelet transform.  Note that the
      // container is only used locally.
      packcontainer_int container( top );

      if (reverse) {
	// Calculate the reverse foward wavelet transform step, 
	// where the high pass result is stored in the upper half
	// of the container and the low pass result is stored
	// in the lower half of the container.
	waveObj->forwardStepRev( container, len );
      }
      else {
	// Calculate the foward wavelet transform step, where
	// the high pass result is stored in the upper half
	// of the container and the low pass result is stored
	// in the lower half of the container.
	waveObj->forwardStep( container, len );
      }

      packnode<int> *lhs = new packnode<int>(container.lhsData(), 
						   len/2,
						   packnode<int>::LowPass );
      packnode<int> *rhs = new packnode<int>(container.rhsData(), 
						   len/2,
						   packnode<int>::HighPass );

      // set the "mark" in the top node to false and
      // mark the two children to true.
      top->mark( false );
      lhs->mark( true );
      rhs->mark( true );

      top->lhsChild( lhs );
      top->rhsChild( rhs );

      // The transform on the left hand side always uses
      // the standard order (e.g., low pass filter result
      // goes in the lower half, high pass goes in the
      // upper half of the container).
      newLevel( lhs, freqCalc, false );

      if (freqCalc) {
	// wavelet packet frequency analysis reverses the
	// storage locations for the filter results in the
	// right hand child
	newLevel( rhs, freqCalc, true );
      }
      else { // freq == false
	// use standard filter location
	newLevel( rhs, freqCalc, false );
      }
    }
  }
} // newLevel



/**
  Print the wavelet packet tree, breadth first (this is also
  sometimes called a level traversal).

  The breadth first traversal uses a queue.  The root of the tree is
  initially inserted into the queue.  The function operates by
  deleting the node at teh front of the queue, printing the data
  associated with that node and adding the node's left and right
  children to the queue.  Since a node's children are at the next
  lower level, and we add the left child before the right child,
  the function prints a tree level from left to right.

  The above paragraph paraphrases Chapter 5, Level Order Traversal
  of <i>Fundamentals of Data Structures in C</i> by Horowitz, Sahni
  and Anderson-Freed, 1993.

 */
void packtree_base_int::breadthFirstPrint(const printKind kind)
{
  queue<int> Q;

  Q.addQueue( root, 0 );
  while (! Q.queueEmpty() ) {
    packnode<int> *node = Q.queueStart()->node;
    size_t indent = Q.queueStart()->indent;
    Q.deleteStart();
    
    if (indent > 0) {
      // print 'indent' spaces
      printf("%*c", indent, ' ');
    }

    switch (kind) {
    case printData: node->pr();
      break;
    case printCost: node->prCost();
      break;
    case printBestBasis: node->prBestBasis();
      break;
    default:
      assert( false );
      break;
    } // switch
    
    packnode<int> *lhs = node->lhsChild();
    packnode<int> *rhs = node->rhsChild();

    if (lhs != 0) {
      Q.addQueue( lhs, indent + 2 );
    }
    if (rhs != 0) {
      Q.addQueue( rhs, indent + 2 );
    }
  }
} // packtree_base_int::breadthFirstPrint




/**
  Print the wavelet packet tree data and wavelet transform
  result to standard out.
 */
void packtree_base_int::pr()
{
  if (root != 0) {
    breadthFirstPrint(printData);
  }
} // pr
