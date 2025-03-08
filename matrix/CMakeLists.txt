cmake_minimum_required(VERSION 3.5.0)
project(matrix VERSION 0.1.0 LANGUAGES C CXX)

find_package(MPI)
set(CMAKE_CXX_COMPILER mpic++)

add_executable(multiplication main.cpp)
add_executable(optimal-multiplication optimal.cpp)

target_link_libraries(multiplication openmpi::openmpi)
target_link_libraries(optimal-multiplication openmpi::openmpi)

