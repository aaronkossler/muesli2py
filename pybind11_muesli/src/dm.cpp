/* File: example.c */

#include "../include/muesli.h"
#include "../include/dm.h"

//#include <stdexcept>
//#include <iostream>
//#include <stdlib.h>
//#include <stdio.h>
//#include <error.h>
//#include <omp.h>
//#include <mpi.h>
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
}

// constructor creates a DM, initialized with v
template<typename T>
msl::DM<T>::DM(int row, int col, const T& v)
    : ncol(col), nrow(row), n(col*row){
    init();
    fill(v);
}

// auxiliary method init()
template<typename T>
void msl::DM<T>::init() {
  if (Muesli::proc_entrance == UNDEFINED) {
    throws(detail::MissingInitializationException());
  }
  id = Muesli::proc_id;
  np = Muesli::num_total_procs;
  n = ncol * nrow;
  nLocal = n / np;
  nCPU = nLocal;
  firstIndex =  id * nLocal;
  localPartition = new T[nLocal];
  printf("loc processes %d , First index %d\n", Muesli::num_local_procs, firstIndex);
  printf("Building datastructure with %d nodes and %d cpus\n", msl::Muesli::num_total_procs,
         msl::Muesli::num_local_procs);
}

// destructor removes a DM
template<typename T>
msl::DM<T>::~DM() {
  printf("Destructor\n");
  //delete[] localPartition;
}

template<typename T>
void msl::DM<T>::fill(const T& value) {
    #pragma acc parallel loop
    for (int k = 0; k < nLocal; k++) {
        localPartition[k] = value;
    }
}

// **************************** auxiliary methods ****************************
template<typename T>
T* msl::DM<T>::getLocalPartition() {
  return localPartition;
}

/*template<typename T>
void msl::DM<T>::setLocalPartition(py::array_t<T> array) {
    #pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        localPartition[k] = *array.data(k);
    }
}*/

template<typename T>
T msl::DM<T>::get(int index) const {
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
int msl::DM<T>::getSize() const {
  return n;
}

template<typename T>
int msl::DM<T>::getLocalSize() const {
  return nLocal;
}

template<typename T>
int msl::DM<T>::getFirstIndex() const {
  return firstIndex;
}

template<typename T>
bool msl::DM<T>::isLocal(int index) const {
  return (index >= firstIndex) && (index < firstIndex + nLocal);
}

template<typename T>
T msl::DM<T>::getLocal(int localIndex) {
  if (localIndex >= nLocal)
    throws(detail::NonLocalAccessException());
  return localPartition[localIndex];
}

template<typename T>
void msl::DM<T>::setLocal(int localIndex, const T& v) {
  if (localIndex < nCPU) {
    localPartition[localIndex] = v;
  } else if (localIndex >= nLocal)
    throws(detail::NonLocalAccessException());
}

template<typename T>
void msl::DM<T>::set(int globalIndex, const T& v) {
  if ((globalIndex >= firstIndex) && (globalIndex < firstIndex + nLocal)) {
    setLocal(globalIndex - firstIndex, v);
  }
}

// method (only) useful for debugging
template<typename T>
//void msl::DM<T>::showLocal(const std::string& descr) {
void msl::DM<T>::showLocal() {
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

// TODO: fix this maybe
template<typename T>
//void msl::DM<T>::show(const std::string& descr) {
void msl::DM<T>::show() {
  T* b = new T[n];
  std::ostringstream s;
//  if (descr.size() > 0)
//    s << descr << ": " << std::endl;

  msl::allgather(localPartition, b, nLocal);

  if (msl::isRootProcess()) {
    s << "[";
    for (int i = 0; i < n - 1; i++) {
      s << b[i];
      ((i+1) % ncol == 0) ? s << "\n " : s << " ";;
    }
    s << b[n - 1] << "]" << std::endl;
    s << std::endl;
  }

  delete b;

  if (msl::isRootProcess()) printf("%s", s.str().c_str());
}

/*template<typename T>
void msl::DM<T>::printmatrix() {
    #pragma acc parallel loop
    for (int i = 0; i< nrow; i++) {
    printf("|");
        #pragma acc parallel loop
        for (int j = 0; j< ncol; j++) {
            printf("%d;", elements[nrow*i+j]);
        }
    printf("|\n");
    }
}*/

//*********************************** Maps ***********************************
template<typename T>
void msl::DM<T>::mapInPlace(const std::function<T(T)> &f) {
    #pragma acc parallel loop
    for (int k = 0; k < nCPU; k++) {
        setLocal(k, f(localPartition[k]));
    }
}

template<typename T>
void msl::DM<T>::mapIndexInPlace(const std::function<T(int,T)> &f) {

  // all necessary calculations are performed otherwise some are skipped.
  #pragma acc parallel loop
  for (int k = 0; k < nCPU; k++) {
    int globalindex = k + firstIndex;
    localPartition[k] = f(globalindex, localPartition[k]);
  }
}

template<typename T>
void msl::DM<T>::mapIndexInPlace(const std::function<T(int,int,T)> &f) {

  // all necessary calculations are performed otherwise some are skipped.
  #pragma acc parallel loop
  for (int k = 0; k < nCPU; k++) {
    int row = (k + firstIndex) / ncol;
    int col = (k + firstIndex) % ncol;
    localPartition[k] = f(row, col, localPartition[k]);
  }
}

template<typename T>
msl::DM<T> msl::DM<T>::map(const std::function<T(T)> &f) {
    DM<T> result(ncol, nrow);

    #pragma acc parallel loop
    for (int k = 0; k < nCPU; k++){
        result.setLocal(k, f(localPartition[k]));
    }

    return result;
}

template<typename T>
msl::DM<T> msl::DM<T>::mapIndex(const std::function<T(int,T)> &f) {
    DM<T> result(ncol, nrow);

    #pragma acc parallel loop
    for (int k = 0; k < nCPU; k++){
      int globalindex = k + firstIndex;
      result.setLocal(k, f(globalindex, localPartition[k]));
    }

    return result;
}

template<typename T>
msl::DM<T> msl::DM<T>::mapIndex(const std::function<T(int,int,T)> &f) {
    DM<T> result(ncol, nrow);

    #pragma acc parallel loop
    for (int k = 0; k < nCPU; k++){
      int row = (k + firstIndex) / ncol;
      int col = (k + firstIndex) % ncol;
      result.setLocal(k, f(row, col, localPartition[k]));
    }

    return result;
}

void bind_dm(py::module& m) {
    py::class_<msl::DM<int>>(
			m, "Pydm"
			)
    .def(py::init())
	.def(py::init<int, int>())
	.def(py::init<int, int, int>())
	.def("fill", &msl::DM<int>::fill)
//	.def("setElements", &msl::DM<int>::setElements)
//	.def("printmatrix", &msl::DM<int>::printmatrix)
	.def("mapInPlace", &msl::DM<int>::mapInPlace)
	.def("mapIndexInPlace", py::overload_cast<const std::function<int(int,int)> &>(&msl::DM<int>::mapIndexInPlace))
	.def("mapIndexInPlace", py::overload_cast<const std::function<int(int,int,int)> &>(&msl::DM<int>::mapIndexInPlace))
	.def("map", &msl::DM<int>::map)
	.def("mapIndex", py::overload_cast<const std::function<int(int,int)> &>(&msl::DM<int>::mapIndex))
	.def("mapIndex", py::overload_cast<const std::function<int(int,int,int)> &>(&msl::DM<int>::mapIndex))
    .def("getLocalPartition", &msl::DM<int>::getLocalPartition)
//    .def("setLocalPartition", &msl::DM<int>::setLocalPartition)
    .def("get", &msl::DM<int>::get)
    .def("set", &msl::DM<int>::set)
    .def("showLocal", &msl::DM<int>::showLocal)
    .def("show", &msl::DM<int>::show)
    .def("getSize", &msl::DM<int>::getSize)
    .def("getLocalSize", &msl::DM<int>::getLocalSize)
    .def("getFirstIndex", &msl::DM<int>::getFirstIndex)
    .def("getLocal", &msl::DM<int>::getLocal)
    .def("setLocal", &msl::DM<int>::setLocal)
    ;
}
