# Select a C++ standard version (default: 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(CMAKE_GENERATOR "Unix Makefiles")

if("${build_type}" STREQUAL "Release" OR "${build_type}" STREQUAL "Debug" OR "${build_type}" STREQUAL "RelWithDebInfo")
	set(CMAKE_BUILD_TYPE "${build_type}" CACHE STRING "Build type (Debug | Release | RelWithDebInfo)" FORCE)
else()
	warning("Invalid build type '${build_type}'. Defaulting to 'Release'.")
	set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Build type (Debug | Release | RelWithDebInfo)" FORCE)
endif()

if("${custom_compiler}" STREQUAL "g++" OR "${custom_compiler}" STREQUAL "clang++")
	set(CMAKE_CXX_COMPILER "${custom_compiler}" CACHE STRING "C++ compiler" FORCE)
else()
	warning("Invalid C++ compiler '${custom_compiler}'. Defaulting to '${default_compiler}'.")
	set(CMAKE_CXX_COMPILER "${default_compiler}" CACHE STRING "C++ compiler" FORCE)
endif()

#set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
