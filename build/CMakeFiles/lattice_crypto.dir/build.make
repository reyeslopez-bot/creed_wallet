# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.29

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
CMAKE_COMMAND = /opt/homebrew/Cellar/cmake/3.29.3/bin/cmake

# The command to remove a file.
RM = /opt/homebrew/Cellar/cmake/3.29.3/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/apikorus/creed_wallet

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/apikorus/creed_wallet/build

# Include any dependencies generated for this target.
include CMakeFiles/lattice_crypto.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/lattice_crypto.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/lattice_crypto.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/lattice_crypto.dir/flags.make

CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o: CMakeFiles/lattice_crypto.dir/flags.make
CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o: /Users/apikorus/creed_wallet/lattice_crypto_bindings.cpp
CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o: CMakeFiles/lattice_crypto.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/Users/apikorus/creed_wallet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o -MF CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o.d -o CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o -c /Users/apikorus/creed_wallet/lattice_crypto_bindings.cpp

CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.i"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/apikorus/creed_wallet/lattice_crypto_bindings.cpp > CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.i

CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.s"
	/Applications/Xcode.app/Contents/Developer/Toolchains/XcodeDefault.xctoolchain/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/apikorus/creed_wallet/lattice_crypto_bindings.cpp -o CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.s

# Object files for target lattice_crypto
lattice_crypto_OBJECTS = \
"CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o"

# External object files for target lattice_crypto
lattice_crypto_EXTERNAL_OBJECTS =

lattice_crypto.cpython-312-darwin.so: CMakeFiles/lattice_crypto.dir/lattice_crypto_bindings.cpp.o
lattice_crypto.cpython-312-darwin.so: CMakeFiles/lattice_crypto.dir/build.make
lattice_crypto.cpython-312-darwin.so: /opt/homebrew/opt/python@3.12/Frameworks/Python.framework/Versions/3.12/lib/libpython3.12.dylib
lattice_crypto.cpython-312-darwin.so: CMakeFiles/lattice_crypto.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/Users/apikorus/creed_wallet/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX shared module lattice_crypto.cpython-312-darwin.so"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lattice_crypto.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/lattice_crypto.dir/build: lattice_crypto.cpython-312-darwin.so
.PHONY : CMakeFiles/lattice_crypto.dir/build

CMakeFiles/lattice_crypto.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/lattice_crypto.dir/cmake_clean.cmake
.PHONY : CMakeFiles/lattice_crypto.dir/clean

CMakeFiles/lattice_crypto.dir/depend:
	cd /Users/apikorus/creed_wallet/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/apikorus/creed_wallet /Users/apikorus/creed_wallet /Users/apikorus/creed_wallet/build /Users/apikorus/creed_wallet/build /Users/apikorus/creed_wallet/build/CMakeFiles/lattice_crypto.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/lattice_crypto.dir/depend

