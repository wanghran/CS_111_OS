# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.8

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/Rex/Documents/CS111/PJ4C

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/Rex/Documents/CS111/PJ4C/cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles/PJ4C.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/PJ4C.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/PJ4C.dir/flags.make

CMakeFiles/PJ4C.dir/lab4c_tcp.c.o: CMakeFiles/PJ4C.dir/flags.make
CMakeFiles/PJ4C.dir/lab4c_tcp.c.o: ../lab4c_tcp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Rex/Documents/CS111/PJ4C/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/PJ4C.dir/lab4c_tcp.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PJ4C.dir/lab4c_tcp.c.o   -c /Users/Rex/Documents/CS111/PJ4C/lab4c_tcp.c

CMakeFiles/PJ4C.dir/lab4c_tcp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PJ4C.dir/lab4c_tcp.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Rex/Documents/CS111/PJ4C/lab4c_tcp.c > CMakeFiles/PJ4C.dir/lab4c_tcp.c.i

CMakeFiles/PJ4C.dir/lab4c_tcp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PJ4C.dir/lab4c_tcp.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Rex/Documents/CS111/PJ4C/lab4c_tcp.c -o CMakeFiles/PJ4C.dir/lab4c_tcp.c.s

CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.requires:

.PHONY : CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.requires

CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.provides: CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.requires
	$(MAKE) -f CMakeFiles/PJ4C.dir/build.make CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.provides.build
.PHONY : CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.provides

CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.provides.build: CMakeFiles/PJ4C.dir/lab4c_tcp.c.o


CMakeFiles/PJ4C.dir/lab4c_tls.c.o: CMakeFiles/PJ4C.dir/flags.make
CMakeFiles/PJ4C.dir/lab4c_tls.c.o: ../lab4c_tls.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/Rex/Documents/CS111/PJ4C/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object CMakeFiles/PJ4C.dir/lab4c_tls.c.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -o CMakeFiles/PJ4C.dir/lab4c_tls.c.o   -c /Users/Rex/Documents/CS111/PJ4C/lab4c_tls.c

CMakeFiles/PJ4C.dir/lab4c_tls.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/PJ4C.dir/lab4c_tls.c.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /Users/Rex/Documents/CS111/PJ4C/lab4c_tls.c > CMakeFiles/PJ4C.dir/lab4c_tls.c.i

CMakeFiles/PJ4C.dir/lab4c_tls.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/PJ4C.dir/lab4c_tls.c.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /Users/Rex/Documents/CS111/PJ4C/lab4c_tls.c -o CMakeFiles/PJ4C.dir/lab4c_tls.c.s

CMakeFiles/PJ4C.dir/lab4c_tls.c.o.requires:

.PHONY : CMakeFiles/PJ4C.dir/lab4c_tls.c.o.requires

CMakeFiles/PJ4C.dir/lab4c_tls.c.o.provides: CMakeFiles/PJ4C.dir/lab4c_tls.c.o.requires
	$(MAKE) -f CMakeFiles/PJ4C.dir/build.make CMakeFiles/PJ4C.dir/lab4c_tls.c.o.provides.build
.PHONY : CMakeFiles/PJ4C.dir/lab4c_tls.c.o.provides

CMakeFiles/PJ4C.dir/lab4c_tls.c.o.provides.build: CMakeFiles/PJ4C.dir/lab4c_tls.c.o


# Object files for target PJ4C
PJ4C_OBJECTS = \
"CMakeFiles/PJ4C.dir/lab4c_tcp.c.o" \
"CMakeFiles/PJ4C.dir/lab4c_tls.c.o"

# External object files for target PJ4C
PJ4C_EXTERNAL_OBJECTS =

PJ4C: CMakeFiles/PJ4C.dir/lab4c_tcp.c.o
PJ4C: CMakeFiles/PJ4C.dir/lab4c_tls.c.o
PJ4C: CMakeFiles/PJ4C.dir/build.make
PJ4C: CMakeFiles/PJ4C.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/Rex/Documents/CS111/PJ4C/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C executable PJ4C"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/PJ4C.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/PJ4C.dir/build: PJ4C

.PHONY : CMakeFiles/PJ4C.dir/build

CMakeFiles/PJ4C.dir/requires: CMakeFiles/PJ4C.dir/lab4c_tcp.c.o.requires
CMakeFiles/PJ4C.dir/requires: CMakeFiles/PJ4C.dir/lab4c_tls.c.o.requires

.PHONY : CMakeFiles/PJ4C.dir/requires

CMakeFiles/PJ4C.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/PJ4C.dir/cmake_clean.cmake
.PHONY : CMakeFiles/PJ4C.dir/clean

CMakeFiles/PJ4C.dir/depend:
	cd /Users/Rex/Documents/CS111/PJ4C/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/Rex/Documents/CS111/PJ4C /Users/Rex/Documents/CS111/PJ4C /Users/Rex/Documents/CS111/PJ4C/cmake-build-debug /Users/Rex/Documents/CS111/PJ4C/cmake-build-debug /Users/Rex/Documents/CS111/PJ4C/cmake-build-debug/CMakeFiles/PJ4C.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/PJ4C.dir/depend
