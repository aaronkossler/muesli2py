#include "../include/da.h"
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
msl::DA<T>::DA():
      n(0)
{}

template<typename T>
msl::DA<T>::DA(int size){
    n = size;
    elements = new int[n];
}

template<typename T>
msl::DA<T>::DA(int size, const T& v){
    n = size;
    elements = new int[n];
    fill(v);
}

template<typename T>
msl::DA<T>::~DA(){
  printf("Destructor\n");
}

template<typename T>
int msl::DA<T>::getSize() {
    return n;
}

template<typename T>
void msl::DA<T>::fill(const T& value) {
    for (int i = 0; i < n; i++) elements[i] = value;
}

template<typename T>
void msl::DA<T>::setElements(py::array_t<T> array) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = *array.data(i);
    }
}

template<typename T>
void msl::DA<T>::printarray() {
    printf("[");
    for (int i = 0; i< n; i++) printf("%d;", elements[i]);
    printf("]\n");
}

template<typename T>
void msl::DA<T>::mapInPlace(const std::function<T(T)> &f) {
    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i]);
    }
}

template<typename T>
void msl::DA<T>::mapIndexInPlace(const std::function<T(T,T)> &f, int index) {
    T ref = elements[index];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        elements[i] = f(elements[i], ref);
    }
}

template<typename T>
msl::DA<T> msl::DA<T>::map(const std::function<T(T)> &f) {
    DA<T> result(n);

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        result.elements[i] = f(elements[i]);
    }

    return result;
}

template<typename T>
msl::DA<T> msl::DA<T>::mapIndex(const std::function<T(T,T)> &f, int index) {
    DA<T> result(n);
    T ref = elements[index];

    #pragma omp parallel for
    for (int i = 0; i < n; i++) {
        result.elements[i] = f(elements[i], ref);
    }

    return result;
}

void bind_da(py::module& m) {
    py::class_<msl::DA<int>>(
			m, "Pyda"
			)
	.def(py::init())
	.def(py::init<int>())
	.def(py::init<int, int>())
	.def("getSize", &msl::DA<int>::getSize)
	.def("fill", &msl::DA<int>::fill)
    .def("setElements", &msl::DA<int>::setElements)
	.def("printarray", &msl::DA<int>::printarray)
	.def("mapInPlace", &msl::DA<int>::mapInPlace)
	.def("mapIndexInPlace", &msl::DA<int>::mapIndexInPlace)
	.def("map", &msl::DA<int>::map)
	.def("mapIndex", &msl::DA<int>::mapIndex)
    ;
}
