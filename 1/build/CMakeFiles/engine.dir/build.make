# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

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
CMAKE_SOURCE_DIR = /home/gabriel/Desktop/aulas/CG/CG2024/1

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/gabriel/Desktop/aulas/CG/CG2024/1/build

# Include any dependencies generated for this target.
include CMakeFiles/engine.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/engine.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/engine.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/engine.dir/flags.make

CMakeFiles/engine.dir/main.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/main.cpp.o: ../main.cpp
CMakeFiles/engine.dir/main.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/engine.dir/main.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/main.cpp.o -MF CMakeFiles/engine.dir/main.cpp.o.d -o CMakeFiles/engine.dir/main.cpp.o -c /home/gabriel/Desktop/aulas/CG/CG2024/1/main.cpp

CMakeFiles/engine.dir/main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/main.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabriel/Desktop/aulas/CG/CG2024/1/main.cpp > CMakeFiles/engine.dir/main.cpp.i

CMakeFiles/engine.dir/main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/main.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabriel/Desktop/aulas/CG/CG2024/1/main.cpp -o CMakeFiles/engine.dir/main.cpp.s

CMakeFiles/engine.dir/Reader.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/Reader.cpp.o: ../Reader.cpp
CMakeFiles/engine.dir/Reader.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/engine.dir/Reader.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/Reader.cpp.o -MF CMakeFiles/engine.dir/Reader.cpp.o.d -o CMakeFiles/engine.dir/Reader.cpp.o -c /home/gabriel/Desktop/aulas/CG/CG2024/1/Reader.cpp

CMakeFiles/engine.dir/Reader.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/Reader.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabriel/Desktop/aulas/CG/CG2024/1/Reader.cpp > CMakeFiles/engine.dir/Reader.cpp.i

CMakeFiles/engine.dir/Reader.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/Reader.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabriel/Desktop/aulas/CG/CG2024/1/Reader.cpp -o CMakeFiles/engine.dir/Reader.cpp.s

CMakeFiles/engine.dir/tinyxml.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/tinyxml.cpp.o: ../tinyxml.cpp
CMakeFiles/engine.dir/tinyxml.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object CMakeFiles/engine.dir/tinyxml.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/tinyxml.cpp.o -MF CMakeFiles/engine.dir/tinyxml.cpp.o.d -o CMakeFiles/engine.dir/tinyxml.cpp.o -c /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxml.cpp

CMakeFiles/engine.dir/tinyxml.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/tinyxml.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxml.cpp > CMakeFiles/engine.dir/tinyxml.cpp.i

CMakeFiles/engine.dir/tinyxml.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/tinyxml.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxml.cpp -o CMakeFiles/engine.dir/tinyxml.cpp.s

CMakeFiles/engine.dir/tinystr.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/tinystr.cpp.o: ../tinystr.cpp
CMakeFiles/engine.dir/tinystr.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Building CXX object CMakeFiles/engine.dir/tinystr.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/tinystr.cpp.o -MF CMakeFiles/engine.dir/tinystr.cpp.o.d -o CMakeFiles/engine.dir/tinystr.cpp.o -c /home/gabriel/Desktop/aulas/CG/CG2024/1/tinystr.cpp

CMakeFiles/engine.dir/tinystr.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/tinystr.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabriel/Desktop/aulas/CG/CG2024/1/tinystr.cpp > CMakeFiles/engine.dir/tinystr.cpp.i

CMakeFiles/engine.dir/tinystr.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/tinystr.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabriel/Desktop/aulas/CG/CG2024/1/tinystr.cpp -o CMakeFiles/engine.dir/tinystr.cpp.s

CMakeFiles/engine.dir/tinyxmlerror.cpp.o: CMakeFiles/engine.dir/flags.make
CMakeFiles/engine.dir/tinyxmlerror.cpp.o: ../tinyxmlerror.cpp
CMakeFiles/engine.dir/tinyxmlerror.cpp.o: CMakeFiles/engine.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_5) "Building CXX object CMakeFiles/engine.dir/tinyxmlerror.cpp.o"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/engine.dir/tinyxmlerror.cpp.o -MF CMakeFiles/engine.dir/tinyxmlerror.cpp.o.d -o CMakeFiles/engine.dir/tinyxmlerror.cpp.o -c /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxmlerror.cpp

CMakeFiles/engine.dir/tinyxmlerror.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/engine.dir/tinyxmlerror.cpp.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxmlerror.cpp > CMakeFiles/engine.dir/tinyxmlerror.cpp.i

CMakeFiles/engine.dir/tinyxmlerror.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/engine.dir/tinyxmlerror.cpp.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/gabriel/Desktop/aulas/CG/CG2024/1/tinyxmlerror.cpp -o CMakeFiles/engine.dir/tinyxmlerror.cpp.s

# Object files for target engine
engine_OBJECTS = \
"CMakeFiles/engine.dir/main.cpp.o" \
"CMakeFiles/engine.dir/Reader.cpp.o" \
"CMakeFiles/engine.dir/tinyxml.cpp.o" \
"CMakeFiles/engine.dir/tinystr.cpp.o" \
"CMakeFiles/engine.dir/tinyxmlerror.cpp.o"

# External object files for target engine
engine_EXTERNAL_OBJECTS =

engine: CMakeFiles/engine.dir/main.cpp.o
engine: CMakeFiles/engine.dir/Reader.cpp.o
engine: CMakeFiles/engine.dir/tinyxml.cpp.o
engine: CMakeFiles/engine.dir/tinystr.cpp.o
engine: CMakeFiles/engine.dir/tinyxmlerror.cpp.o
engine: CMakeFiles/engine.dir/build.make
engine: /usr/lib/x86_64-linux-gnu/libGL.so
engine: /usr/lib/x86_64-linux-gnu/libGLU.so
engine: /usr/lib/x86_64-linux-gnu/libglut.so
engine: /usr/lib/x86_64-linux-gnu/libXmu.so
engine: /usr/lib/x86_64-linux-gnu/libXi.so
engine: CMakeFiles/engine.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_6) "Linking CXX executable engine"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/engine.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/engine.dir/build: engine
.PHONY : CMakeFiles/engine.dir/build

CMakeFiles/engine.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/engine.dir/cmake_clean.cmake
.PHONY : CMakeFiles/engine.dir/clean

CMakeFiles/engine.dir/depend:
	cd /home/gabriel/Desktop/aulas/CG/CG2024/1/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/gabriel/Desktop/aulas/CG/CG2024/1 /home/gabriel/Desktop/aulas/CG/CG2024/1 /home/gabriel/Desktop/aulas/CG/CG2024/1/build /home/gabriel/Desktop/aulas/CG/CG2024/1/build /home/gabriel/Desktop/aulas/CG/CG2024/1/build/CMakeFiles/engine.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/engine.dir/depend

