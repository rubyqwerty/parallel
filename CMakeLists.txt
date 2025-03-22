cmake_minimum_required(VERSION 3.5.0)
project(parallel VERSION 0.1.0 LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 23)

list(APPEND Modules
    producer-consumer
    race
    mpi-test
    race-mpi
    matrix
    virtual
)

foreach(module ${Modules})
    message(STATUS "module: ${module}")
    add_subdirectory(${module})    
endforeach()

