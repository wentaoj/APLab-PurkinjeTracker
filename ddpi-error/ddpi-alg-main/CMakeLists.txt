cmake_minimum_required(VERSION 3.12)

set(__PROJECT_NAME__ ddpiAlg)

project(${__PROJECT_NAME__} LANGUAGES CXX CUDA)


# Set up CXX and CUDA
set(CMAKE_CUDA_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CUDA_STANDARD 11)
string(APPEND CMAKE_CUDA_FLAGS " -gencode arch=compute_60,code=sm_60")
string(APPEND CMAKE_CUDA_FLAGS " -gencode arch=compute_61,code=sm_61")
string(APPEND CMAKE_CUDA_FLAGS " -gencode arch=compute_75,code=sm_75")
set(CMAKE_CUDA_ARCHITECTURES 60 61 75)

# Search header and source file
file(GLOB DDPI_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)
file(GLOB CUDA_SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cu)

## Compile ddpi library
add_library(${__PROJECT_NAME__} SHARED
		${DDPI_SOURCE_FILES}
		${CUDA_SOURCE_FILES})

set_target_properties(${__PROJECT_NAME__} PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
target_include_directories(${__PROJECT_NAME__} PUBLIC
		${CMAKE_CURRENT_SOURCE_DIR}/include
		${CMAKE_CUDA_TOOLKIT_INCLUDE_DIRECTORIES}
		${CMAKE_CURRENT_SOURCE_DIR}
		${CMAKE_SOURCE_DIR}/src)
		#${CMAKE_SOURCE_DIR}/vendor/spdlog/include)

target_link_directories(${__PROJECT_NAME__} PUBLIC ${CMAKE_CUDA_IMPLICIT_LINK_DIRECTORIES})

target_link_libraries(${__PROJECT_NAME__} PUBLIC
		cudart
		nppc
		nppidei
		nppif
		nppig
		nppitc
		nppist)