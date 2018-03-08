include(ExternalProject)

# Parallel network evaluation using threads
find_package(Threads REQUIRED)

#if (${use_blas})

#	set(arma_dir ${dep_dir}/armadillo)

#	msg("Using BLAS")
#	msg("Armadillo dir: ${arma_dir}")

#	ExternalProject_Add(
#		arma_local
#		PREFIX ${arma_dir}
#		TMP_DIR ${arma_dir}/tmp
#		GIT_REPOSITORY
#		SOURCE_DIR ${arma_dir}
#		BUILD_IN_SOURCE 1
#		INSTALL_NAME_DIR ${lib_dir}
#	)
#endif()

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
