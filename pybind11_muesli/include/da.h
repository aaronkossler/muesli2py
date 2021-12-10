/* File: da.h */
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace msl {

template <typename T>
class DA
{
public:

  // Constructors
  DA();
  DA(int size);
  DA(int size, const T& initial_value);

  // Destructor
  ~DA();

  // Functions
  int getSize();
  void fill(const T& value);
  void setElements(py::array_t<T> array);
  void printarray();
  void mapInPlace(const std::function<T(T)> &f);
  void mapIndexInPlace(const std::function<T(T,T)> &f, int index);
  DA<T> map(const std::function<T(T)> &f);
  DA<T> mapIndex(const std::function<T(T,T)> &f, int index);


  // Attributes
  // Number of elements
  int n;
  // Pointer to array of elements
  int* elements;
};
} //namespace msl

//
// BINDING FUNCTION
//

void bind_da(py::module& m);
