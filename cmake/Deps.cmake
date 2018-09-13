include(ExternalProject)

# Parallel network evaluation using threads
find_package(Threads REQUIRED)

if (use_blas)
	find_package(Armadillo REQUIRED)

	msg("Using BLAS")
	msg("Armadillo include file found under: ${ARMADILLO_INCLUDE_DIRS}")
endif()

#if (NOT DEFINED download_single_header_libs)
#	msg("Downloading single-header libraries...")

#	file(DOWNLOAD
#		https://raw.githubusercontent.com/cantordust/threadpool/master/include/threadpool.hpp
#		${dep_dir}/threadpool.hpp
#	)

#    file(DOWNLOAD
#		https://raw.githubusercontent.com/cantordust/dlog/master/include/dlog.hpp
#		${dep_dir}/dlog.hpp
#	)

#    file(DOWNLOAD
#		https://raw.githubusercontent.com/cantordust/abacus_hash/master/include/abacus_hash.hpp
#		${dep_dir}/abacus_hash.hpp
#	)

#    file(DOWNLOAD
#		https://raw.githubusercontent.com/nlohmann/json/master/single_include/nlohmann/json.hpp
#		${dep_dir}/json.hpp
#	)

#    set(download_single_header_libs
#		FALSE
#		CACHE
#		BOOL
#		"Download single-header libraries"
#		)
#endif()

# TODO: Qt for visualisation
#find_package(Qt5 REQUIRED COMPONENTS Widgets)
# Instruct CMake to run moc automatically when needed.
#set(CMAKE_AUTOMOC ON)
#set(CMAKE_AUTORCC ON)
