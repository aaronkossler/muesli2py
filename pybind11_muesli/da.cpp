#include "da.h"
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
//#include <mpi.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
// #include <pybind11/numpy.h>
#include <pybind11/functional.h>

namespace py = pybind11;

template<typename T>
DA<T>::DA():
      n(0)
{}

template<typename T>
DA<T>::DA(int size){
    n = size;
    elements = new int[n];
}

template<typename T>
DA<T>::DA(int size, const T& v){
    n = size;
    elements = new int[n];
    fill(v);
}

template<typename T>
DA<T>::~DA(){
  printf("Destructor\n");
}

template<typename T>
int DA<T>::getSize() {
    return n;
}

template<typename T>
void DA<T>::fill(const T& value) {
    for (int i = 0; i < n; i++) elements[i] = value;
}

template<typename T>
void DA<T>::printarray() {
    printf("[");
    for (int i = 0; i< n; i++) printf("%d;", elements[i]);
    printf("]\n");
}

template<typename T>
void DA<T>::map(const std::function<T(T)> &f) {
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i]);
    }
}

PYBIND11_MODULE(da, da_handle) {
  da_handle.doc() = "I'm a docstring hehe";
  py::class_<DA<int>>(
			da_handle, "Pyda"
			)
	.def(py::init())
	.def(py::init<int>())
	.def(py::init<int, int>())
	.def("getSize", &DA<int>::getSize)
	.def("fill", &DA<int>::fill)
	.def("printarray", &DA<int>::printarray)
	.def("map", &DA<int>::map)
    ;
}
