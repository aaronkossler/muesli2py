#include <pybind11/pybind11.h>
#include <stdlib.h>
#include <stdio.h>
#include <error.h>
#include <omp.h>
#include <mpi.h>
#include <iostream>
#include "../include/muesli.h"

int msl::Muesli::proc_id;
int msl::Muesli::proc_entrance;
int msl::Muesli::running_proc_no = 0;
int msl::Muesli::num_total_procs;
int msl::Muesli::num_local_procs; // equals numOfTotalProcs except when nesting DP into TP skeletons
double msl::Muesli::start_time;
char* msl::Muesli::program_name;
int msl::Muesli::distribution_mode;
int msl::Muesli::task_group_size;
//int msl::Muesli::num_conc_kernels;
int msl::Muesli::num_threads;
int msl::Muesli::num_runs;
//int msl::Muesli::num_gpus;
//int msl::Muesli::max_gpus;
double msl::Muesli::cpu_fraction = 0.2; // fraction of each DA partition handled by CPU cores (rather than GPUs)
int msl::Muesli::threads_per_block;
//int msl::Muesli::tpb_x;
//int msl::Muesli::tpb_y;
bool msl::Muesli::debug_communication;
bool msl::Muesli::use_timer;
bool msl::Muesli::farm_statistics = false;
msl::Timer* timer;


//void msl::initSkeletons(int argc, char** argv, bool debug_communication)
void msl::initSkeletons(bool debug_communication)
{
//  MPI_Init(&argc, &argv);
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &Muesli::num_total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &Muesli::proc_id);
  #ifdef _OPENMP
    omp_set_nested(1);
  #endif

/*  if (1 <= argc) {
    Muesli::program_name = argv[0];
  }*/

  int device_count = 0;

  Muesli::task_group_size = DEFAULT_TASK_GROUP_SIZE;
//  Muesli::num_conc_kernels = DEFAULT_NUM_CONC_KERNELS;

  #ifdef _OPENMP
    Muesli::num_threads = omp_get_max_threads();
  #else
    Muesli::num_threads = 1;
  #endif

  Muesli::debug_communication = debug_communication;
  Muesli::num_runs = DEFAULT_NUM_RUNS;
  Muesli::num_local_procs = Muesli::num_total_procs;
  Muesli::proc_entrance = 0;
//  setThreadsPerBlock(512); // default for one dimensional thread blocks
//  setThreadsPerBlock(16, 16); // default for two dimensional thread blocks
  Muesli::start_time = MPI_Wtime();
}

void msl::terminateSkeletons()
{
  std::ostringstream s;
  std::ostringstream s_time;
  if (isRootProcess())
    printf("debug: terminating skeletons\n");
  MPI_Barrier(MPI_COMM_WORLD);
  if (isRootProcess())
    printf("debug: behind barrier\n");

  if (Muesli::use_timer) {
    double total_time = timer->totalTime();
    s_time << "Total time: " << total_time << "s" << std::endl;
  }

  if (isRootProcess()) {
//    s << std::endl << "Name: " << Muesli::program_name << std::endl;
    s << "Proc: " << Muesli::num_total_procs << std::endl;
    s << "CPU only" << std::endl;
    s << "Threads per proc: " << Muesli::num_threads << std::endl;
    if (Muesli::use_timer) {
      s << s_time.str();
      delete timer;
    } else {
      s << "Total time: " << MPI_Wtime() - Muesli::start_time << "s" << std::endl;
    }

    printf("%s", s.str().c_str());
  }
  if (isRootProcess())
    printf("debug: behind output of run time statistics\n");

  MPI_Finalize();
  Muesli::running_proc_no = 0;
}

/*void msl::printv(const char* format, ...) {
  va_list argp;
  va_start(argp, format);

  if(isRootProcess()) {
    vprintf(format, argp);
  }

  va_end(argp);
}*/

void msl::setNumThreads(int threads)
{
  #ifdef _OPENMP
    Muesli::num_threads = threads;
    omp_set_num_threads(threads);
  #else
    Muesli::num_threads = 1;
  #endif
}

void msl::setNumRuns(int runs)
{
  Muesli::num_runs = runs;
}

void msl::setTaskGroupSize(int size)
{
  Muesli::task_group_size = size;
}

void msl::startTiming()
{
  Muesli::use_timer = 1;
  MPI_Barrier(MPI_COMM_WORLD);
  timer = new Timer();
}

void msl::splitTime(int run)
{
  double result = timer->splitTime();
  if (isRootProcess()) {
    std::cout << "Run " << run << ": " << result << "s" << std::endl;
  }
}

double msl::stopTiming()
{
  int runs = timer->getNumSplits();
  double total_time = timer->stop();
  printf("\nTime/run: %fs\n", total_time/runs);
  return total_time;
}

bool msl::isRootProcess()
{
  return Muesli::proc_id == 0;
}

void msl::setFarmStatistics(bool val)
{
  Muesli::farm_statistics = val;
}

void msl::fail_exit()
{
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Finalize();
  exit(EXIT_FAILURE);
}

//void msl::throws(const detail::Exception& e)
//{
//  std::cout << Muesli::proc_id << ": " << e << std::endl;
//}

PYBIND11_MODULE(muesli, muesli_handle) {
  muesli_handle.doc() = "I'm a docstring hehe";
  muesli_handle.def("initSkeletons", &msl::initSkeletons);
  muesli_handle.def("terminateSkeletons", &msl::terminateSkeletons);
//  muesli_handle.def("printv", &msl::printv);
  muesli_handle.def("setNumThreads", &msl::setNumThreads);
  muesli_handle.def("setNumRuns", &msl::setNumRuns);
  muesli_handle.def("setTaskGroupSize", &msl::setTaskGroupSize);
  muesli_handle.def("startTiming", &msl::startTiming);
  muesli_handle.def("splitTime", &msl::splitTime);
  muesli_handle.def("stopTiming", &msl::stopTiming);
  muesli_handle.def("isRootProcess", &msl::isRootProcess);
  muesli_handle.def("setFarmStatistics", &msl::setFarmStatistics);
  muesli_handle.def("fail_exit", &msl::fail_exit);
//  muesli_handle.def("throws", &msl::throws);
}
