#pragma once

#include <mpi.h>

#ifdef _OPENMP
   #include <omp.h>
#else
   #define omp_get_thread_num() 0
#endif

#include <pybind11/pybind11.h>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <string>
#include <limits>
#include <sstream>
#include <cstdarg>
#include <vector>
#include <math.h>

#include "detail/exception.h"
//#include "conversion.h"
#include "timer.h"

#define MSL_USERFUNC
//#define MSL_GPUFUNC
#define MSL_CPUFUNC

namespace py = pybind11;

namespace msl {

typedef int ProcessorNo;

enum Distribution {DIST, COPY};

class Muesli
{
public:
  static int proc_id;                   // process id
  static int proc_entrance;             // process entrance (farm skeleton)
  static int running_proc_no;           // running process number (farm skeleton)
  static int num_total_procs;           // number of total processes
  static int num_local_procs;           // equals num_total_procs except when nesting DP into TP skeletons
  static double start_time;             // start time of an application
  static char* program_name;            // program name of an application
  static int distribution_mode;         // for farm skeleton
  static int task_group_size;           // aggregated task group size (farm skeleton)
  static int num_conc_kernels;          // number of concurrent kernels (farm skeleton)
  static int num_threads;               // number of CPU threads
  static int num_runs;                  // number of runs, for benchmarking
//  static int num_gpus;                // number of GPUs
//  static double cpu_fraction;           // fraction of each DA partition handled by CPU cores (rather than GPUs)
//  static int max_gpus;                // maximum number of GPUs of each process
//  static int threads_per_block;         // for one dimensional GPU thread blocks (DArray)
//  static int tpb_x; // for two dimensional GPU thread blocks (DMatrix)
//  static int tpb_y; // for two dimensional GPU thread blocks (DMatrix)
  static bool debug_communication;      // farm skeleton
  static bool use_timer;                // use a timer?
  static bool farm_statistics;          // collect statistics of how many task were processed by CPU/GPU

};

static const int ANY_TAG = MPI_ANY_TAG;
static const int MYTAG = 1; // used for ordinary messages containing data
static const int STOPTAG = 2; // used to stop the following process
static const int TERMINATION_TEST = 3;
static const int RANDOM_DISTRIBUTION = 1;
static const int CYCLIC_DISTRIBUTION = 2;
static const int DEFAULT_DISTRIBUTION = CYCLIC_DISTRIBUTION;
static const int UNDEFINED = -1;
static const int DEFAULT_TASK_GROUP_SIZE = 256;
static const int DEFAULT_NUM_CONC_KERNELS = 16;
static const int DEFAULT_NUM_RUNS = 1;
static const int DEFAULT_TILE_WIDTH = 16;

/**
 * \brief Initializes Muesli. Needs to be called before any skeleton is used.
 */
//void initSkeletons(int argc, char** argv, bool debug_communication = 0);
void initSkeletons(bool debug_communication = 0);

/**
 * \brief Terminates Muesli. Needs to be called at the end of a Muesli application.
 */
void terminateSkeletons();

/**
 * \brief Wrapper for printf. Only process with id 0 prints the given format string.
 */
//void printv(const char* format, ...);

/**
 * \brief Sets the number of CPU threads.
 *
 * @param num_threads The number of CPU threads.
 */
void setNumThreads(int num_threads);

/**
 * \brief Sets the number of runs for a benchmark application.
 *
 * @param num_runs The number of runs for a benchmark application.
 */
void setNumRuns(int num_runs);

/**
 * \brief Sets the task group size (i.e. size of sets to be processed) for the
 *        heterogeneous farm skeleton.
 *
 * @param size The task group size.
 */
void setTaskGroupSize(int size);

/**
 * \brief Starts timing
 */
void startTiming();

/**
 * \brief Prints the time elapsed since last split time.
 */
void splitTime(int run);

/**
 * \brief Ends timing.
 *
 * @return Elapsed time.
 */
double stopTiming();

/**
 * \brief Checks whether this is process with id 0.
 *
 * @return True if process id equals 0.
 */
bool isRootProcess();

/**
 * \brief Switches on or off (depending on the value of \em val) collecting farm
 *        statistics.
 */
void setFarmStatistics(bool val);

//
// SEND/RECV TAGS
//

/**
 * \brief Sends a message without content. Mainly used for control messages such
 *        as stop messages.
 *
 * @param destination The destination process id of the message.
 * @param tag Message tag.
 */
inline void MSL_SendTag(int destination, int tag);

/**
 * \brief Receives a message without content. Mainly used for control messages such
 *        as stop messages.
 *
 * @param source The source process id of the message.
 * @param tag Message tag.
 */
inline void MSL_ReceiveTag(int source, int tag);


//
// SEND/RECV FOR DATA PARALLEL SKELETONS
//

/**
 * \brief Sends a buffer of type \em T to process \em destination.
 *
 * @param destination The destination process id.
 * @param send_buffer The send buffer.
 * @param size Size (number of elements) of the message.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Send(int destination, T* send_buffer, size_t size, int tag = MYTAG);

/**
 * \brief Sends (non-blocking) a buffer of type \em T to process \em destination.
 *
 * @param destination The destination process id.
 * @param send_buffer The send buffer.
 * @param req MPI request to check for completion.
 * @param size Size (number of elements) of the message.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_ISend(int destination, T* send_buffer, MPI_Request& req, size_t size, int tag = MYTAG);

/**
 * \brief Receives a buffer of type \em T from process \em source.
 *
 * @param source The source process id.
 * @param recv_buffer The receive buffer.
 * @param size Size (number of elements) of the message.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Recv(int source, T* recv_buffer, size_t size, int tag = MYTAG);

/**
 * \brief Receives a buffer of type \em T from process \em source.
 *
 * @param source The source process id.
 * @param recv_buffer The receive buffer.
 * @param stat MPI status to check for completion.
 * @param size Size (number of elements) of the message.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Recv(int source, T* recv_buffer, MPI_Status& stat, size_t size, int tag = MYTAG);

/**
 * \brief Receives (non-blockig) a buffer of type \em T from process \em source.
 *
 * @param source The source process id.
 * @param recv_buffer The receive buffer.
 * @param req MPI request to check for completion.
 * @param size Size (number of elements) of the message.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_IRecv(int source, T* recv_buffer, MPI_Request& req, size_t size, int tag = MYTAG);

// Send/receive function for sending a buffer of type T to process \em destination and
// receiving a buffer of type T from the same process (destination).
template<typename T>
inline void MSL_SendReceive(int destination, T* send_buffer, T* recv_buffer, size_t size = 1);

/**
 * \brief Implementation of the MPI_Broadcast routine. Only the processes in
 *        \em ids participate.
 *
 * @param buffer Message buffer.
 * @param ids The process ids that participate in broadcasting.
 * @param np Number of processes that participate.
 * @param idRoot Root process id of the broadcast.
 * @param count Number of elements in \em buffer.
 * @tparam T Type of the message.
 */
template<typename T>
void broadcast(T* buffer, int* const ids, int np, int idRoot, size_t count);

/**
 * \brief Implementation of the MPI_Allgather routine. Only the processes in
 *        ȩm ids participate.
 *
 * @param send_buffer Send buffer.
 * @param recv_buffer Receive buffer.
 * @param ids The process ids that participate in broadcasting.
 * @param np Number of processes that participate.
 * @param count Number of elements in \em send_buffer.
 * @tparam T Type of the message.
 */
template<typename T>
void allgather(T* send_buffer, T* recv_buffer, int* const ids, int np, size_t count);

/**
 * \brief Wrapper for the MPI_Allgather routine. Every process in \em MPI_COMM WORLD
 *        participates.
 *
 * @param send_buffer Send buffer.
 * @param recv_buffer Receive buffer.
 * @param count Number of elements in \em send_buffer.
 * @tparam T Type of the message.
 */
template<typename T>
void allgather(T* send_buffer, T* recv_buffer, int count);

/**
 * \brief Wrapper for the MPI_Scatter routine. Every process in \em MPI_COMM WORLD
 *        participates.
 *
 * @param send_buffer Send buffer.
 * @param recv_buffer Receive buffer.
 * @param count Number of elements in \em send_buffer.
 * @tparam T Type of the message.
 */
template<typename T>
void scatter(T* send_buffer, T* recv_buffer, size_t count);

/**
 * \brief Wrapper for the MPI_Broadcast routine. Every process in \em MPI_COMM WORLD
 *        participates.
 *
 * @param source Root process id of the broadcast.
 * @param buffer The message buffer.
 * @param size Number of elements to broadcast.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Broadcast(int source, T* buffer, int size);

/**
 * \brief Wrapper for the MPI_Barrier routine. Every process in \em MPI_COMM WORLD
 *        participates.
 *
 */
inline void barrier();


//
// SEND/RECV FOR TASK PARALLEL SKELETONS
//

/**
 * \brief Sends a std::vector of type \em T to process \em destination.
 *
 * @param destination The destination process id.
 * @param send_buffer The send buffer.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Send(int destination, std::vector<T>& send_buffer, int tag = MYTAG);

/**
 * \brief Receives a std::vector of type \em T from process \em source.
 *
 * @param source The source process id.
 * @param send_buffer The receive buffer.
 * @param tag Message tag.
 * @tparam T Type of the message.
 */
template <typename T>
inline void MSL_Recv(int source, std::vector<T>& recv_buffer, int tag = MYTAG);

//
// AUXILIARY FUNCTIONS
//

/**
 * \brief Used to quit the program on failure,  must be used after initSkeletons()
 */
void fail_exit();

/**
 * \brief Throws an Exception.
 *
 * @param e The exception to throw.
 */
void throws(const detail::Exception& e);

template <typename C1, typename C2>
inline C1 proj1_2(C1 a, C2 b);

template <typename C1, typename C2>
inline C2 proj2_2(C1 a, C2 b);

//template <typename F>
// inline int auxRotateRows(const Fct1<int, int, F>& f, int blocks, int row, int col);

//template <typename F>
// inline int auxRotateCols(const Fct1<int, int, F>& f, int blocks, int row, int col);

template <typename T>
inline void show(T* a, int size);

int doSomething();


}

//
// BINDING FUNCTION
//

void bind_muesli(py::module& m);

#include "../src/muesli_com.tpp"