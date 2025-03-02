cmake_minimum_required(VERSION 3.5.0)
project(test-mpi VERSION 0.1.0 LANGUAGES C CXX)
message(STATUS ${CONAN_ENV_SCRIPT})

set(CMAKE_CXX_COMPILER mpic++)

find_package(MPI)

add_executable(test-mpi main.cpp)

target_link_libraries(test-mpi openmpi::openmpi)

