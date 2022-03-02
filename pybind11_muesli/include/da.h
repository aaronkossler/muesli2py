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
 * \brief Class DA represents a distributed array.
 *
 * A distributed array represents a one-dimensional parallel container and is
 * distributed among all MPI processes the application was started with. It
 * includes the data parallel skeleton map as well as variants of it.
 *
 * \tparam T Element type. Restricted to classes without pointer data members.
 */
    template <typename T>
    class DA
    {
    public:

        //
        // CONSTRUCTORS / DESTRUCTOR
        //

        /**
        * \brief Default constructor.
        */
        DA();

        /**
        * \brief Creates an empty distributed matrix.
        *
        * @param size Size of the distributed array.
        * @param d Distribution of the distributed array.
        */
        DA(int n);

        /**
        * \brief Creates a distributed matrix with \em size elements equal to
        *        \em initial_value.
        *
        * @param size Size of the distributed array.
        * @param initial_value Initial value for all elements.
        */
        DA(int n, const T& initial_value);

        /**
        * \brief Destructor.
        */
        ~DA();

        /**
        * \brief Initializes the elements of the distributed array with the value \em
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
        * \brief Replaces each element a[i] of the distributed array with f(a[i]).
        *
        * @param f Python function.
        */
        void mapInPlace(const std::function<T(T)> &f);

        /**
        * \brief Replaces each element a[i] of the distributed array with f(i, a[i]).
        *        Note that besides the element itself also its index is passed to the
        *        function.
        *
        * @param f Python function.
        */
        void mapIndexInPlace(const std::function<T(int,T)> &f);

        /**
        * \brief Returns a new distributed array with a_new[i] = f(a[i]).
        *
        * @param f Python function.
        * @return The newly created distributed array.
        */
        DA<T> map(const std::function<T(T)> &f);

        /**
        * \brief Returns a new distributed array with a_new[i] = f(i, a[i]). Note
        *        that besides the element itself also its index is passed to the function.
        *
        * @param f Python Function.
        * @return The newly created distributed array.
        */
        DA<T> mapIndex(const std::function<T(int,T)> &f);


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
        * \brief Returns the local partition.
        *
        * @return The local partition.
        */
        T* getLocalPartition();

        /**
        * \briefs Sets the local partition.
        *
        * @param array Numpy Array.
        */
        void setLocalPartition(py::array_t<T> array);

        /**
        * \briefs Sets the Distributed Array.
        *
        * @param array Numpy Array.
        */
        void setArray(py::array_t<T> array);

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

void bind_da(py::module& m);
