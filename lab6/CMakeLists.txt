cmake_minimum_required(VERSION 3.30)
project(lab6)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(lab6-1 task1.cpp)
add_executable(lab6-2 task2.cpp)

target_link_libraries(lab6-1 OpenMP::OpenMP_CXX)
target_link_libraries(lab6-2 OpenMP::OpenMP_CXX)
