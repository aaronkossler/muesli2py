#include <type_traits>
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace msl {

template <typename T>
class DM
{
public:

    //
    // Constructors
    //

    DM();
    DM(int col, int row);
    DM(int col, int row, const T& initial_value);

    // Destructor
    ~DM();

    //
    // Functions
    //

    int getSize() const;
    void fill(const T& value);
    void setElements(py::array_t<T> array);
    void printmatrix();
    void mapInPlace(const std::function<T(T)> &f);
    void mapIndexInPlace(const std::function<T(T,T)> &f, int index);
    void mapIndexInPlace(const std::function<T(T,T)> &f, int row, int col);
    DM<T> map(const std::function<T(T)> &f);
    DM<T> mapIndex(const std::function<T(T,T)> &f, int index);
    DM<T> mapIndex(const std::function<T(T,T)> &f, int row, int col);

    //
    // Attributes
    //

    // local partition
    T* localPartition;
    // position of processor in data parallel group of processors; zero-base
    int id;
    // Number of elements
    int n;
    // Number of cols
    int ncol;
    // Number of rows
    int nrow;
    // Pointer to array of elements
    int* elements;
    // Number of local elements
    int nLocal;
    // First (global) index of local partition
    int firstIndex;
    // First (global) row in local partition
    int firstRow;
    // Total number of MPI processes
    int np;

    //
    // AUXILIARY
    //

    // initializes distributed matrix (used in constructors).
//    void init();
};
}
