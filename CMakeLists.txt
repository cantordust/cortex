cmake_minimum_required(VERSION 3.8.2)

project(cortex CXX)

#####################
# Compilation options
#####################

# Build type
set(build_type Debug)

# Custom compiler path
# NOTE: C++17 support required
# set(use_compiler /usr/bin/g++)

###################
# Optional bindings
###################

set(generate_python_bindings ON CACHE BOOL "Generate Python bindings")

set(use_libtorch OFF CACHE BOOL "Use LibTorch")

#########################
# Auxiliary CMake modules
#########################

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
include(Paths)
include(Functions)
include(Dependencies)
include(CompilerOptions)

##############
# Header files
##############

# Find headers dependency headers
find_headers(dep_header_dirs ${dep_src_root_dir})

# Directories containing library headers
find_headers(lib_header_dirs ${lib_src_root_dir})

###########
# Libraries
###########

setup_lib(${core_lib_name})

##########
# Binaries
##########

setup_bin(test)
#setup_bin(xor)
#setup_bin(wdbc)
#setup_bin(cart_pole)

############
# Unit tests
############

#setup_test(stat)

###############
# Documentation
###############

#setup_doc()
