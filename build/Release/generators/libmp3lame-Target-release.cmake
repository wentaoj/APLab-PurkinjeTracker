# Avoid multiple calls to find_package to append duplicated properties to the targets
include_guard()########### VARIABLES #######################################################################
#############################################################################################
set(libmp3lame_FRAMEWORKS_FOUND_RELEASE "") # Will be filled later
conan_find_apple_frameworks(libmp3lame_FRAMEWORKS_FOUND_RELEASE "${libmp3lame_FRAMEWORKS_RELEASE}" "${libmp3lame_FRAMEWORK_DIRS_RELEASE}")

set(libmp3lame_LIBRARIES_TARGETS "") # Will be filled later


######## Create an interface target to contain all the dependencies (frameworks, system and conan deps)
if(NOT TARGET libmp3lame_DEPS_TARGET)
    add_library(libmp3lame_DEPS_TARGET INTERFACE IMPORTED)
endif()

set_property(TARGET libmp3lame_DEPS_TARGET
             APPEND PROPERTY INTERFACE_LINK_LIBRARIES
             $<$<CONFIG:Release>:${libmp3lame_FRAMEWORKS_FOUND_RELEASE}>
             $<$<CONFIG:Release>:${libmp3lame_SYSTEM_LIBS_RELEASE}>
             $<$<CONFIG:Release>:>)

####### Find the libraries declared in cpp_info.libs, create an IMPORTED target for each one and link the
####### libmp3lame_DEPS_TARGET to all of them
conan_package_library_targets("${libmp3lame_LIBS_RELEASE}"    # libraries
                              "${libmp3lame_LIB_DIRS_RELEASE}" # package_libdir
                              "${libmp3lame_BIN_DIRS_RELEASE}" # package_bindir
                              "${libmp3lame_LIBRARY_TYPE_RELEASE}"
                              "${libmp3lame_IS_HOST_WINDOWS_RELEASE}"
                              libmp3lame_DEPS_TARGET
                              libmp3lame_LIBRARIES_TARGETS  # out_libraries_targets
                              "_RELEASE"
                              "libmp3lame"    # package_name
                              "${libmp3lame_NO_SONAME_MODE_RELEASE}")  # soname

# FIXME: What is the result of this for multi-config? All configs adding themselves to path?
set(CMAKE_MODULE_PATH ${libmp3lame_BUILD_DIRS_RELEASE} ${CMAKE_MODULE_PATH})

########## GLOBAL TARGET PROPERTIES Release ########################################
    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                 $<$<CONFIG:Release>:${libmp3lame_OBJECTS_RELEASE}>
                 $<$<CONFIG:Release>:${libmp3lame_LIBRARIES_TARGETS}>
                 )

    if("${libmp3lame_LIBS_RELEASE}" STREQUAL "")
        # If the package is not declaring any "cpp_info.libs" the package deps, system libs,
        # frameworks etc are not linked to the imported targets and we need to do it to the
        # global target
        set_property(TARGET libmp3lame::libmp3lame
                     APPEND PROPERTY INTERFACE_LINK_LIBRARIES
                     libmp3lame_DEPS_TARGET)
    endif()

    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_LINK_OPTIONS
                 $<$<CONFIG:Release>:${libmp3lame_LINKER_FLAGS_RELEASE}>)
    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_INCLUDE_DIRECTORIES
                 $<$<CONFIG:Release>:${libmp3lame_INCLUDE_DIRS_RELEASE}>)
    # Necessary to find LINK shared libraries in Linux
    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_LINK_DIRECTORIES
                 $<$<CONFIG:Release>:${libmp3lame_LIB_DIRS_RELEASE}>)
    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_COMPILE_DEFINITIONS
                 $<$<CONFIG:Release>:${libmp3lame_COMPILE_DEFINITIONS_RELEASE}>)
    set_property(TARGET libmp3lame::libmp3lame
                 APPEND PROPERTY INTERFACE_COMPILE_OPTIONS
                 $<$<CONFIG:Release>:${libmp3lame_COMPILE_OPTIONS_RELEASE}>)

########## For the modules (FindXXX)
set(libmp3lame_LIBRARIES_RELEASE libmp3lame::libmp3lame)
