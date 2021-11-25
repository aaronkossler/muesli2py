/* File: example.c */

#include "dm.h"
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
//#include <mpi.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>
#include <pybind11/functional.h>

namespace py = pybind11;

template<typename T>
DM<T>::DM():                  // distributed array (resides on GPUs until deleted!)
      n(0),                        // number of elements of distributed matrix
      ncol(0),                     // number of columns of distributed matrix
      nrow(0)                    // number of rows of distributed matrix
{}

// constructor creates a non-initialized DM
template<typename T>
DM<T>::DM(int row, int col)
    : ncol(col), nrow(row), n(col*row){
    elements = new int[n];
}

template<typename T>
DM<T>::DM(int row, int col, const T& v)
    : ncol(col), nrow(row), n(col*row){
    elements = new int[n];
    fill(v);
}

template<typename T>
DM<T>::~DM() {
  printf("Destructor\n");
}

template<typename T>
int DM<T>::getSize() const {
  return n;
}

template<typename T>
void DM<T>::fill(const T& value) {
    for (int i = 0; i < n; i++) {
        elements[i] = value;
    }
}

template<typename T>
void DM<T>::printmatrix() {
    for (int i = 0; i< nrow; i++) {
    printf("|");
        for (int i = 0; i< ncol; i++) {
            printf("%d;", elements[i]);
        }
    printf("|\n");
    }
}

template<typename T>
void DM<T>::map(const std::function<T(T)> &f) {
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i]);
    }
}

PYBIND11_MODULE(dm, dm_handle) {
  dm_handle.doc() = "I'm a docstring hehe";
  py::class_<DM<int>>(
			dm_handle, "Pydm"
			)
    .def(py::init())
	.def(py::init<int, int>())
	.def(py::init<int, int, int>())
	.def("fill", &DM<int>::fill)
	.def("printmatrix", &DM<int>::printmatrix)
	.def("map", &DM<int>::map)
    ;
}
