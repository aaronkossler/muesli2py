/* File: da.h */
#include <pybind11/functional.h>

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
  void printarray();
  void map(const std::function<T(T)> &f);

  // Attributes
  // Number of elements
  int n;
  // Pointer to array of elements
  int* elements;
};
