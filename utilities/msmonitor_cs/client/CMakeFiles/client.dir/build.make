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
CMAKE_SOURCE_DIR = /home/aelali/ampehre/utilities/msmonitor_cs/client

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/aelali/ampehre/utilities/msmonitor_cs/client

# Include any dependencies generated for this target.
include CMakeFiles/client.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/client.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/client.dir/flags.make

CMakeFiles/client.dir/src/CComC.cpp.o: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/src/CComC.cpp.o: src/CComC.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aelali/ampehre/utilities/msmonitor_cs/client/CMakeFiles $(CMAKE_PROGRESS_1)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/client.dir/src/CComC.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/client.dir/src/CComC.cpp.o -c /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CComC.cpp

CMakeFiles/client.dir/src/CComC.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/src/CComC.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CComC.cpp > CMakeFiles/client.dir/src/CComC.cpp.i

CMakeFiles/client.dir/src/CComC.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/src/CComC.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CComC.cpp -o CMakeFiles/client.dir/src/CComC.cpp.s

CMakeFiles/client.dir/src/CComC.cpp.o.requires:
.PHONY : CMakeFiles/client.dir/src/CComC.cpp.o.requires

CMakeFiles/client.dir/src/CComC.cpp.o.provides: CMakeFiles/client.dir/src/CComC.cpp.o.requires
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/src/CComC.cpp.o.provides.build
.PHONY : CMakeFiles/client.dir/src/CComC.cpp.o.provides

CMakeFiles/client.dir/src/CComC.cpp.o.provides.build: CMakeFiles/client.dir/src/CComC.cpp.o

CMakeFiles/client.dir/src/CClient.cpp.o: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/src/CClient.cpp.o: src/CClient.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aelali/ampehre/utilities/msmonitor_cs/client/CMakeFiles $(CMAKE_PROGRESS_2)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/client.dir/src/CClient.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/client.dir/src/CClient.cpp.o -c /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CClient.cpp

CMakeFiles/client.dir/src/CClient.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/src/CClient.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CClient.cpp > CMakeFiles/client.dir/src/CClient.cpp.i

CMakeFiles/client.dir/src/CClient.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/src/CClient.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CClient.cpp -o CMakeFiles/client.dir/src/CClient.cpp.s

CMakeFiles/client.dir/src/CClient.cpp.o.requires:
.PHONY : CMakeFiles/client.dir/src/CClient.cpp.o.requires

CMakeFiles/client.dir/src/CClient.cpp.o.provides: CMakeFiles/client.dir/src/CClient.cpp.o.requires
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/src/CClient.cpp.o.provides.build
.PHONY : CMakeFiles/client.dir/src/CClient.cpp.o.provides

CMakeFiles/client.dir/src/CClient.cpp.o.provides.build: CMakeFiles/client.dir/src/CClient.cpp.o

CMakeFiles/client.dir/src/CProtocolC.cpp.o: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/src/CProtocolC.cpp.o: src/CProtocolC.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aelali/ampehre/utilities/msmonitor_cs/client/CMakeFiles $(CMAKE_PROGRESS_3)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/client.dir/src/CProtocolC.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/client.dir/src/CProtocolC.cpp.o -c /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CProtocolC.cpp

CMakeFiles/client.dir/src/CProtocolC.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/src/CProtocolC.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CProtocolC.cpp > CMakeFiles/client.dir/src/CProtocolC.cpp.i

CMakeFiles/client.dir/src/CProtocolC.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/src/CProtocolC.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aelali/ampehre/utilities/msmonitor_cs/client/src/CProtocolC.cpp -o CMakeFiles/client.dir/src/CProtocolC.cpp.s

CMakeFiles/client.dir/src/CProtocolC.cpp.o.requires:
.PHONY : CMakeFiles/client.dir/src/CProtocolC.cpp.o.requires

CMakeFiles/client.dir/src/CProtocolC.cpp.o.provides: CMakeFiles/client.dir/src/CProtocolC.cpp.o.requires
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/src/CProtocolC.cpp.o.provides.build
.PHONY : CMakeFiles/client.dir/src/CProtocolC.cpp.o.provides

CMakeFiles/client.dir/src/CProtocolC.cpp.o.provides.build: CMakeFiles/client.dir/src/CProtocolC.cpp.o

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o: CMakeFiles/client.dir/flags.make
CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o: /home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp
	$(CMAKE_COMMAND) -E cmake_progress_report /home/aelali/ampehre/utilities/msmonitor_cs/client/CMakeFiles $(CMAKE_PROGRESS_4)
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Building CXX object CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o"
	/usr/bin/c++   $(CXX_DEFINES) $(CXX_FLAGS) -o CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o -c /home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.i"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -E /home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp > CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.i

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.s"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_FLAGS) -S /home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp -o CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.s

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.requires:
.PHONY : CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.requires

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.provides: CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.requires
	$(MAKE) -f CMakeFiles/client.dir/build.make CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.provides.build
.PHONY : CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.provides

CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.provides.build: CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o

# Object files for target client
client_OBJECTS = \
"CMakeFiles/client.dir/src/CComC.cpp.o" \
"CMakeFiles/client.dir/src/CClient.cpp.o" \
"CMakeFiles/client.dir/src/CProtocolC.cpp.o" \
"CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o"

# External object files for target client
client_EXTERNAL_OBJECTS =

libclient.so: CMakeFiles/client.dir/src/CComC.cpp.o
libclient.so: CMakeFiles/client.dir/src/CClient.cpp.o
libclient.so: CMakeFiles/client.dir/src/CProtocolC.cpp.o
libclient.so: CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o
libclient.so: CMakeFiles/client.dir/build.make
libclient.so: CMakeFiles/client.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --red --bold "Linking CXX shared library libclient.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/client.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/client.dir/build: libclient.so
.PHONY : CMakeFiles/client.dir/build

CMakeFiles/client.dir/requires: CMakeFiles/client.dir/src/CComC.cpp.o.requires
CMakeFiles/client.dir/requires: CMakeFiles/client.dir/src/CClient.cpp.o.requires
CMakeFiles/client.dir/requires: CMakeFiles/client.dir/src/CProtocolC.cpp.o.requires
CMakeFiles/client.dir/requires: CMakeFiles/client.dir/home/aelali/ampehre/utilities/msmonitor_cs/include/CProtocol.cpp.o.requires
.PHONY : CMakeFiles/client.dir/requires

CMakeFiles/client.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/client.dir/cmake_clean.cmake
.PHONY : CMakeFiles/client.dir/clean

CMakeFiles/client.dir/depend:
	cd /home/aelali/ampehre/utilities/msmonitor_cs/client && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/aelali/ampehre/utilities/msmonitor_cs/client /home/aelali/ampehre/utilities/msmonitor_cs/client /home/aelali/ampehre/utilities/msmonitor_cs/client /home/aelali/ampehre/utilities/msmonitor_cs/client /home/aelali/ampehre/utilities/msmonitor_cs/client/CMakeFiles/client.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/client.dir/depend
