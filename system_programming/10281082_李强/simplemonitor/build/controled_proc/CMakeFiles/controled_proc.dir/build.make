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
include controled_proc/CMakeFiles/controled_proc.dir/depend.make

# Include the progress variables for this target.
include controled_proc/CMakeFiles/controled_proc.dir/progress.make

# Include the compile flags for this target's objects.
include controled_proc/CMakeFiles/controled_proc.dir/flags.make

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o: controled_proc/CMakeFiles/controled_proc.dir/flags.make
controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o: ../controled_proc/controled_proc.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dryland/system_programming/cmake/simplemonitor/build/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/controled_proc.dir/controled_proc.c.o   -c /home/dryland/system_programming/cmake/simplemonitor/controled_proc/controled_proc.c

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/controled_proc.dir/controled_proc.c.i"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/dryland/system_programming/cmake/simplemonitor/controled_proc/controled_proc.c > CMakeFiles/controled_proc.dir/controled_proc.c.i

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/controled_proc.dir/controled_proc.c.s"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/dryland/system_programming/cmake/simplemonitor/controled_proc/controled_proc.c -o CMakeFiles/controled_proc.dir/controled_proc.c.s

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.requires:
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.requires

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.provides: controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.requires
	$(MAKE) -f controled_proc/CMakeFiles/controled_proc.dir/build.make controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.provides.build
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.provides

controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.provides.build: controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o

controled_proc/CMakeFiles/controled_proc.dir/main.c.o: controled_proc/CMakeFiles/controled_proc.dir/flags.make
controled_proc/CMakeFiles/controled_proc.dir/main.c.o: ../controled_proc/main.c
	$(CMAKE_COMMAND) -E cmake_progress_report /home/dryland/system_programming/cmake/simplemonitor/build/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building C object controled_proc/CMakeFiles/controled_proc.dir/main.c.o"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles/controled_proc.dir/main.c.o   -c /home/dryland/system_programming/cmake/simplemonitor/controled_proc/main.c

controled_proc/CMakeFiles/controled_proc.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/controled_proc.dir/main.c.i"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -E /home/dryland/system_programming/cmake/simplemonitor/controled_proc/main.c > CMakeFiles/controled_proc.dir/main.c.i

controled_proc/CMakeFiles/controled_proc.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/controled_proc.dir/main.c.s"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && /usr/bin/cc  $(C_DEFINES) $(C_FLAGS) -S /home/dryland/system_programming/cmake/simplemonitor/controled_proc/main.c -o CMakeFiles/controled_proc.dir/main.c.s

controled_proc/CMakeFiles/controled_proc.dir/main.c.o.requires:
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/main.c.o.requires

controled_proc/CMakeFiles/controled_proc.dir/main.c.o.provides: controled_proc/CMakeFiles/controled_proc.dir/main.c.o.requires
	$(MAKE) -f controled_proc/CMakeFiles/controled_proc.dir/build.make controled_proc/CMakeFiles/controled_proc.dir/main.c.o.provides.build
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/main.c.o.provides

controled_proc/CMakeFiles/controled_proc.dir/main.c.o.provides.build: controled_proc/CMakeFiles/controled_proc.dir/main.c.o

# Object files for target controled_proc
controled_proc_OBJECTS = \
"CMakeFiles/controled_proc.dir/controled_proc.c.o" \
"CMakeFiles/controled_proc.dir/main.c.o"

# External object files for target controled_proc
controled_proc_EXTERNAL_OBJECTS =

bin/controled_proc: controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o
bin/controled_proc: controled_proc/CMakeFiles/controled_proc.dir/main.c.o
bin/controled_proc: controled_proc/CMakeFiles/controled_proc.dir/build.make
bin/controled_proc: simplelog/lib/libsimplelog.so
bin/controled_proc: controled_proc/CMakeFiles/controled_proc.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking C executable ../bin/controled_proc"
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/controled_proc.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
controled_proc/CMakeFiles/controled_proc.dir/build: bin/controled_proc
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/build

controled_proc/CMakeFiles/controled_proc.dir/requires: controled_proc/CMakeFiles/controled_proc.dir/controled_proc.c.o.requires
controled_proc/CMakeFiles/controled_proc.dir/requires: controled_proc/CMakeFiles/controled_proc.dir/main.c.o.requires
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/requires

controled_proc/CMakeFiles/controled_proc.dir/clean:
	cd /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc && $(CMAKE_COMMAND) -P CMakeFiles/controled_proc.dir/cmake_clean.cmake
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/clean

controled_proc/CMakeFiles/controled_proc.dir/depend:
	cd /home/dryland/system_programming/cmake/simplemonitor/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dryland/system_programming/cmake/simplemonitor /home/dryland/system_programming/cmake/simplemonitor/controled_proc /home/dryland/system_programming/cmake/simplemonitor/build /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc /home/dryland/system_programming/cmake/simplemonitor/build/controled_proc/CMakeFiles/controled_proc.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : controled_proc/CMakeFiles/controled_proc.dir/depend

