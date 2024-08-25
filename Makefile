# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Default target executed when no arguments are given to make.
default_target: all
.PHONY : default_target

# Allow only one "make -f Makefile2" at a time, but pass parallelism.
.NOTPARALLEL:

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
CMAKE_SOURCE_DIR = /home/vconterno/CLionProjects/untitled

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/vconterno/CLionProjects/untitled

#=============================================================================
# Targets provided globally by CMake.

# Special rule for the target edit_cache
edit_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "No interactive CMake dialog available..."
	/usr/bin/cmake -E echo No\ interactive\ CMake\ dialog\ available.
.PHONY : edit_cache

# Special rule for the target edit_cache
edit_cache/fast: edit_cache
.PHONY : edit_cache/fast

# Special rule for the target rebuild_cache
rebuild_cache:
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --cyan "Running CMake to regenerate build system..."
	/usr/bin/cmake --regenerate-during-build -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR)
.PHONY : rebuild_cache

# Special rule for the target rebuild_cache
rebuild_cache/fast: rebuild_cache
.PHONY : rebuild_cache/fast

# The main all target
all: cmake_check_build_system
	$(CMAKE_COMMAND) -E cmake_progress_start /home/vconterno/CLionProjects/untitled/CMakeFiles /home/vconterno/CLionProjects/untitled//CMakeFiles/progress.marks
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 all
	$(CMAKE_COMMAND) -E cmake_progress_start /home/vconterno/CLionProjects/untitled/CMakeFiles 0
.PHONY : all

# The main clean target
clean:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 clean
.PHONY : clean

# The main clean target
clean/fast: clean
.PHONY : clean/fast

# Prepare targets for installation.
preinstall: all
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall

# Prepare targets for installation.
preinstall/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 preinstall
.PHONY : preinstall/fast

# clear depends
depend:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 1
.PHONY : depend

#=============================================================================
# Target rules for targets named master

# Build rule for target.
master: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 master
.PHONY : master

# fast build rule for target.
master/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/build
.PHONY : master/fast

#=============================================================================
# Target rules for targets named atomo

# Build rule for target.
atomo: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 atomo
.PHONY : atomo

# fast build rule for target.
atomo/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/build
.PHONY : atomo/fast

#=============================================================================
# Target rules for targets named attivatore

# Build rule for target.
attivatore: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 attivatore
.PHONY : attivatore

# fast build rule for target.
attivatore/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/build
.PHONY : attivatore/fast

#=============================================================================
# Target rules for targets named alimentatore

# Build rule for target.
alimentatore: cmake_check_build_system
	$(MAKE) $(MAKESILENT) -f CMakeFiles/Makefile2 alimentatore
.PHONY : alimentatore

# fast build rule for target.
alimentatore/fast:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/build
.PHONY : alimentatore/fast

src/alimentatore.o: src/alimentatore.c.o
.PHONY : src/alimentatore.o

# target to build an object file
src/alimentatore.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/alimentatore.c.o
.PHONY : src/alimentatore.c.o

src/alimentatore.i: src/alimentatore.c.i
.PHONY : src/alimentatore.i

# target to preprocess a source file
src/alimentatore.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/alimentatore.c.i
.PHONY : src/alimentatore.c.i

src/alimentatore.s: src/alimentatore.c.s
.PHONY : src/alimentatore.s

# target to generate assembly for a file
src/alimentatore.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/alimentatore.c.s
.PHONY : src/alimentatore.c.s

src/atomo.o: src/atomo.c.o
.PHONY : src/atomo.o

# target to build an object file
src/atomo.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/atomo.c.o
.PHONY : src/atomo.c.o

src/atomo.i: src/atomo.c.i
.PHONY : src/atomo.i

# target to preprocess a source file
src/atomo.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/atomo.c.i
.PHONY : src/atomo.c.i

src/atomo.s: src/atomo.c.s
.PHONY : src/atomo.s

# target to generate assembly for a file
src/atomo.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/atomo.c.s
.PHONY : src/atomo.c.s

src/attivatore.o: src/attivatore.c.o
.PHONY : src/attivatore.o

# target to build an object file
src/attivatore.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/attivatore.c.o
.PHONY : src/attivatore.c.o

src/attivatore.i: src/attivatore.c.i
.PHONY : src/attivatore.i

# target to preprocess a source file
src/attivatore.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/attivatore.c.i
.PHONY : src/attivatore.c.i

src/attivatore.s: src/attivatore.c.s
.PHONY : src/attivatore.s

# target to generate assembly for a file
src/attivatore.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/attivatore.c.s
.PHONY : src/attivatore.c.s

src/io.o: src/io.c.o
.PHONY : src/io.o

# target to build an object file
src/io.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/io.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/io.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/io.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/io.c.o
.PHONY : src/io.c.o

src/io.i: src/io.c.i
.PHONY : src/io.i

# target to preprocess a source file
src/io.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/io.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/io.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/io.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/io.c.i
.PHONY : src/io.c.i

src/io.s: src/io.c.s
.PHONY : src/io.s

# target to generate assembly for a file
src/io.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/io.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/io.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/io.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/io.c.s
.PHONY : src/io.c.s

src/master.o: src/master.c.o
.PHONY : src/master.o

# target to build an object file
src/master.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/master.c.o
.PHONY : src/master.c.o

src/master.i: src/master.c.i
.PHONY : src/master.i

# target to preprocess a source file
src/master.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/master.c.i
.PHONY : src/master.c.i

src/master.s: src/master.c.s
.PHONY : src/master.s

# target to generate assembly for a file
src/master.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/master.c.s
.PHONY : src/master.c.s

src/process.o: src/process.c.o
.PHONY : src/process.o

# target to build an object file
src/process.c.o:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/process.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/process.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/process.c.o
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/process.c.o
.PHONY : src/process.c.o

src/process.i: src/process.c.i
.PHONY : src/process.i

# target to preprocess a source file
src/process.c.i:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/process.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/process.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/process.c.i
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/process.c.i
.PHONY : src/process.c.i

src/process.s: src/process.c.s
.PHONY : src/process.s

# target to generate assembly for a file
src/process.c.s:
	$(MAKE) $(MAKESILENT) -f CMakeFiles/master.dir/build.make CMakeFiles/master.dir/src/process.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/atomo.dir/build.make CMakeFiles/atomo.dir/src/process.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/attivatore.dir/build.make CMakeFiles/attivatore.dir/src/process.c.s
	$(MAKE) $(MAKESILENT) -f CMakeFiles/alimentatore.dir/build.make CMakeFiles/alimentatore.dir/src/process.c.s
.PHONY : src/process.c.s

# Help Target
help:
	@echo "The following are some of the valid targets for this Makefile:"
	@echo "... all (the default if no target is provided)"
	@echo "... clean"
	@echo "... depend"
	@echo "... edit_cache"
	@echo "... rebuild_cache"
	@echo "... alimentatore"
	@echo "... atomo"
	@echo "... attivatore"
	@echo "... master"
	@echo "... src/alimentatore.o"
	@echo "... src/alimentatore.i"
	@echo "... src/alimentatore.s"
	@echo "... src/atomo.o"
	@echo "... src/atomo.i"
	@echo "... src/atomo.s"
	@echo "... src/attivatore.o"
	@echo "... src/attivatore.i"
	@echo "... src/attivatore.s"
	@echo "... src/io.o"
	@echo "... src/io.i"
	@echo "... src/io.s"
	@echo "... src/master.o"
	@echo "... src/master.i"
	@echo "... src/master.s"
	@echo "... src/process.o"
	@echo "... src/process.i"
	@echo "... src/process.s"
.PHONY : help



#=============================================================================
# Special targets to cleanup operation of make.

# Special rule to run CMake to check the build system integrity.
# No rule that depends on this can have commands that come from listfiles
# because they might be regenerated.
cmake_check_build_system:
	$(CMAKE_COMMAND) -S$(CMAKE_SOURCE_DIR) -B$(CMAKE_BINARY_DIR) --check-build-system CMakeFiles/Makefile.cmake 0
.PHONY : cmake_check_build_system

