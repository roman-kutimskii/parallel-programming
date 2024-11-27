cmake_minimum_required(VERSION 3.30)
project(lab6)

set(CMAKE_CXX_STANDARD 20)

find_package(OpenMP REQUIRED)

add_executable(lab6 main.cpp)

target_link_libraries(lab6 OpenMP::OpenMP_CXX)
