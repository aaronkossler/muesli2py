#include <pybind11/pybind11.h>
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
int msl::Muesli::num_runs;
int msl::Muesli::num_gpus;
bool msl::Muesli::debug_communication;
bool msl::Muesli::use_timer;
bool msl::Muesli::farm_statistics = false;
msl::Timer* timer;


void msl::initSkeletons(bool debug_communication)
{
  MPI_Init(NULL, NULL);
  MPI_Comm_size(MPI_COMM_WORLD, &Muesli::num_total_procs);
  MPI_Comm_rank(MPI_COMM_WORLD, &Muesli::proc_id);

  int device_count = 0;

  Muesli::task_group_size = DEFAULT_TASK_GROUP_SIZE;

  Muesli::debug_communication = debug_communication;
  Muesli::num_runs = DEFAULT_NUM_RUNS;
  Muesli::num_local_procs = Muesli::num_total_procs;
  Muesli::proc_entrance = 0;
  Muesli::start_time = MPI_Wtime();
}

void msl::terminateSkeletons()
{
  std::ostringstream s;
  std::ostringstream s_time;
/*  if (isRootProcess())
    printf("debug: terminating skeletons\n");*/
  MPI_Barrier(MPI_COMM_WORLD);
/*  if (isRootProcess())
    printf("debug: behind barrier\n");*/

  if (Muesli::use_timer) {
    double total_time = timer->totalTime();
    s_time << "Total time: " << total_time << "s" << std::endl;
  }

  if (isRootProcess()) {
//    s << std::endl << "Name: " << Muesli::program_name << std::endl;
    s << "Proc: " << Muesli::num_total_procs << std::endl;
    s << "CPU only" << std::endl;
//    s << "Threads per proc: " << Muesli::num_threads << std::endl;
    if (Muesli::use_timer) {
      s << s_time.str();
      delete timer;
    } else {
      s << "Total time: " << MPI_Wtime() - Muesli::start_time << "s" << std::endl;
    }

//    printf("%s", s.str().c_str());
  }
  /*if (isRootProcess())
    printf("debug: behind output of run time statistics\n");*/

  MPI_Finalize();
  Muesli::running_proc_no = 0;
}

int msl::getNumRuns()
{
    return Muesli::num_runs;
}

int msl::getNumGpus()
{
    return Muesli::num_gpus;
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

void msl::throws(const detail::Exception& e)
{
  std::cout << Muesli::proc_id << ": " << e << std::endl;
}

void bind_muesli(py::module& m) {
  m.def("initSkeletons", &msl::initSkeletons);
  m.def("terminateSkeletons", &msl::terminateSkeletons);
  m.def("setNumRuns", &msl::setNumRuns);
  m.def("getNumRuns", &msl::getNumRuns);
  m.def("getNumGpus", &msl::getNumGpus);
  m.def("setTaskGroupSize", &msl::setTaskGroupSize);
  m.def("setFarmStatistics", &msl::setFarmStatistics);
  m.def("fail_exit", &msl::fail_exit);
  m.def("isRootProcess", &msl::isRootProcess);
  py::class_<msl::Muesli>(m, "Muesli")
      .def_readonly_static("num_runs",  &msl::Muesli::num_runs)
  ;
}
