# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(ade_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(ade_FRAMEWORKS_FOUND_RELEASE "${ade_FRAMEWORKS_RELEASE}" "${ade_FRAMEWORK_DIRS_RELEASE}")

set(ade_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET ade_DEPS_TARGET)
    add_library(ade_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET ade_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${ade_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${ade_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### ade_DEPS_TARGET to all of them
conan_package_library_targets("${ade_LIBS_RELEASE}"    # libraries
                              "${ade_LIB_DIRS_RELEASE}" # package_libdir
                              "${ade_BIN_DIRS_RELEASE}" # package_bindir
                              "${ade_LIBRARY_TYPE_RELEASE}"
                              "${ade_IS_HOST_WINDOWS_RELEASE}"
                              ade_DEPS_TARGET
                              ade_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "ade"    # package_name
                              "${ade_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${ade_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${ade_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${ade_LIBRARIES_TARGETS}>
                 )

    if("${ade_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET ade
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     ade_DEPS_TARGET)
    endif()

    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${ade_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${ade_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${ade_LIB_DIRS_RELEASE}>)
    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${ade_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET ade
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${ade_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(ade_LIBRARIES_RELEASE ade)
