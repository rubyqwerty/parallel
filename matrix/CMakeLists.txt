cmake_minimum_required(VERSION 3.5.0)
project(matrix VERSION 0.1.0 LANGUAGES C CXX)

find_package(MPI)
set(CMAKE_CXX_COMPILER mpic++)

add_executable(matrix main.cpp)

target_link_libraries(matrix openmpi::openmpi)

