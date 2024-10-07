set(CMAKE_CXX_FLAGS "-static")

add_executable(lab2 main.cpp libbmp/libbmp.cpp libbmp/libbmp.h blur/blur.cpp blur/blur.h)
