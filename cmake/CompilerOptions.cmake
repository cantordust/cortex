# Select a C++ standard (default: 17)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)
set(default_compiler /usr/bin/clang++)
set(default_build_type Debug)

set(CMAKE_CXX_COMPILER ${custom_compiler} CACHE STRING "Custom compiler" FORCE)

if(NOT (${build_type} STREQUAL "Release" OR ${build_type} STREQUAL "Debug" OR ${build_type} STREQUAL "RelWithDebInfo") )
	warning("Invalid build type '${build_type}'. Defaulting to '${default_build_type}'.")
	set(build_type ${default_build_type})
endif()

set(CMAKE_BUILD_TYPE ${build_type} CACHE STRING "Build type (Debug | Release | RelWithDebInfo)" FORCE)

msg("Build type: ${CMAKE_BUILD_TYPE}")
msg("Compiler: ${CMAKE_CXX_COMPILER}")
if(${CMAKE_BUILD_TYPE} STREQUAL "Release")
	msg("Compiler flags: ${CMAKE_CXX_FLAGS_RELEASE}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")
	msg("Compiler flags: ${CMAKE_CXX_FLAGS_DEBUG}")
elseif(${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
	msg("Compiler flags: ${CMAKE_CXX_FLAGS_RELWITHDEBINFO}")
endif()

