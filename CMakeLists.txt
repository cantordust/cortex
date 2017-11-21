cmake_minimum_required(VERSION 3.2)

project(cortex)

# Select a custom C++ compiler.
#
# Available options: clang++ | g++
#
# Default: clang++
#
# NOTE: C++14 support required
#
# TODO: Test other compilers (MSVC, MinGW, etc.)

set(default_compiler "clang++")
set(custom_compiler "clang++")

# Select build type
#
# Available options: Release | Debug | RelWithDebInfo
#
# Default: Release

set(build_type "Debug")

# Root directory
set(root_dir "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Root directory")

# Root directory for auxiliary CMake files
set(cmake_aux_dir "${root_dir}/cmake" CACHE PATH "Root directory for auxiliary CMake files")

# Directory containing sources
set(src_dir "${root_dir}/src" CACHE PATH "Root directory for source files")

# Directory containing library sources
set(lib_src_dir "${src_dir}/lib" CACHE PATH "Root directory for library sources")

# Directory containing executable sources
set(bin_src_dir "${src_dir}/bin" CACHE PATH "Root directory for executable sources")

# Directory containing dependencies
set(dep_dir "${root_dir}/dep" CACHE PATH "Root directory for dependencies")

# Directory containin resources
set(res_dir "${root_dir}/res" CACHE PATH "Root directory for resources")

# Directory where the library will be placed
set(lib_dir "${root_dir}/lib")

# Directory where binaries will be placed
set(bin_dir "${root_dir}/bin")

# Various functions
include("${cmake_aux_dir}/Functions.cmake")

# Compiler options
include("${cmake_aux_dir}/CompilerOptions.cmake")

# Dependencies
include("${cmake_aux_dir}/Deps.cmake")

# Name of the configuration template file
set(config_template "config.json")

# Library name
set(lib_name "cortex")

# Directories containing library headers
header_dirs(lib_header_dirs ${lib_src_dir})

# Directories containing dependency headers
header_dirs(dep_header_dirs ${dep_dir})

################
# Cortex library
################

setup_lib()

########################
# Binaries (experiments)
########################

setup_bin()
