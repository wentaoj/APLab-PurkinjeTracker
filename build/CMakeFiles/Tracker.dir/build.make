# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.28

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /app/ddpi-test

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /app/ddpi-test/build

# Include any dependencies generated for this target.
include CMakeFiles/Tracker.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/Tracker.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/Tracker.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/Tracker.dir/flags.make

CMakeFiles/Tracker.dir/src/main.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/main.cpp.o: /app/ddpi-test/src/main.cpp
CMakeFiles/Tracker.dir/src/main.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/Tracker.dir/src/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/main.cpp.o -MF CMakeFiles/Tracker.dir/src/main.cpp.o.d -o CMakeFiles/Tracker.dir/src/main.cpp.o -c /app/ddpi-test/src/main.cpp

CMakeFiles/Tracker.dir/src/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/main.cpp > CMakeFiles/Tracker.dir/src/main.cpp.i

CMakeFiles/Tracker.dir/src/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/main.cpp -o CMakeFiles/Tracker.dir/src/main.cpp.s

CMakeFiles/Tracker.dir/src/image_generator.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/image_generator.cpp.o: /app/ddpi-test/src/image_generator.cpp
CMakeFiles/Tracker.dir/src/image_generator.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/Tracker.dir/src/image_generator.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/image_generator.cpp.o -MF CMakeFiles/Tracker.dir/src/image_generator.cpp.o.d -o CMakeFiles/Tracker.dir/src/image_generator.cpp.o -c /app/ddpi-test/src/image_generator.cpp

CMakeFiles/Tracker.dir/src/image_generator.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/image_generator.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/image_generator.cpp > CMakeFiles/Tracker.dir/src/image_generator.cpp.i

CMakeFiles/Tracker.dir/src/image_generator.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/image_generator.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/image_generator.cpp -o CMakeFiles/Tracker.dir/src/image_generator.cpp.s

CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o: /app/ddpi-test/src/doh_tracker.cpp
CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o -MF CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o.d -o CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o -c /app/ddpi-test/src/doh_tracker.cpp

CMakeFiles/Tracker.dir/src/doh_tracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/doh_tracker.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/doh_tracker.cpp > CMakeFiles/Tracker.dir/src/doh_tracker.cpp.i

CMakeFiles/Tracker.dir/src/doh_tracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/doh_tracker.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/doh_tracker.cpp -o CMakeFiles/Tracker.dir/src/doh_tracker.cpp.s

CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o: /app/ddpi-test/src/kalman_filter.cpp
CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o -MF CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o.d -o CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o -c /app/ddpi-test/src/kalman_filter.cpp

CMakeFiles/Tracker.dir/src/kalman_filter.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/kalman_filter.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/kalman_filter.cpp > CMakeFiles/Tracker.dir/src/kalman_filter.cpp.i

CMakeFiles/Tracker.dir/src/kalman_filter.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/kalman_filter.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/kalman_filter.cpp -o CMakeFiles/Tracker.dir/src/kalman_filter.cpp.s

CMakeFiles/Tracker.dir/src/base_tracker.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/base_tracker.cpp.o: /app/ddpi-test/src/base_tracker.cpp
CMakeFiles/Tracker.dir/src/base_tracker.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/Tracker.dir/src/base_tracker.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/base_tracker.cpp.o -MF CMakeFiles/Tracker.dir/src/base_tracker.cpp.o.d -o CMakeFiles/Tracker.dir/src/base_tracker.cpp.o -c /app/ddpi-test/src/base_tracker.cpp

CMakeFiles/Tracker.dir/src/base_tracker.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/base_tracker.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/base_tracker.cpp > CMakeFiles/Tracker.dir/src/base_tracker.cpp.i

CMakeFiles/Tracker.dir/src/base_tracker.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/base_tracker.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/base_tracker.cpp -o CMakeFiles/Tracker.dir/src/base_tracker.cpp.s

CMakeFiles/Tracker.dir/src/blob.cpp.o: CMakeFiles/Tracker.dir/flags.make
CMakeFiles/Tracker.dir/src/blob.cpp.o: /app/ddpi-test/src/blob.cpp
CMakeFiles/Tracker.dir/src/blob.cpp.o: CMakeFiles/Tracker.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Building CXX object CMakeFiles/Tracker.dir/src/blob.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/Tracker.dir/src/blob.cpp.o -MF CMakeFiles/Tracker.dir/src/blob.cpp.o.d -o CMakeFiles/Tracker.dir/src/blob.cpp.o -c /app/ddpi-test/src/blob.cpp

CMakeFiles/Tracker.dir/src/blob.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/Tracker.dir/src/blob.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /app/ddpi-test/src/blob.cpp > CMakeFiles/Tracker.dir/src/blob.cpp.i

CMakeFiles/Tracker.dir/src/blob.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/Tracker.dir/src/blob.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /app/ddpi-test/src/blob.cpp -o CMakeFiles/Tracker.dir/src/blob.cpp.s

# Object files for target Tracker
Tracker_OBJECTS = \
"CMakeFiles/Tracker.dir/src/main.cpp.o" \
"CMakeFiles/Tracker.dir/src/image_generator.cpp.o" \
"CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o" \
"CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o" \
"CMakeFiles/Tracker.dir/src/base_tracker.cpp.o" \
"CMakeFiles/Tracker.dir/src/blob.cpp.o"

# External object files for target Tracker
Tracker_EXTERNAL_OBJECTS =

Tracker: CMakeFiles/Tracker.dir/src/main.cpp.o
Tracker: CMakeFiles/Tracker.dir/src/image_generator.cpp.o
Tracker: CMakeFiles/Tracker.dir/src/doh_tracker.cpp.o
Tracker: CMakeFiles/Tracker.dir/src/kalman_filter.cpp.o
Tracker: CMakeFiles/Tracker.dir/src/base_tracker.cpp.o
Tracker: CMakeFiles/Tracker.dir/src/blob.cpp.o
Tracker: CMakeFiles/Tracker.dir/build.make
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_stitching.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_alphamat.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_aruco.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_barcode.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_bgsegm.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_bioinspired.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_ccalib.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_cvv.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_dnn_objdetect.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_dnn_superres.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_dpm.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_face.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_freetype.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_fuzzy.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_hdf.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_hfs.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_img_hash.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_intensity_transform.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_line_descriptor.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_mcc.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_quality.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_rapid.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_reg.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_rgbd.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_saliency.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_shape.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_stereo.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_structured_light.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_superres.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_surface_matching.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_tracking.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_videostab.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_viz.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_wechat_qrcode.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_xobjdetect.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_xphoto.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_highgui.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_datasets.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_plot.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_text.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_ml.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_phase_unwrapping.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_optflow.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_ximgproc.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_video.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_videoio.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_imgcodecs.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_objdetect.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_calib3d.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_dnn.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_features2d.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_flann.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_photo.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_imgproc.so.4.6.0
Tracker: /usr/lib/x86_64-linux-gnu/libopencv_core.so.4.6.0
Tracker: CMakeFiles/Tracker.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/app/ddpi-test/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_7) "Linking CXX executable Tracker"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/Tracker.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/Tracker.dir/build: Tracker
.PHONY : CMakeFiles/Tracker.dir/build

CMakeFiles/Tracker.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/Tracker.dir/cmake_clean.cmake
.PHONY : CMakeFiles/Tracker.dir/clean

CMakeFiles/Tracker.dir/depend:
	cd /app/ddpi-test/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /app/ddpi-test /app/ddpi-test /app/ddpi-test/build /app/ddpi-test/build /app/ddpi-test/build/CMakeFiles/Tracker.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/Tracker.dir/depend

