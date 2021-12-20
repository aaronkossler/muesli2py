#pragma once

#include <type_traits>
#include "muesli.h"
#include "detail/exception.h"
#include <pybind11/pybind11.h>
#include <pybind11/functional.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

namespace msl {

/**
 * \brief Class DA represents a distributed array.
 *
 * A distributed array represents a one-dimensional parallel container and is
 * distributed among all MPI processes the application was started with. It
 * includes data parallel skeletons such as map, mapStencil, zip, and fold as
 * well as variants of these skeletons.
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
    * @param size Size of the distributed array.
    * @param d Distribution of the distributed array.
    */
    DM(int col, int row);

    /**
    * \brief Creates a distributed matrix with \em size elements equal to
    *        \em initial_value.
    *
    * @param size Size of the distributed array.
    * @param initial_value Initial value for all elements.
    */
    DM(int col, int row, const T& initial_value);

    /**
    * \brief Destructor.
    */
    ~DM();

    /**
    * \brief Initializes the elements of the distributed array with the value \em
    *        value.
    *
    * @param value The value.
    */
    void fill(const T& value);

    //TODO: 4 fills

    //
    // SKELETONS / COMPUTATION
    //

    // SKELETONS / COMPUTATION / MAP

    //TODO: Change Description
    /**
    * \brief Replaces each element a[i] of the distributed array with f(a[i]).
    *
    * @param f The map functor, must be of type \em AMapFunctor.
    * @tparam MapFunctor Functor type.
    */
    void mapInPlace(const std::function<T(T)> &f);

    //TODO: Change Description
    /**
    * \brief Replaces each element a[i] of the distributed array with f(i, a[i]).
    *        Note that besides the element itself also its index is passed to the
    *        functor.
    *
    * @param f The mapIndex functor, must be of type \em AMapIndexFunctor.
    * @tparam MapIndexFunctor Functor type.
    */
    void mapIndexInPlace(const std::function<T(T,T)> &f, int index);

    //TODO: Add Description
    void mapIndexInPlace(const std::function<T(T,T)> &f, int row, int col);

    //TODO: Change Description
    /**
    * \brief Returns a new distributed array with a_new[i] = f(a[i]).
    *
    * @param f The map functor, must be of type \em AMapFunctor.
    * @tparam MapFunctor Functor type.
    * @tparam R Return type.
    * @return The newly created distributed array.
    */
    DM<T> map(const std::function<T(T)> &f);

    //TODO: Change Description
    /**
    * \brief Returns a new distributed array with a_new[i] = f(i, a[i]). Note
    *        that besides the element itself also its index is passed to the functor.
    *
    * @param f The mapIndex functor, must be of type \em AMapIndexFunctor.
    * @tparam MapIndexFunctor Functor type.
    * @tparam R Return type.
    * @return The newly created distributed array.
    */
    DM<T> mapIndex(const std::function<T(T,T)> &f, int index);

    //TODO: Add Description
    DM<T> mapIndex(const std::function<T(T,T)> &f, int row, int col);


    //
    // GETTERS AND SETTERS
    //

    /**
    * \brief Returns the local partition.
    *
    * @return The local partition.
    */
    T* getLocalPartition();

    /**
    * \briefs Sets the local partition.
    *
    * @param elements for the local partition.
    */
//    void setLocalPartition(T* elements);

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
    * \brief Returns the global size of the distributed array.
    *
    * @return The global size.
    */
    int getSize() const;

    /**
    * \brief Returns the size of local partitions of the distributed array.
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
    * \brief Prints the local partion of the root processor of the distributed array to standard output. Optionally, the user
    *        may pass a description that will be printed with the output. Just useful for debugging.
    *
    * @param descr The description string.
    */
//    void showLocal(const std::string& descr);
    void showLocal();

    /**
    * \brief Prints the distributed array to standard output. Optionally, the user
    *        may pass a description that will be printed with the output.
    *
    * @param descr The description string.
    */
//    void show(const std::string& descr = std::string());
    void show();

    /**
    * \brief Each process prints its local partition of the distributed array.
    */
    void printLocal();

//    void printmatrix();

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
//    // Pointer to array of elements
//    int* elements;
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
