#pragma once

#include <mpi.h>

#ifdef _OPENMP
   #include <omp.h>
#else
   #define omp_get_thread_num() 0
#endif

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <limits>
#include <sstream>
#include <cstdarg>
#include <vector>
#include <math.h>

#include "exception.h"
#include "conversion.h"
#include "timer.h"

#define MSL_USERFUNC
#define MSL_GPUFUNC
#define MSL_CPUFUNC

namespace msl {

typedef int ProcessorNo;

enum Distribution {DIST, COPY};

class Muesli
{
public:
  static int proc_id; // process id
  static int proc_entrance; // process entrance (farm skeleton)
  static int running_proc_no; // running process number (farm skeleton)
  static int num_total_procs; // number of total processes
  static int num_local_procs; // equals num_total_procs except when nesting DP into TP skeletons
  static double start_time; // start time of an application
  static char* program_name; // program name of an application
  static int distribution_mode; // for farm skeleton
  static int task_group_size; // aggregated task group size (farm skeleton)
  static int num_conc_kernels; // number of concurrent kernels (farm skeleton)
  static int num_threads; // number of CPU threads
  static int num_runs; // number of runs, for benchmarking
//  static int num_gpus; // number of GPUs
  static double cpu_fraction; // fraction of each DA partition handled by CPU cores (rather than GPUs)
//  static int max_gpus; // maximum number of GPUs of each process
  static int threads_per_block; // for one dimensional GPU thread blocks (DArray)
//  static int tpb_x; // for two dimensional GPU thread blocks (DMatrix)
//  static int tpb_y; // for two dimensional GPU thread blocks (DMatrix)
  static bool debug_communication; // farm skeleton
  static bool use_timer; // use a timer?
  static bool farm_statistics; // collect statistics of how many task were processed by CPU/GPU

};

static const int ANY_TAG = MPI_ANY_TAG;
static const int MYTAG = 1; // used for ordinary messages containing data
static const int STOPTAG = 2; // used to stop the following process
static const int TERMINATION_TEST = 3;
static const int RANDOM_DISTRIBUTION = 1;
static const int CYCLIC_DISTRIBUTION = 2;
static const int DEFAULT_DISTRIBUTION = CYCLIC_DISTRIBUTION;
static const int UNDEFINED = -1;
static const int DEFAULT_TASK_GOUP_SIZE = 256;
static const int DEFAULT_NUM_CONC_KERNELS = 16;
static const int DEFAULT_NUM_RUNS = 1;
static const int DEFAULT_TILE_WIDTH = 16;

}