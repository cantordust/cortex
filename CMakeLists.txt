cmake_minimum_required(VERSION 3.6)

project(cortex CXX)

# Select a custom C++ compiler.
#
# Default: /usr/bin/clang++
#
# NOTE: C++17 support required
#
# TODO: Test other compilers (msvc, mingw, etc.)

set(custom_compiler /usr/bin/clang++)
#set(custom_compiler /usr/bin/g++)

# Select build type
#
# Recognised options: Release | Debug | RelWithDebInfo
#
# Default: Debug

set(build_type Debug)

# Use the Armadillo BLAS library
set(use_blas OFF)

# Root directory
set(root_dir "${CMAKE_CURRENT_SOURCE_DIR}" CACHE PATH "Root directory")

# Root directory for auxiliary CMake files
set(cmake_aux_dir "${root_dir}/cmake" CACHE PATH "Root directory for auxiliary CMake files")

# Directory containing sources
set(src_dir "${root_dir}/src" CACHE PATH "Root directory for source files")

# Root directory for library sources
set(lib_src_dir "${src_dir}/lib" CACHE PATH "Root directory for library sources")

# Root directory for executable sources
set(bin_src_dir "${src_dir}/bin" CACHE PATH "Root directory for executable sources")

# Root directory for dependencies
set(dep_dir "${root_dir}/dep" CACHE PATH "Root directory for dependencies")

# Root directory for resources
set(res_dir "${root_dir}/res" CACHE PATH "Root directory for resources")

# Root directory for libraries
set(lib_dir "${root_dir}/lib")

# Root directory for binaries
set(bin_dir "${root_dir}/bin")

# System includes
set(sys_inc_dir "/usr/include")

# Name of the configuration template file
set(config_template "config.json")

# Library name
set(lib_name "cortex")

#########################
# Auxiliary CMake modules
#########################

# Various functions
include("${cmake_aux_dir}/Functions.cmake")

# Dependencies
include("${cmake_aux_dir}/Deps.cmake")

# Compiler options
include("${cmake_aux_dir}/CompilerOptions.cmake")

#######################################
# Gather all header files for inclusion
#######################################

# Directories containing library headers
header_dirs(lib_header_dirs ${lib_src_dir})

# Directories containing dependency headers
header_dirs(dep_header_dirs ${dep_dir})

################
# Cortex library
################

setup_lib()

##########
# Binaries
##########

setup_bin(dummy)
#setup_bin(xor)
#setup_bin(wdbc)
#setup_bin(cart_pole)
