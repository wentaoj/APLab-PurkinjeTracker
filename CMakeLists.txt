cmake_minimum_required(VERSION 3.10)
set(CMAKE_CUDA_COMPILER /usr/bin/nvcc)
project(Tracker LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenCV REQUIRED)
find_package(nlohmann_json 3.11.3 REQUIRED)

add_executable(Tracker src/main.cpp src/image_generator.cpp src/doh_tracker.cpp src/base_tracker.cpp src/blob.cpp src/utils.hpp)

target_link_libraries(Tracker PRIVATE ddpiAlg ${OpenCV_LIBS} nlohmann_json::nlohmann_json)

target_include_directories(Tracker PRIVATE ./ddpi-alg/include ${OpenCV_INCLUDE_DIRS} ${nlohmann_json_INCLUDE_DIRS})
add_subdirectory(./ddpi-alg)