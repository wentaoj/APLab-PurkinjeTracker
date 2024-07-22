

# Conan automatically generated toolchain file
# DO NOT EDIT MANUALLY, it will be overwritten

# Avoid including toolchain file several times (bad if appending to variables like
#   CMAKE_CXX_FLAGS. See https://github.com/android/ndk/issues/323
include_guard()

message(STATUS "Using Conan toolchain: ${CMAKE_CURRENT_LIST_FILE}")

if(${CMAKE_VERSION} VERSION_LESS "3.15")
    message(FATAL_ERROR "The 'CMakeToolchain' generator only works with CMake >= 3.15")
endif()




########## generic_system block #############
# Definition of system, platform and toolset
#############################################







string(APPEND CONAN_CXX_FLAGS " -m64")
string(APPEND CONAN_C_FLAGS " -m64")
string(APPEND CONAN_SHARED_LINKER_FLAGS " -m64")
string(APPEND CONAN_EXE_LINKER_FLAGS " -m64")



message(STATUS "Conan toolchain: C++ Standard 17 with extensions ON")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_EXTENSIONS ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)


# Conan conf flags start: 
# Conan conf flags end

foreach(config IN LISTS CMAKE_CONFIGURATION_TYPES)
    string(TOUPPER ${config} config)
    if(DEFINED CONAN_CXX_FLAGS_${config})
      string(APPEND CMAKE_CXX_FLAGS_${config}_INIT " ${CONAN_CXX_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_C_FLAGS_${config})
      string(APPEND CMAKE_C_FLAGS_${config}_INIT " ${CONAN_C_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_SHARED_LINKER_FLAGS_${config})
      string(APPEND CMAKE_SHARED_LINKER_FLAGS_${config}_INIT " ${CONAN_SHARED_LINKER_FLAGS_${config}}")
    endif()
    if(DEFINED CONAN_EXE_LINKER_FLAGS_${config})
      string(APPEND CMAKE_EXE_LINKER_FLAGS_${config}_INIT " ${CONAN_EXE_LINKER_FLAGS_${config}}")
    endif()
endforeach()

if(DEFINED CONAN_CXX_FLAGS)
  string(APPEND CMAKE_CXX_FLAGS_INIT " ${CONAN_CXX_FLAGS}")
endif()
if(DEFINED CONAN_C_FLAGS)
  string(APPEND CMAKE_C_FLAGS_INIT " ${CONAN_C_FLAGS}")
endif()
if(DEFINED CONAN_SHARED_LINKER_FLAGS)
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${CONAN_SHARED_LINKER_FLAGS}")
endif()
if(DEFINED CONAN_EXE_LINKER_FLAGS)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${CONAN_EXE_LINKER_FLAGS}")
endif()




get_property( _CMAKE_IN_TRY_COMPILE GLOBAL PROPERTY IN_TRY_COMPILE )
if(_CMAKE_IN_TRY_COMPILE)
    message(STATUS "Running toolchain IN_TRY_COMPILE")
    return()
endif()

set(CMAKE_FIND_PACKAGE_PREFER_CONFIG ON)

# Definition of CMAKE_MODULE_PATH
list(PREPEND CMAKE_MODULE_PATH "/home/wentao/.conan2/p/b/proto04955f0659aa2/p/lib/cmake/protobuf" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/lib/cmake" "/home/wentao/.conan2/p/b/opens2a30e2d8e3bdb/p/lib/cmake")
# the generators folder (where conan generates files, like this toolchain)
list(PREPEND CMAKE_MODULE_PATH ${CMAKE_CURRENT_LIST_DIR})

# Definition of CMAKE_PREFIX_PATH, CMAKE_XXXXX_PATH
# The explicitly defined "builddirs" of "host" context dependencies must be in PREFIX_PATH
list(PREPEND CMAKE_PREFIX_PATH "/home/wentao/.conan2/p/b/proto04955f0659aa2/p/lib/cmake/protobuf" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/lib/cmake" "/home/wentao/.conan2/p/b/opens2a30e2d8e3bdb/p/lib/cmake")
# The Conan local "generators" folder, where this toolchain is saved.
list(PREPEND CMAKE_PREFIX_PATH ${CMAKE_CURRENT_LIST_DIR} )
list(PREPEND CMAKE_LIBRARY_PATH "/home/wentao/.conan2/p/b/opencef0ede5564eba/p/lib" "/home/wentao/.conan2/p/b/proto04955f0659aa2/p/lib" "/home/wentao/.conan2/p/b/ade2d6af454fa36d/p/lib" "/home/wentao/.conan2/p/b/xkbcodb72b747ff572/p/lib" "/home/wentao/.conan2/p/b/wayla63f46a079d43c/p/lib" "/home/wentao/.conan2/p/b/libff58b6d0a5251bb/p/lib" "/home/wentao/.conan2/p/b/libxme2ba1ce540529/p/lib" "/home/wentao/.conan2/p/b/expat1079ef2ef9498/p/lib" "/home/wentao/.conan2/p/b/jaspeaa201dc966e4f/p/lib" "/home/wentao/.conan2/p/b/opene7a47ea5e7223a/p/lib" "/home/wentao/.conan2/p/b/imath204d6bdf8a46d/p/lib" "/home/wentao/.conan2/p/b/libtid8eb9fb29ce73/p/lib" "/home/wentao/.conan2/p/b/libde4ed63edd71961/p/lib" "/home/wentao/.conan2/p/b/libjp19dceaea7aa48/p/lib" "/home/wentao/.conan2/p/b/jbig50ab8a7f5f611/p/lib" "/home/wentao/.conan2/p/b/zstd32801e8cc7ff9/p/lib" "/home/wentao/.conan2/p/b/quirc172a3984fbf87/p/lib" "/home/wentao/.conan2/p/b/ffmpe3412662b3faf0/p/lib" "/home/wentao/.conan2/p/b/xz_ut6f1b0919804f1/p/lib" "/home/wentao/.conan2/p/b/freetc090b1a6c8a28/p/lib" "/home/wentao/.conan2/p/b/libpn4a171247cce27/p/lib" "/home/wentao/.conan2/p/b/bzip2ffc183d0478e4/p/lib" "/home/wentao/.conan2/p/b/brotl22fafd4982864/p/lib" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/lib" "/home/wentao/.conan2/p/b/openh4eaa5b5657f48/p/lib" "/home/wentao/.conan2/p/b/libx2126a43f17c0f6/p/lib" "/home/wentao/.conan2/p/b/libx29eae6b460e84a/p/lib" "/home/wentao/.conan2/p/b/libvpd975b45b31d70/p/lib" "/home/wentao/.conan2/p/b/libfdf172d16af2d3d/p/lib" "/home/wentao/.conan2/p/b/libwe6242b59c3abb8/p/lib" "/home/wentao/.conan2/p/b/pulseb75759b671702/p/lib" "/home/wentao/.conan2/p/b/pulseb75759b671702/p/lib/pulseaudio" "/home/wentao/.conan2/p/b/libicc1751b1223acc/p/lib" "/home/wentao/.conan2/p/b/libsn3c49ae9cf1c64/p/lib" "/home/wentao/.conan2/p/b/vorbiba5b3669ed701/p/lib" "/home/wentao/.conan2/p/b/flaca5717ffd9122e/p/lib" "/home/wentao/.conan2/p/b/ogg5736bd7acbf3a/p/lib" "/home/wentao/.conan2/p/b/opus07c60ebf40ca2/p/lib" "/home/wentao/.conan2/p/b/mpg12b0de0314af319/p/lib" "/home/wentao/.conan2/p/b/libmpe75bb90a7d442/p/lib" "/home/wentao/.conan2/p/b/libcac88a32c48ae01/p/lib" "/home/wentao/.conan2/p/b/libto46cac857f04b6/p/lib" "/home/wentao/.conan2/p/b/libalff0ae2edd1037/p/lib" "/home/wentao/.conan2/p/b/opens2a30e2d8e3bdb/p/lib" "/home/wentao/.conan2/p/b/zliba99c0558d0fc4/p/lib" "/home/wentao/.conan2/p/b/libsve0d52024081fe/p/lib" "/home/wentao/.conan2/p/b/cpuin218e8bcc611cf/p/lib" "/home/wentao/.conan2/p/b/libaoe1459339de328/p/lib" "/home/wentao/.conan2/p/b/dav1d494c6f34fef9c/p/lib")
list(PREPEND CMAKE_INCLUDE_PATH "/home/wentao/.conan2/p/b/opencef0ede5564eba/p/include" "/home/wentao/.conan2/p/b/opencef0ede5564eba/p/include/opencv4" "/home/wentao/.conan2/p/eigen3d88c0279cc26/p/include/eigen3" "/home/wentao/.conan2/p/b/proto04955f0659aa2/p/include" "/home/wentao/.conan2/p/b/ade2d6af454fa36d/p/include" "/home/wentao/.conan2/p/b/xkbcodb72b747ff572/p/include" "/home/wentao/.conan2/p/b/wayla63f46a079d43c/p/include" "/home/wentao/.conan2/p/b/libff58b6d0a5251bb/p/include" "/home/wentao/.conan2/p/b/libxme2ba1ce540529/p/include" "/home/wentao/.conan2/p/b/libxme2ba1ce540529/p/include/libxml2" "/home/wentao/.conan2/p/b/expat1079ef2ef9498/p/include" "/home/wentao/.conan2/p/b/jaspeaa201dc966e4f/p/include" "/home/wentao/.conan2/p/b/opene7a47ea5e7223a/p/include" "/home/wentao/.conan2/p/b/opene7a47ea5e7223a/p/include/OpenEXR" "/home/wentao/.conan2/p/b/imath204d6bdf8a46d/p/include" "/home/wentao/.conan2/p/b/imath204d6bdf8a46d/p/include/Imath" "/home/wentao/.conan2/p/b/libtid8eb9fb29ce73/p/include" "/home/wentao/.conan2/p/b/libde4ed63edd71961/p/include" "/home/wentao/.conan2/p/b/libjp19dceaea7aa48/p/include" "/home/wentao/.conan2/p/b/jbig50ab8a7f5f611/p/include" "/home/wentao/.conan2/p/b/zstd32801e8cc7ff9/p/include" "/home/wentao/.conan2/p/b/quirc172a3984fbf87/p/include" "/home/wentao/.conan2/p/b/ffmpe3412662b3faf0/p/include" "/home/wentao/.conan2/p/b/xz_ut6f1b0919804f1/p/include" "/home/wentao/.conan2/p/b/freetc090b1a6c8a28/p/include" "/home/wentao/.conan2/p/b/freetc090b1a6c8a28/p/include/freetype2" "/home/wentao/.conan2/p/b/libpn4a171247cce27/p/include" "/home/wentao/.conan2/p/b/bzip2ffc183d0478e4/p/include" "/home/wentao/.conan2/p/b/brotl22fafd4982864/p/include" "/home/wentao/.conan2/p/b/brotl22fafd4982864/p/include/brotli" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/include" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/include/openjpeg-2.5" "/home/wentao/.conan2/p/b/openh4eaa5b5657f48/p/include" "/home/wentao/.conan2/p/b/libx2126a43f17c0f6/p/include" "/home/wentao/.conan2/p/b/libx29eae6b460e84a/p/include" "/home/wentao/.conan2/p/b/libvpd975b45b31d70/p/include" "/home/wentao/.conan2/p/b/libfdf172d16af2d3d/p/include" "/home/wentao/.conan2/p/b/libwe6242b59c3abb8/p/include" "/home/wentao/.conan2/p/b/pulseb75759b671702/p/include" "/home/wentao/.conan2/p/b/libicc1751b1223acc/p/include" "/home/wentao/.conan2/p/b/libsn3c49ae9cf1c64/p/include" "/home/wentao/.conan2/p/b/vorbiba5b3669ed701/p/include" "/home/wentao/.conan2/p/b/flaca5717ffd9122e/p/include" "/home/wentao/.conan2/p/b/ogg5736bd7acbf3a/p/include" "/home/wentao/.conan2/p/b/opus07c60ebf40ca2/p/include" "/home/wentao/.conan2/p/b/opus07c60ebf40ca2/p/include/opus" "/home/wentao/.conan2/p/b/mpg12b0de0314af319/p/include" "/home/wentao/.conan2/p/b/libmpe75bb90a7d442/p/include" "/home/wentao/.conan2/p/b/libcac88a32c48ae01/p/include" "/home/wentao/.conan2/p/b/libto46cac857f04b6/p/include" "/home/wentao/.conan2/p/b/libalff0ae2edd1037/p/include" "/home/wentao/.conan2/p/b/opens2a30e2d8e3bdb/p/include" "/home/wentao/.conan2/p/b/zliba99c0558d0fc4/p/include" "/home/wentao/.conan2/p/b/libsve0d52024081fe/p/include/svt-av1" "/home/wentao/.conan2/p/b/cpuin218e8bcc611cf/p/include" "/home/wentao/.conan2/p/b/libaoe1459339de328/p/include" "/home/wentao/.conan2/p/b/dav1d494c6f34fef9c/p/include")
set(CONAN_RUNTIME_LIB_DIRS "/home/wentao/.conan2/p/b/opencef0ede5564eba/p/lib" "/home/wentao/.conan2/p/b/proto04955f0659aa2/p/lib" "/home/wentao/.conan2/p/b/ade2d6af454fa36d/p/lib" "/home/wentao/.conan2/p/b/xkbcodb72b747ff572/p/lib" "/home/wentao/.conan2/p/b/wayla63f46a079d43c/p/lib" "/home/wentao/.conan2/p/b/libff58b6d0a5251bb/p/lib" "/home/wentao/.conan2/p/b/libxme2ba1ce540529/p/lib" "/home/wentao/.conan2/p/b/expat1079ef2ef9498/p/lib" "/home/wentao/.conan2/p/b/jaspeaa201dc966e4f/p/lib" "/home/wentao/.conan2/p/b/opene7a47ea5e7223a/p/lib" "/home/wentao/.conan2/p/b/imath204d6bdf8a46d/p/lib" "/home/wentao/.conan2/p/b/libtid8eb9fb29ce73/p/lib" "/home/wentao/.conan2/p/b/libde4ed63edd71961/p/lib" "/home/wentao/.conan2/p/b/libjp19dceaea7aa48/p/lib" "/home/wentao/.conan2/p/b/jbig50ab8a7f5f611/p/lib" "/home/wentao/.conan2/p/b/zstd32801e8cc7ff9/p/lib" "/home/wentao/.conan2/p/b/quirc172a3984fbf87/p/lib" "/home/wentao/.conan2/p/b/ffmpe3412662b3faf0/p/lib" "/home/wentao/.conan2/p/b/xz_ut6f1b0919804f1/p/lib" "/home/wentao/.conan2/p/b/freetc090b1a6c8a28/p/lib" "/home/wentao/.conan2/p/b/libpn4a171247cce27/p/lib" "/home/wentao/.conan2/p/b/bzip2ffc183d0478e4/p/lib" "/home/wentao/.conan2/p/b/brotl22fafd4982864/p/lib" "/home/wentao/.conan2/p/b/openjfb2dc5694794c/p/lib" "/home/wentao/.conan2/p/b/openh4eaa5b5657f48/p/lib" "/home/wentao/.conan2/p/b/libx2126a43f17c0f6/p/lib" "/home/wentao/.conan2/p/b/libx29eae6b460e84a/p/lib" "/home/wentao/.conan2/p/b/libvpd975b45b31d70/p/lib" "/home/wentao/.conan2/p/b/libfdf172d16af2d3d/p/lib" "/home/wentao/.conan2/p/b/libwe6242b59c3abb8/p/lib" "/home/wentao/.conan2/p/b/pulseb75759b671702/p/lib" "/home/wentao/.conan2/p/b/pulseb75759b671702/p/lib/pulseaudio" "/home/wentao/.conan2/p/b/libicc1751b1223acc/p/lib" "/home/wentao/.conan2/p/b/libsn3c49ae9cf1c64/p/lib" "/home/wentao/.conan2/p/b/vorbiba5b3669ed701/p/lib" "/home/wentao/.conan2/p/b/flaca5717ffd9122e/p/lib" "/home/wentao/.conan2/p/b/ogg5736bd7acbf3a/p/lib" "/home/wentao/.conan2/p/b/opus07c60ebf40ca2/p/lib" "/home/wentao/.conan2/p/b/mpg12b0de0314af319/p/lib" "/home/wentao/.conan2/p/b/libmpe75bb90a7d442/p/lib" "/home/wentao/.conan2/p/b/libcac88a32c48ae01/p/lib" "/home/wentao/.conan2/p/b/libto46cac857f04b6/p/lib" "/home/wentao/.conan2/p/b/libalff0ae2edd1037/p/lib" "/home/wentao/.conan2/p/b/opens2a30e2d8e3bdb/p/lib" "/home/wentao/.conan2/p/b/zliba99c0558d0fc4/p/lib" "/home/wentao/.conan2/p/b/libsve0d52024081fe/p/lib" "/home/wentao/.conan2/p/b/cpuin218e8bcc611cf/p/lib" "/home/wentao/.conan2/p/b/libaoe1459339de328/p/lib" "/home/wentao/.conan2/p/b/dav1d494c6f34fef9c/p/lib" )



if (DEFINED ENV{PKG_CONFIG_PATH})
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR}:$ENV{PKG_CONFIG_PATH}")
else()
set(ENV{PKG_CONFIG_PATH} "${CMAKE_CURRENT_LIST_DIR}:")
endif()




set(CMAKE_INSTALL_BINDIR "bin")
set(CMAKE_INSTALL_SBINDIR "bin")
set(CMAKE_INSTALL_LIBEXECDIR "bin")
set(CMAKE_INSTALL_LIBDIR "lib")
set(CMAKE_INSTALL_INCLUDEDIR "include")
set(CMAKE_INSTALL_OLDINCLUDEDIR "include")


# Variables
# Variables  per configuration


# Preprocessor definitions
# Preprocessor definitions per configuration


if(CMAKE_POLICY_DEFAULT_CMP0091)  # Avoid unused and not-initialized warnings
endif()
