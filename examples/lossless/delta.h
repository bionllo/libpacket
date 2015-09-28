
/**
  The delta transform.

  A data set of N elements is replaced by the differences.  In this
  simple algorithm

  <pre>
  s<sub>i</sub> = s<sub>i</sub> - s<sub>i-1</sub>.
  </pre>

  For an array indexed from 0, i = 1 to N-1.  The element at s<sub>0</sub>
  is the reference element, which is unchanged.  The next element
  at s<sub>1</sub> is replaced by the difference between that element
  and s<sub>0</sub>.

  The algorithm implemented here is an in-place algorithm which
  replaces the original data in the forward transform and
  reconstructs the data from in-place in the inverse transform.

  The class is implemented as a template class.  Obviously the
  type used to instantiate the template must support signed
  arithmetic.

  The class was written as a comparision baseline for the wavelet
  compresson algorithms.  The "delta" algorithm is very simple and has
  a time complexity of N, where as the wavelet algorithm is
  N<i>log</i><sub>2</sub>N.  Obviously, if the wavelet algorithm does
  not do better than this simple algorithm for the data set in
  question, the wavelet is a poor choice (or, perhaps, the wavelet
  function in the predict step is poorly chosen for the data set).

*/
template <class T>
class delta_trans
{
public:
  /**
    Convert the value in the array into an initial reference
    value and a set of delta values.
   */
  void forward( T *vec, size_t len )
  {
    if (vec != 0 && len > 0) {
      // reference value
      T next;
      T refVal = vec[0];

      for (size_t i = 1; i < len; i++) {
	next = vec[i];
	vec[i] = vec[i] - refVal;
	refVal = next;
      }
    }
  } // forward

  void inverse( T *vec, size_t len )
  {
    if (vec != 0 && len > 0) {
      for (size_t i = 1; i < len; i++) {
	vec[i] = vec[i] + vec[i-1];
      }
    }
  } // inverse

}; // delta_trans
