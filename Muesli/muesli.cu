//muesli.c//

#include <Python.h>
#include <stdlib.h>
#include <error.h>
#include <cmath>
#include <chrono>
#include <omp.h>
#include <mpi.h>
//#ifdef __CUDACC__
#include <cuda.h>
// #endif
#include <string>

std::chrono::steady_clock::time_point start_time;
std::chrono::steady_clock::time_point end_time;

int Cfib(int n)
{
    if (n < 2)
        return n;
    else
        return Cfib(n-1) + Cfib(n-2);
}
 
static PyObject* fib(PyObject* self, PyObject* args)
{
    int n;
 
    if (!PyArg_ParseTuple(args, "i", &n))
        return NULL;
 
    return Py_BuildValue("i", Cfib(n));
}
static PyObject* test_mpi(PyObject* self, PyObject* args)
{
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);

    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);

    // Print off a hello world message
    printf("Hello world from processor %s, rank %d out of %d processors\n",
           processor_name, world_rank, world_size);

    // Finalize the MPI environment.
    MPI_Finalize();
    return Py_BuildValue("i", 2);

}
static PyObject* test_openmp(PyObject* self, PyObject* args){
    #pragma omp parallel for
    for (int i = 0; i < 5; i++) {
        printf("Hello from process: %d \n", i);
    }
    return Py_BuildValue("i", 1);
}
#ifdef __CUDACC__
static PyObject* test_cuda(PyObject* self, PyObject* args){
    cudaStream_t stream;
    cudaStreamCreate(&stream);                                                                  // Create CUDA stream
    int device_id;
    cudaDeviceProp prop;                                                                        // CUDA device properties variable
    cudaGetDeviceProperties( &prop, device_id);
    const char * s = "Device Name: ";
    return Py_BuildValue("s", s);
}
#endif
static PyObject* start_timer(PyObject* self, PyObject* args) {
    start_time = std::chrono::steady_clock::now();
    return Py_BuildValue("i", (start_time));
}
static PyObject* end_timer(PyObject* self, PyObject* args)
{
    end_time = std::chrono::steady_clock::now();
    return Py_BuildValue("i", (end_time-start_time));
}
static PyObject* version(PyObject* self)
{
    return Py_BuildValue("s", "Version 0.000001");
}
 
static PyMethodDef myMethods[] = {
    {"fib", fib, METH_VARARGS, "Muesli Wrapper"},
    {"start_timer", start_timer, METH_VARARGS, "Start Timer"},
    {"end_timer", end_timer, METH_VARARGS, "End Timer"},
    {"test_mpi", test_mpi, METH_VARARGS, "Test MPI"},
    {"test_openmp", test_openmp, METH_VARARGS, "Test OpenMP"},
    {"test_cuda", test_cuda, METH_VARARGS, "Test Cuda"},
    {"version", (PyCFunction)version, METH_NOARGS, "Returns the version."},
    {NULL, NULL, 0, NULL}
};
 
static struct PyModuleDef muesli = {
	PyModuleDef_HEAD_INIT,
	"muesli",
	"Muesli Module",
	500,
	myMethods
};

PyMODINIT_FUNC PyInit_muesli(void)
{
    return PyModule_Create(&muesli);
}

