/*
 * da.cpp
 *
 *      Author: Herbert Kuchen <kuchen@uni-muenster.de>,
 *              Steffen Ernsting <s.ernsting@uni-muenster.de>,
 *              Nina Herrmann <nina.herrmann@uni-muenster.de>
 *
 * ---------------------------------------------------------------------------------------------------
 *
 * The MIT License
 *
 * Copyright 2014-2020 Steffen Ernsting <s.ernsting@uni-muenster.de>,
 *                     Herbert Kuchen <kuchen@uni-muenster.de,
 *                     Nina Herrmann <nina.herrmann@uni-muenster.de>.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "../include/muesli.h"
#include "../include/da.h"
#include "../include/pixel.h"
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

using namespace std;

namespace py = pybind11;

template<typename T>
msl::DA<T>::DA():                       // distributed array (resides on GPUs until deleted!)
        n(0),                        // number of elements of distributed matrix
        nLocal(0),                   // number of local elements on a node
        np(0),                       // number of (MPI-) nodes (= Muesli::num_local_procs)
        id(0),                       // id of local node among all nodes (= Muesli::proc_id)
        localPartition(0),           // local partition of the DA
        firstIndex(0),               // first global index of the DA in the local partition
        firstRow(0)                  // first global row index of the DA on the local partition
{}

// constructor creates a non-initialized DA
template<typename T>
msl::DA<T>::DA(int n)
        : n(n){
    init();
}

// constructor creates a DA, initialized with v
template<typename T>
msl::DA<T>::DA(int n, const T& v)
        : n(n){
    init();
    fill(v);
}

// auxiliary method init()
template<typename T>
void msl::DA<T>::init() {
    if (Muesli::proc_entrance == UNDEFINED) {
        throws(detail::MissingInitializationException());
    }
    id = Muesli::proc_id;
    np = Muesli::num_total_procs;
    nLocal = n / np;
    nCPU = nLocal;
    firstIndex =  id * nLocal;
    localPartition = new T[nLocal];
    // printf("loc processes %d , First index %d\n", Muesli::num_local_procs, firstIndex);
    // printf("Building datastructure with %d nodes and %d cpus\n", msl::Muesli::num_total_procs,
    //        msl::Muesli::num_local_procs);
}

// destructor removes a DA
template<typename T>
msl::DA<T>::~DA() {
    //printf("Destructor\n");
    //delete[] localPartition;
}

template<typename T>
void msl::DA<T>::fill(const T& value) {
#pragma acc parallel loop
    for (int k = 0; k < nLocal; k++) {
        localPartition[k] = value;
    }
}

// **************************** auxiliary methods ****************************

template<typename T>
T* msl::DA<T>::getLocalPartition() {
    return localPartition;
}

template<typename T>
void msl::DA<T>::setLocalPartition(py::array_t<T> array) {
#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        localPartition[k] = *array.data(k);
    }
}

template<typename T>
void msl::DA<T>::setArray(py::array_t<T> array) {
#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        localPartition[k] = *array.data(firstIndex+k);
    }
}

template<typename T>
T msl::DA<T>::get(int index) const {
    int idSource;
    T message;
    // TODO: adjust to new structure
    // element with global index is locally stored
    if (isLocal(index)) {
        message = localPartition[index - firstIndex];
        idSource = Muesli::proc_id;
    }
        // Element with global index is not locally stored
    else {
        // Calculate id of the process that stores the element locally
        idSource = (int) (index / nLocal);
    }

    msl::MSL_Broadcast(idSource, &message, 1);
    return message;
}

template<typename T>
int msl::DA<T>::getSize() const {
    return n;
}

template<typename T>
int msl::DA<T>::getLocalSize() const {
    return nLocal;
}

template<typename T>
int msl::DA<T>::getFirstIndex() const {
    return firstIndex;
}

template<typename T>
bool msl::DA<T>::isLocal(int index) const {
    return (index >= firstIndex) && (index < firstIndex + nLocal);
}

template<typename T>
T msl::DA<T>::getLocal(int localIndex) {
    if (localIndex >= nLocal)
        throws(detail::NonLocalAccessException());
    return localPartition[localIndex];
}

template<typename T>
void msl::DA<T>::setLocal(int localIndex, const T& v) {
    if (localIndex < nCPU) {
        localPartition[localIndex] = v;
    } else if (localIndex >= nLocal)
        throws(detail::NonLocalAccessException());
}

template<typename T>
void msl::DA<T>::set(int globalIndex, const T& v) {
    if ((globalIndex >= firstIndex) && (globalIndex < firstIndex + nLocal)) {
        setLocal(globalIndex - firstIndex, v);
    }
}

// method (only) useful for debugging
template<typename T>
//void msl::DA<T>::showLocal(const std::string& descr) {
void msl::DA<T>::showLocal() {
    if (msl::isRootProcess()) {
        std::ostringstream s;
//    if (descr.size() > 0)
//    s << descr << ": ";
        s << "[";
        for (int i = 0; i < nLocal; i++) {
            s << localPartition[i] << " ";
        }
        s << "]" << std::endl;
        printf("%s", s.str().c_str());
    }
}

template<typename T>
void msl::DA<T>::show() {
    T* b = new T[n];
    std::ostringstream s;
    msl::allgather(localPartition, b, nLocal);

    if (msl::isRootProcess()) {
        s << "[";
        for (int i = 0; i < n - 1; i++) {
            s << b[i];
            s << " ";
        }
        s << b[n - 1] << "]" << std::endl;
        s << std::endl;
    }

    delete b;

    if (msl::isRootProcess()) printf("%s", s.str().c_str());
}

// SKELETONS / COMMUNICATION / GATHER

template<typename T>
py::array_t<T> msl::DA<T>::gather() {
    T* array = new T[n];
    msl::allgather(localPartition, array, nLocal);

    // Create a Python object that will free the allocated
    // memory when destroyed:
    py::capsule free_when_done(array, [](void *f) {
        T *array = reinterpret_cast<T *>(f);
        delete[] array;
    });

    return py::array_t<T>(
            {n,}, // shape
            array, // the data pointer
            free_when_done); // numpy array references this parent
}

//*********************************** Maps ***********************************
template<typename T>
void msl::DA<T>::mapInPlace(const std::function<T(T)> &f) {
#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        setLocal(k, f(localPartition[k]));
    }
}

template<typename T>
void msl::DA<T>::mapIndexInPlace(const std::function<T(int,T)> &f) {

    // all necessary calculations are performed otherwise some are skipped.
#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        int globalindex = k + firstIndex;
        localPartition[k] = f(globalindex, localPartition[k]);
    }
}

template<typename T>
msl::DA<T> msl::DA<T>::map(const std::function<T(T)> &f) {
    DA<T> result(n);

#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++){
        result.setLocal(k, f(localPartition[k]));
    }

    return result;
}

template<typename T>
msl::DA<T> msl::DA<T>::mapIndex(const std::function<T(int,T)> &f) {
    DA<T> result(n);

#pragma acc parallel loop
    for (int k = 0; k < nCPU; k++){
        int globalindex = k + firstIndex;
        result.setLocal(k, f(globalindex, localPartition[k]));
    }

    return result;
}

void bind_da(py::module& m) {
    py::class_<msl::DA<int>>(m, "intDA")
            .def(py::init())
            .def(py::init<int>())
            .def(py::init<int, int>())
            .def("fill", &msl::DA<int>::fill)
            .def("mapInPlace", &msl::DA<int>::mapInPlace)
            .def("mapIndexInPlace", &msl::DA<int>::mapIndexInPlace)
            .def("map", &msl::DA<int>::map)
            .def("mapIndex", &msl::DA<int>::mapIndex)
            .def("getLocalPartition", &msl::DA<int>::getLocalPartition)
            .def("setLocalPartition", &msl::DA<int>::setLocalPartition)
            .def("setArray", &msl::DA<int>::setArray)
            .def("get", &msl::DA<int>::get)
            .def("set", &msl::DA<int>::set)
            .def("showLocal", &msl::DA<int>::showLocal)
            .def("show", &msl::DA<int>::show)
            .def("getSize", &msl::DA<int>::getSize)
            .def("getLocalSize", &msl::DA<int>::getLocalSize)
            .def("getFirstIndex", &msl::DA<int>::getFirstIndex)
            .def("isLocal", &msl::DA<int>::isLocal)
            .def("getLocal", &msl::DA<int>::getLocal)
            .def("setLocal", &msl::DA<int>::setLocal)
            .def("gather", &msl::DA<int>::gather)
            ;
    py::class_<msl::DA<float>>(m, "floatDA")
            .def(py::init<int, float>())
            .def("get", &msl::DA<float>::get)
            .def("mapIndexInPlace", &msl::DA<float>::mapIndexInPlace)
            ;
}
