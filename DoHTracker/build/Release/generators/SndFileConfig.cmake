########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(SndFile_FIND_QUIETLY)
    set(SndFile_MESSAGE_MODE VERBOSE)
else()
    set(SndFile_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/SndFileTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${libsndfile_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(SndFile_VERSION_STRING "1.2.2")
set(SndFile_INCLUDE_DIRS ${libsndfile_INCLUDE_DIRS_RELEASE} )
set(SndFile_INCLUDE_DIR ${libsndfile_INCLUDE_DIRS_RELEASE} )
set(SndFile_LIBRARIES ${libsndfile_LIBRARIES_RELEASE} )
set(SndFile_DEFINITIONS ${libsndfile_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${libsndfile_BUILD_MODULES_PATHS_RELEASE} )
    message(${SndFile_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


