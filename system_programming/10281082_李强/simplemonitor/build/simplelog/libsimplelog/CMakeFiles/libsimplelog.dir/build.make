# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 2.8

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

# The program to use to edit the cache.
CMAKE_EDIT_COMMAND = /usr/bin/ccmake

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dryland/system_programming/cmake/simplemonitor

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dryland/system_programming/cmake/simplemonitor/build

# Include any dependencies generated for this target.
include simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/depend.make

# Include the progress variables for this target.
include simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/progress.make

# Include the compile flags for this target's objects.
include simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/flags.make

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/flags.make
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o: ../simplelog/libsimplelog/libsimplelog.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dryland/system_programming/cmake/simplemonitor/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/libsimplelog.dir/libsimplelog.c.o   -c /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/libsimplelog.c

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/libsimplelog.dir/libsimplelog.c.i"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/libsimplelog.c > CMakeFiles/libsimplelog.dir/libsimplelog.c.i

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/libsimplelog.dir/libsimplelog.c.s"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/libsimplelog.c -o CMakeFiles/libsimplelog.dir/libsimplelog.c.s

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.requires:
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.requires

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.provides: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.requires
	$(MAKE) -f simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build.make simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.provides.build
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.provides

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.provides.build: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/flags.make
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o: ../simplelog/libsimplelog/log_func.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dryland/system_programming/cmake/simplemonitor/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/libsimplelog.dir/log_func.c.o   -c /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/log_func.c

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/libsimplelog.dir/log_func.c.i"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/log_func.c > CMakeFiles/libsimplelog.dir/log_func.c.i

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/libsimplelog.dir/log_func.c.s"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/log_func.c -o CMakeFiles/libsimplelog.dir/log_func.c.s

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.requires:
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.requires

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.provides: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.requires
	$(MAKE) -f simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build.make simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.provides.build
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.provides

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.provides.build: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/flags.make
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o: ../simplelog/libsimplelog/index_func.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dryland/system_programming/cmake/simplemonitor/build/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/libsimplelog.dir/index_func.c.o   -c /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/index_func.c

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/libsimplelog.dir/index_func.c.i"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/index_func.c > CMakeFiles/libsimplelog.dir/index_func.c.i

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/libsimplelog.dir/index_func.c.s"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog/index_func.c -o CMakeFiles/libsimplelog.dir/index_func.c.s

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.requires:
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.requires

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.provides: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.requires
	$(MAKE) -f simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build.make simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.provides.build
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.provides

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.provides.build: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o

# Object files for target libsimplelog
libsimplelog_OBJECTS = \
"CMakeFiles/libsimplelog.dir/libsimplelog.c.o" \
"CMakeFiles/libsimplelog.dir/log_func.c.o" \
"CMakeFiles/libsimplelog.dir/index_func.c.o"

# External object files for target libsimplelog
libsimplelog_EXTERNAL_OBJECTS =

simplelog/lib/libsimplelog.so: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o
simplelog/lib/libsimplelog.so: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o
simplelog/lib/libsimplelog.so: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o
simplelog/lib/libsimplelog.so: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build.make
simplelog/lib/libsimplelog.so: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C shared library ../lib/libsimplelog.so"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/libsimplelog.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build: simplelog/lib/libsimplelog.so
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/build

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/requires: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/libsimplelog.c.o.requires
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/requires: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/log_func.c.o.requires
simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/requires: simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/index_func.c.o.requires
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/requires

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/clean:
	cd /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog && $(CMAKE_COMMAND) -P CMakeFiles/libsimplelog.dir/cmake_clean.cmake
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/clean

simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/depend:
	cd /home/dryland/system_programming/cmake/simplemonitor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dryland/system_programming/cmake/simplemonitor /home/dryland/system_programming/cmake/simplemonitor/simplelog/libsimplelog /home/dryland/system_programming/cmake/simplemonitor/build /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog /home/dryland/system_programming/cmake/simplemonitor/build/simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : simplelog/libsimplelog/CMakeFiles/libsimplelog.dir/depend

