# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.6

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
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
RM = /usr/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/pi/Documents/GP

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/pi/Documents/GP

# Include any dependencies generated for this target.
include CMakeFiles/master.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/master.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/master.dir/flags.make

CMakeFiles/master.dir/Master.cpp.o: CMakeFiles/master.dir/flags.make
CMakeFiles/master.dir/Master.cpp.o: Master.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/pi/Documents/GP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/master.dir/Master.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/master.dir/Master.cpp.o -c /home/pi/Documents/GP/Master.cpp

CMakeFiles/master.dir/Master.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/master.dir/Master.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/pi/Documents/GP/Master.cpp > CMakeFiles/master.dir/Master.cpp.i

CMakeFiles/master.dir/Master.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/master.dir/Master.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/pi/Documents/GP/Master.cpp -o CMakeFiles/master.dir/Master.cpp.s

CMakeFiles/master.dir/Master.cpp.o.requires:

.PHONY : CMakeFiles/master.dir/Master.cpp.o.requires

CMakeFiles/master.dir/Master.cpp.o.provides: CMakeFiles/master.dir/Master.cpp.o.requires
	$(MAKE) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/Master.cpp.o.provides.build
.PHONY : CMakeFiles/master.dir/Master.cpp.o.provides

CMakeFiles/master.dir/Master.cpp.o.provides.build: CMakeFiles/master.dir/Master.cpp.o


# Object files for target master
master_OBJECTS = \
"CMakeFiles/master.dir/Master.cpp.o"

# External object files for target master
master_EXTERNAL_OBJECTS =

master: CMakeFiles/master.dir/Master.cpp.o
master: CMakeFiles/master.dir/build.make
master: /usr/local/lib/libopencv_dnn.so.3.2.0
master: /usr/local/lib/libopencv_ml.so.3.2.0
master: /usr/local/lib/libopencv_objdetect.so.3.2.0
master: /usr/local/lib/libopencv_shape.so.3.2.0
master: /usr/local/lib/libopencv_stitching.so.3.2.0
master: /usr/local/lib/libopencv_superres.so.3.2.0
master: /usr/local/lib/libopencv_videostab.so.3.2.0
master: /usr/local/lib/libopencv_calib3d.so.3.2.0
master: /usr/local/lib/libopencv_features2d.so.3.2.0
master: /usr/local/lib/libopencv_flann.so.3.2.0
master: /usr/local/lib/libopencv_highgui.so.3.2.0
master: /usr/local/lib/libopencv_photo.so.3.2.0
master: /usr/local/lib/libopencv_video.so.3.2.0
master: /usr/local/lib/libopencv_videoio.so.3.2.0
master: /usr/local/lib/libopencv_imgcodecs.so.3.2.0
master: /usr/local/lib/libopencv_imgproc.so.3.2.0
master: /usr/local/lib/libopencv_core.so.3.2.0
master: CMakeFiles/master.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/pi/Documents/GP/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable master"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/master.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/master.dir/build: master

.PHONY : CMakeFiles/master.dir/build

CMakeFiles/master.dir/requires: CMakeFiles/master.dir/Master.cpp.o.requires

.PHONY : CMakeFiles/master.dir/requires

CMakeFiles/master.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/master.dir/cmake_clean.cmake
.PHONY : CMakeFiles/master.dir/clean

CMakeFiles/master.dir/depend:
	cd /home/pi/Documents/GP && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/pi/Documents/GP /home/pi/Documents/GP /home/pi/Documents/GP /home/pi/Documents/GP /home/pi/Documents/GP/CMakeFiles/master.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/master.dir/depend

