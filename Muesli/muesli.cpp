//muesli.c//

#include <Python.h>
#include <stdlib.h>
#include <error.h>
#include <cmath>
#include <chrono>

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
static PyObject* testMPI(PyObject* self, PyObject* args)
{
/*    int size, rank;
    MPI_Comm_size(comm, &size);
    MPI_Comm_rank(comm, &rank);
    printf("Hello, World! "
           "I am process %d of %d.\n",
           rank, size);*/
}

static PyObject* startTimer(PyObject* self, PyObject* args)
{
    start_time = std::chrono::steady_clock::now();
    return Py_BuildValue("i", (start_time));
}
static PyObject* endTimer(PyObject* self, PyObject* args)
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
    {"startTimer", startTimer, METH_VARARGS, "Start Timer"},
    {"endTimer", endTimer, METH_VARARGS, "End Timer"},
    {"testMPI", testMPI, METH_VARARGS, "Test MPI"},
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

