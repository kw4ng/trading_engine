# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake

# The command to remove a file.
RM = /Applications/CLion.app/Contents/bin/cmake/mac/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/kevinwang/Documents/projects/trading_engine

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug

# Include any dependencies generated for this target.
include networking/CMakeFiles/networking.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include networking/CMakeFiles/networking.dir/compiler_depend.make

# Include the progress variables for this target.
include networking/CMakeFiles/networking.dir/progress.make

# Include the compile flags for this target's objects.
include networking/CMakeFiles/networking.dir/flags.make

networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o: networking/CMakeFiles/networking.dir/flags.make
networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o: ../networking/src/client/tcp_client.cpp
networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o: networking/CMakeFiles/networking.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o -MF CMakeFiles/networking.dir/src/client/tcp_client.cpp.o.d -o CMakeFiles/networking.dir/src/client/tcp_client.cpp.o -c /Users/kevinwang/Documents/projects/trading_engine/networking/src/client/tcp_client.cpp

networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/networking.dir/src/client/tcp_client.cpp.i"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kevinwang/Documents/projects/trading_engine/networking/src/client/tcp_client.cpp > CMakeFiles/networking.dir/src/client/tcp_client.cpp.i

networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/networking.dir/src/client/tcp_client.cpp.s"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kevinwang/Documents/projects/trading_engine/networking/src/client/tcp_client.cpp -o CMakeFiles/networking.dir/src/client/tcp_client.cpp.s

networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o: networking/CMakeFiles/networking.dir/flags.make
networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o: ../networking/src/server/tcp_connection.cpp
networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o: networking/CMakeFiles/networking.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o -MF CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o.d -o CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o -c /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_connection.cpp

networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/networking.dir/src/server/tcp_connection.cpp.i"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_connection.cpp > CMakeFiles/networking.dir/src/server/tcp_connection.cpp.i

networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/networking.dir/src/server/tcp_connection.cpp.s"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_connection.cpp -o CMakeFiles/networking.dir/src/server/tcp_connection.cpp.s

networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o: networking/CMakeFiles/networking.dir/flags.make
networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o: ../networking/src/server/tcp_server.cpp
networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o: networking/CMakeFiles/networking.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Building CXX object networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o -MF CMakeFiles/networking.dir/src/server/tcp_server.cpp.o.d -o CMakeFiles/networking.dir/src/server/tcp_server.cpp.o -c /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_server.cpp

networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/networking.dir/src/server/tcp_server.cpp.i"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_server.cpp > CMakeFiles/networking.dir/src/server/tcp_server.cpp.i

networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/networking.dir/src/server/tcp_server.cpp.s"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/kevinwang/Documents/projects/trading_engine/networking/src/server/tcp_server.cpp -o CMakeFiles/networking.dir/src/server/tcp_server.cpp.s

# Object files for target networking
networking_OBJECTS = \
"CMakeFiles/networking.dir/src/client/tcp_client.cpp.o" \
"CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o" \
"CMakeFiles/networking.dir/src/server/tcp_server.cpp.o"

# External object files for target networking
networking_EXTERNAL_OBJECTS =

networking/libnetworking.a: networking/CMakeFiles/networking.dir/src/client/tcp_client.cpp.o
networking/libnetworking.a: networking/CMakeFiles/networking.dir/src/server/tcp_connection.cpp.o
networking/libnetworking.a: networking/CMakeFiles/networking.dir/src/server/tcp_server.cpp.o
networking/libnetworking.a: networking/CMakeFiles/networking.dir/build.make
networking/libnetworking.a: networking/CMakeFiles/networking.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/CMakeFiles --progress-num=$(CMAKE_PROGRESS_4) "Linking CXX static library libnetworking.a"
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && $(CMAKE_COMMAND) -P CMakeFiles/networking.dir/cmake_clean_target.cmake
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/networking.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
networking/CMakeFiles/networking.dir/build: networking/libnetworking.a
.PHONY : networking/CMakeFiles/networking.dir/build

networking/CMakeFiles/networking.dir/clean:
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking && $(CMAKE_COMMAND) -P CMakeFiles/networking.dir/cmake_clean.cmake
.PHONY : networking/CMakeFiles/networking.dir/clean

networking/CMakeFiles/networking.dir/depend:
	cd /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/kevinwang/Documents/projects/trading_engine /Users/kevinwang/Documents/projects/trading_engine/networking /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking /Users/kevinwang/Documents/projects/trading_engine/cmake-build-debug/networking/CMakeFiles/networking.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : networking/CMakeFiles/networking.dir/depend

