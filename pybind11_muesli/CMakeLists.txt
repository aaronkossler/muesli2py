cmake_minimum_required(VERSION 3.4...3.18)
project(pybind11_muesli)

find_package(PythonLibs)
include_directories(${PYTHON_INCLUDE_DIRS})

find_package(OpenACC REQUIRED)
find_package(MPI REQUIRED)

set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS} ${OPENACC_CXX_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

include_directories(${MPI_INCLUDE_PATH})

add_subdirectory(pybind11)
pybind11_add_module(muesli module.cpp src/muesli.cpp src/muesli_com.tpp src/dm.cpp src/da.cpp)

target_link_libraries(muesli PRIVATE mpi)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/testDA.py
  DESTINATION ${CMAKE_CURRENT_BINARY_DIR}/)
