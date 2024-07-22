########### AGGREGATED COMPONENTS AND DEPENDENCIES FOR THE MULTI CONFIG #####################
#############################################################################################

set(dav1d_COMPONENT_NAMES "")
if(DEFINED dav1d_FIND_DEPENDENCY_NAMES)
  list(APPEND dav1d_FIND_DEPENDENCY_NAMES )
  list(REMOVE_DUPLICATES dav1d_FIND_DEPENDENCY_NAMES)
else()
  set(dav1d_FIND_DEPENDENCY_NAMES )
endif()

########### VARIABLES #######################################################################
#############################################################################################
set(dav1d_PACKAGE_FOLDER_RELEASE "/home/wentao/.conan2/p/b/dav1d494c6f34fef9c/p")
set(dav1d_BUILD_MODULES_PATHS_RELEASE )


set(dav1d_INCLUDE_DIRS_RELEASE )
set(dav1d_RES_DIRS_RELEASE )
set(dav1d_DEFINITIONS_RELEASE )
set(dav1d_SHARED_LINK_FLAGS_RELEASE )
set(dav1d_EXE_LINK_FLAGS_RELEASE )
set(dav1d_OBJECTS_RELEASE )
set(dav1d_COMPILE_DEFINITIONS_RELEASE )
set(dav1d_COMPILE_OPTIONS_C_RELEASE )
set(dav1d_COMPILE_OPTIONS_CXX_RELEASE )
set(dav1d_LIB_DIRS_RELEASE "${dav1d_PACKAGE_FOLDER_RELEASE}/lib")
set(dav1d_BIN_DIRS_RELEASE )
set(dav1d_LIBRARY_TYPE_RELEASE STATIC)
set(dav1d_IS_HOST_WINDOWS_RELEASE 0)
set(dav1d_LIBS_RELEASE dav1d)
set(dav1d_SYSTEM_LIBS_RELEASE dl pthread)
set(dav1d_FRAMEWORK_DIRS_RELEASE )
set(dav1d_FRAMEWORKS_RELEASE )
set(dav1d_BUILD_DIRS_RELEASE )
set(dav1d_NO_SONAME_MODE_RELEASE FALSE)


# COMPOUND VARIABLES
set(dav1d_COMPILE_OPTIONS_RELEASE
    "$<$<COMPILE_LANGUAGE:CXX>:${dav1d_COMPILE_OPTIONS_CXX_RELEASE}>"
    "$<$<COMPILE_LANGUAGE:C>:${dav1d_COMPILE_OPTIONS_C_RELEASE}>")
set(dav1d_LINKER_FLAGS_RELEASE
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,SHARED_LIBRARY>:${dav1d_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,MODULE_LIBRARY>:${dav1d_SHARED_LINK_FLAGS_RELEASE}>"
    "$<$<STREQUAL:$<TARGET_PROPERTY:TYPE>,EXECUTABLE>:${dav1d_EXE_LINK_FLAGS_RELEASE}>")


set(dav1d_COMPONENTS_RELEASE )