# Select a C++ standard version (default: 14)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(default_compiler Clang)

if(NOT (${custom_compiler} STREQUAL "GCC" OR ${custom_compiler} STREQUAL "Clang") )
	warning("Invalid C++ compiler '${custom_compiler}'. Defaulting to '${default_compiler}'.")
	set(custom_compiler ${default_compiler})
endif()

set(CMAKE_CXX_COMPILER_ID ${custom_compiler} CACHE STRING "C++ compiler" FORCE)
msg("Compiler: ${CMAKE_CXX_COMPILER}")

## TODO:	Platform-aware stdandard library implementation
if(${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang")
#	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

if(NOT (${build_type} STREQUAL "Release" OR ${build_type} STREQUAL "Debug" OR ${build_type} STREQUAL "RelWithDebInfo") )
	warning("Invalid build type '${build_type}'. Defaulting to 'Release'.")
	set(build_type Release)
endif()

if(${build_type} STREQUAL "Debug")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")
elseif(${build_type} STREQUAL "Release")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3")
elseif(${build_type} STREQUAL "RelWithDebInfo")
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -g")
endif()

msg("Compiler flags: ${CMAKE_CXX_FLAGS}")

set(CMAKE_BUILD_TYPE ${build_type} CACHE STRING "Build type (Debug | Release | RelWithDebInfo)" FORCE)
