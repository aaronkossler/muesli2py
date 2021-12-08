/* File: example.c */

#include "../include/muesli.h"
#include "../include/dm.h"
#include <stdexcept>
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
#include <mpi.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

using namespace std;

namespace py = pybind11;

template<typename T>
msl::DM<T>::DM():                       // distributed array (resides on GPUs until deleted!)
      n(0),                        // number of elements of distributed matrix
      ncol(0),                     // number of columns of distributed matrix
      nrow(0),                     // number of rows of distributed matrix
      nLocal(0),                   // number of local elements on a node
      np(0),                       // number of (MPI-) nodes (= Muesli::num_local_procs)
      id(0),                       // id of local node among all nodes (= Muesli::proc_id)
      localPartition(0),           // local partition of the DM
      firstIndex(0),               // first global index of the DM in the local partition
      firstRow(0)                  // first global row index of the DM on the local partition
{}

// constructor creates a non-initialized DM
template<typename T>
msl::DM<T>::DM(int row, int col)
    : ncol(col), nrow(row), n(col*row){
    init();
    elements = new T[n];
}

template<typename T>
msl::DM<T>::DM(int row, int col, const T& v)
    : ncol(col), nrow(row), n(col*row){
    init();
    elements = new T[n];
    fill(v);
}

// auxiliary method init()
template<typename T>
void msl::DM<T>::init() {
//  if (Muesli::proc_entrance == UNDEFINED) {
//    throws(detail::MissingInitializationException());
//  }
//  TODO: ImportError when accessing Muesli static variables.
//  id = Muesli::proc_id;
//  np = Muesli::num_total_procs;
//  n = ncol * nrow;
//  nLocal = n / np;
  nCPU = nLocal;
  firstIndex =  id * nLocal;
//  printf("loc processes %d , First index %d\n", Muesli::num_local_procs, firstIndex);
//  printf("Building datastructure with %d nodes and %d cpus\n", msl::Muesli::num_total_procs,
//         msl::Muesli::num_local_procs);
}

template<typename T>
msl::DM<T>::~DM() {
  printf("Destructor\n");
}

template<typename T>
int msl::DM<T>::getSize() const {
  return n;
}

template<typename T>
void msl::DM<T>::fill(const T& value) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = value;
    }
}

template<typename T>
void msl::DM<T>::setElements(py::array_t<T> array) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = *array.data(i);
    }
}

template<typename T>
void msl::DM<T>::printmatrix() {
    #pragma omp parallel for
    for (int i = 0; i< nrow; i++) {
    printf("|");
        #pragma omp parallel for
        for (int j = 0; j< ncol; j++) {
            printf("%d;", elements[nrow*i+j]);
        }
    printf("|\n");
    }
}

template<typename T>
void msl::DM<T>::mapInPlace(const std::function<T(T)> &f) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i]);
    }
}

template<typename T>
void msl::DM<T>::mapIndexInPlace(const std::function<T(T,T)> &f, int index) {
    T ref = elements[index];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i], ref);
    }
}

template<typename T>
void msl::DM<T>::mapIndexInPlace(const std::function<T(T,T)> &f, int row, int col) {
    T ref = elements[row*ncol+col];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i], ref);
    }
}

template<typename T>
msl::DM<T> msl::DM<T>::map(const std::function<T(T)> &f) {
    DM<T> result(ncol, nrow);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        result.elements[i] = f(elements[i]);
    }

    return result;
}

template<typename T>
msl::DM<T> msl::DM<T>::mapIndex(const std::function<T(T,T)> &f, int index) {
    DM<T> result(ncol, nrow);
    T ref = elements[index];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        result.elements[i] = f(elements[i], ref);
    }

    return result;
}

template<typename T>
msl::DM<T> msl::DM<T>::mapIndex(const std::function<T(T,T)> &f, int row, int col) {
    DM<T> result(ncol, nrow);
    T ref = elements[row*ncol+col];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        result.elements[i] = f(elements[i], ref);
    }

    return result;
}

PYBIND11_MODULE(dm, dm_handle) {
  dm_handle.doc() = "I'm a docstring hehe";
  py::class_<msl::DM<int>>(
			dm_handle, "Pydm"
			)
    .def(py::init())
	.def(py::init<int, int>())
	.def(py::init<int, int, int>())
	.def("init", &msl::DM<int>::init)
	.def("fill", &msl::DM<int>::fill)
	.def("setElements", &msl::DM<int>::setElements)
	.def("printmatrix", &msl::DM<int>::printmatrix)
	.def("mapInPlace", &msl::DM<int>::mapInPlace)
	.def("mapIndexInPlace", py::overload_cast<const std::function<int(int,int)> &, int>(&msl::DM<int>::mapIndexInPlace))
	.def("mapIndexInPlace", py::overload_cast<const std::function<int(int,int)> &, int, int>(&msl::DM<int>::mapIndexInPlace))
	.def("map", &msl::DM<int>::map)
	.def("mapIndex", py::overload_cast<const std::function<int(int,int)> &, int>(&msl::DM<int>::mapIndex))
	.def("mapIndex", py::overload_cast<const std::function<int(int,int)> &, int, int>(&msl::DM<int>::mapIndex))
    ;
}
