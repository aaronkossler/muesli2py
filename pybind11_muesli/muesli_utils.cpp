#include <pybind11/pybind11.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
#include <mpi.h>


double start_time;
double end_time;

double fib(double n)
{
    if (n < 0){ /* This should probably return an error, but this is simpler */
        return 0;
    }
    if (n == 0) {
        return 1;
    }
    else {
        /* testing for overflow would be a good idea here */
        return n * fib(n-1);
    }
}
double start_timer() {
    start_time = MPI_Wtime();
    return start_time;
}
double end_timer()
{
    end_time = MPI_Wtime();
    return end_time-start_time;
}

int test_mpi()
{
    // Initialize the MPI environment
    MPI_Init(NULL, NULL);

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
    return 1;

}
int test_openmp(){
    #pragma omp parallel for
    for (int i = 0; i < 5; i++) {
        printf("Hello from process: %d \n", i);
    }
    return 1;
}

PYBIND11_MODULE(muesli, muesli_handle) {
  muesli_handle.doc() = "I'm a docstring hehe";
  muesli_handle.def("fib", &fib);
  muesli_handle.def("start_timer", &start_timer);
  muesli_handle.def("end_timer", &end_timer);
  muesli_handle.def("test_mpi", &test_mpi);
  muesli_handle.def("test_openmp", &test_openmp);
}

