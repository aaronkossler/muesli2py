/* File: example.c */

#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
//#include <mpi.h>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <pybind11/numpy.h>

namespace py = pybind11;

class da {
    int size;
    int elements [];
public:
    da(int c_size){
        size = c_size;
        elements [size];
    }

    da(int c_size, int value){
        size = c_size;
        elements [size];
        fill(value);
    }

    ~da(){
        printf("Destructor\n");
    }

    int get_size() {
        return size;
    }

    void fill(int value) {
        for (int i = 0; i < size; i++) elements[i] = value;
    }

    void printarray() {
        printf("[");
        for (int i = 0; i< size; i++) printf("%d;", elements[i]);
        printf("]");
    }
};

PYBIND11_MODULE(da, da_handle) {
  da_handle.doc() = "I'm a docstring hehe";
  py::class_<da>(
			da_handle, "Pyda"
			)
	.def(py::init<int>())
	.def(py::init<int, int>())
	.def("get_size", &da::get_size)
	.def("fill", &da::fill)
	.def("printarray", &da::printarray)
    ;
}
