include(ExternalProject)

# Parallel network evaluation using threads
find_package(Threads REQUIRED)

set(CMAKE_PREFIX_PATH "${CMAKE_PREFIX_PATH};$ENV{HOME}/code/external/libs/libtorch")
find_package(Torch REQUIRED)

include_directories(${PYTHON_INCLUDE_DIRS})
include_directories("/home/cantor/code/external/libs/libtorch/include")
include_directories("/home/cantor/code/external/libs/libtorch/include/torch/csrc/api/include/")

if (generate_python_bindings)

	find_package(Python COMPONENTS Interpreter Development REQUIRED)

	include_directories(${Python_INCLUDE_DIRS})

	msg("Emitting instruction to generate Python bindings")
	add_definitions(-DGENERATE_PYTHON_BINDINGS)

endif()

# TODO: Qt for visualisation
# find_package(Qt5 REQUIRED COMPONENTS Widgets)
# Instruct CMake to run moc automatically when needed.
# set(CMAKE_AUTOMOC ON)
# set(CMAKE_AUTORCC ON)
