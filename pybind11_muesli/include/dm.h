#pragma once

#include <type_traits>
#include "muesli.h"
#include "detail/exception.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>
#include <openacc.h>

namespace py = pybind11;

namespace msl {

/**
 * \brief Class DM represents a distributed matrix.
 *
 * A distributed matrix represents a one-dimensional parallel container and is
 * distributed among all MPI processes the application was started with. It
 * includes the data parallel skeleton map as well as variants of it.
 *
 * \tparam T Element type. Restricted to classes without pointer data members.
 */
template <typename T>
class DM
{
public:

    //
    // CONSTRUCTORS / DESTRUCTOR
    //

    /**
    * \brief Default constructor.
    */
    DM();

    /**
    * \brief Creates an empty distributed matrix.
    *
    * @param col amount of columns of the distributed matrix.
    * @param row amount of rows of the distributed matrix.
    */
    DM(int col, int row);

    /**
    * \brief Creates a distributed matrix and fills it with
    *           \em initial_value.
    *
    * @param col amount of columns of the distributed matrix.
    * @param row amount of rows of the distributed matrix.
    * @param initial_value Initial value for all elements.
    */
    DM(int col, int row, const T& initial_value);

    /**
    * \brief Destructor.
    */
    ~DM();

    /**
    * \brief Initializes the elements of the distributed matrix with the value \em
    *        value.
    *
    * @param value The value.
    */
    void fill(const T& value);


    //
    // SKELETONS / COMPUTATION
    //

    // SKELETONS / COMPUTATION / MAP

    /**
    * \brief Replaces each element a[i] of the distributed matrix with f(a[i]).
    *
    * @param f Python function.
    */
    void mapInPlace(const std::function<T(T)> &f);

    /**
    * \brief Replaces each element a[i] of the distributed matrix with f(i, a[i]).
    *        Note that besides the element itself also its index is passed to the
    *        function.
    *
    * @param f Python function.
    */
    void mapIndexInPlace(const std::function<T(int,T)> &f);

    /**
    * \brief Replaces each element a[i] of the distributed matrix with f(row, column, a[i]).
    *        Note that besides the element itself also its row and column is passed to the
    *        function.
    *
    * @param f Python function.
    */
    void mapIndexInPlace2(const std::function<T(int,int,T)> &f);

    //TODO: Add Description
    void mapIndexInPlaceM(const std::function<T(int,int,T)> &f);

    /**
    * \brief Returns a new distributed matrix with a_new[i] = f(a[i]).
    *
    * @param f Python function.
    * @return The newly created distributed matrix.
    */
    DM<T> map(const std::function<T(T)> &f);

    /**
    * \brief Returns a new distributed matrix with a_new[i] = f(i, a[i]). Note
    *        that besides the element itself also its index is passed to the function.
    *
    * @param f Python Function.
    * @return The newly created distributed matrix.
    */
    DM<T> mapIndex(const std::function<T(int,T)> &f);

    /**
    * \brief Returns a new distributed matrix with a_new[i] = f(row, column, a[i]). Note
    *        that besides the element itself also its row and column is passed to the function.
    *
    * @param f Python Function.
    * @return The newly created distributed matrix.
    */
    DM<T> mapIndex2(const std::function<T(int,int,T)> &f);


    // SKELETONS / COMMUNICATION / GATHER

    /**
     * \brief Transforms a distributed matrix to a numpy array.
     *
     * @return Numpy Array.
     */
    py::array_t<T> gather();


    //
    // GETTERS AND SETTERS
    //

    /**
    * \brief Returns the amount of rows in the DM.
    *
    * @return Amount of rows.
    */
    int getRows();

    /**
    * \brief Returns the amount of columns in the DM.
    *
    * @return Amount of columns.
    */
    int getCols();

    /**
    * \brief Returns the local partition.
    *
    * @return The local partition.
    */
    T* getLocalPartition();

    /**
    * \briefs Sets the local partition.
    *
    * @param array for the local partition.
    */
    void setLocalPartition(py::array_t<T> array);

    /**
    * \briefs Sets the local partition.
    *
    * @param array for the local partition.
    */
    void setMatrix(py::array_t<T> array);

    /**
    * \brief Returns the element at the given global index \em index.
    *
    * @param index The global index.
    * @return The element at the given global index.
    */
    T get(int index) const;

    /**
    * \brief Sets the element at the given global index \em globalIndex to the
    *        given value \em v, with 0 <= globalIndex < size.
    *
    * @param globalIndex The global index.
    * @param v The new value.
    */
    void set(int globalIndex, const T& v);

    /**
    * \brief Returns the global size of the distributed matrix.
    *
    * @return The global size.
    */
    int getSize() const;

    /**
    * \brief Returns the size of local partitions of the distributed matrix.
    *
    * @return The size of local partitions.
    */
    int getLocalSize() const;

    /**
    * \brief Returns the first (global) index of the local partition.
    *
    * @return The first (global) index.
    */
    int getFirstIndex() const;

    /**
    * \brief Checks whether the element at the given global index \em index is
    *        locally stored.
    *
    * @param index The global index.
    * @return True if the element is locally stored.
    */
    bool isLocal(int index) const;

    /**
    * \brief Returns the element at the given local index \em index. Note that
    *        0 <= \em index < getLocalSize() must hold (will not be checked, for
    *        reasons of performance).
    *
    * @param index The local index.
    */
    T getLocal(int localIndex);

    /**
    * \brief Sets the element at the given local index \em localIndex to the
    *        given value \em v.
    *
    * @param localIndex The local index.
    * @param v The new value.
    */
    void setLocal(int localIndex, const T& v);

    /**
    * \brief Prints the local partition of the root processor of the distributed matrix to standard output.
    *
    */
    void showLocal();

    /**
    * \brief Prints the distributed matrix to standard output.
    *
    */
    void show();

    /**
    * \brief Each process prints its local partition of the distributed matrix.
    */
    void printLocal();

private:
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
    // Number of local elements
    int nLocal;
    // First (global) index of local partition
    int firstIndex;
    // First (global) row in local partition
    int firstRow;
    // Total number of MPI processes
    int np;
    // number of elements on CPU
    int nCPU;

    //
    // AUXILIARY
    //

    // initializes distributed matrix (used in constructors).
    void init();
};
}

//
// BINDING FUNCTION
//

void bind_dm(py::module& m);
