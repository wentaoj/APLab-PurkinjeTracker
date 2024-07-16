########## MACROS ###########################################################################
#############################################################################################

# Requires CMake > 3.15
if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeDeps' generator only works with CMake >= 3.15")
endif()

if(jbig_FIND_QUIETLY)
    set(jbig_MESSAGE_MODE VERBOSE)
else()
    set(jbig_MESSAGE_MODE STATUS)
endif()

include(${CMAKE_CURRENT_LIST_DIR}/cmakedeps_macros.cmake)
include(${CMAKE_CURRENT_LIST_DIR}/jbigTargets.cmake)
include(CMakeFindDependencyMacro)

check_build_type_defined()

foreach(_DEPENDENCY ${jbig_FIND_DEPENDENCY_NAMES} )
    # Check that we have not already called a find_package with the transitive dependency
    if(NOT ${_DEPENDENCY}_FOUND)
        find_dependency(${_DEPENDENCY} REQUIRED ${${_DEPENDENCY}_FIND_MODE})
    endif()
endforeach()

set(jbig_VERSION_STRING "20160605")
set(jbig_INCLUDE_DIRS ${jbig_INCLUDE_DIRS_RELEASE} )
set(jbig_INCLUDE_DIR ${jbig_INCLUDE_DIRS_RELEASE} )
set(jbig_LIBRARIES ${jbig_LIBRARIES_RELEASE} )
set(jbig_DEFINITIONS ${jbig_DEFINITIONS_RELEASE} )


# Only the last installed configuration BUILD_MODULES are included to avoid the collision
foreach(_BUILD_MODULE ${jbig_BUILD_MODULES_PATHS_RELEASE} )
    message(${jbig_MESSAGE_MODE} "Conan: Including build module from '${_BUILD_MODULE}'")
    include(${_BUILD_MODULE})
endforeach()


