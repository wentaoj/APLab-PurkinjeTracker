cmake_minimum_required(VERSION 3.28)
set(CMAKE_CUDA_COMPILER /usr/bin/nvcc)
project(DoHTracker LANGUAGES CXX CUDA)
set(CMAKE_CXX_STANDARD 17)
# set(CMAKE_CUDA_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

find_package(OpenCV REQUIRED)
# find_package(CUDA REQUIRED)# find_package(nlohmann_json REQUIRED)

add_executable(DoHTracker
    src/main.cpp
    src/tracker.cpp
    src/base_tracker.cpp
    src/blob.cpp
    src/utils.hpp
)
target_include_directories(DoHTracker PRIVATE ${OpenCV_INCLUDE_DIRS})
target_link_libraries(DoHTracker PRIVATE ${OpenCV_LIBS}) # ${CUDA_LIBRARIES}
