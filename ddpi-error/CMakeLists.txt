cmake_minimum_required(VERSION 3.10)
set(CMAKE_CUDA_COMPILER /usr/bin/nvcc)
project(ddpi_test LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenCV REQUIRED)

add_executable(ddpi_test src/main.cpp)

target_link_libraries(ddpi_test PRIVATE ddpiAlg ${OpenCV_LIBS})
target_include_directories(ddpi_test PRIVATE ./ddpi-alg-main/include ${OpenCV_INCLUDE_DIRS})
add_subdirectory(./ddpi-alg-main)