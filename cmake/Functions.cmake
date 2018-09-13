# Store all subdirectories of a directory into a list
macro(subdirlist curdir subdirs)
	file(GLOB children RELATIVE ${curdir} ${curdir}/*)
	foreach(child ${children})
		if(IS_DIRECTORY ${curdir}/${child})
			list(APPEND ${subdirs} ${curdir}/${child})
			subdirlist(${curdir}/${child} ${subdirs})
		endif()
	endforeach()
endmacro()

macro(header_dirs return_list dir)
	file(GLOB_RECURSE new_list ${dir}/*.h ${dir}/*.hpp ${dir}/*.hxx)
	set(dir_list "")
	foreach(file_path ${new_list})
		get_filename_component(dir_path ${file_path} PATH)
		set(dir_list ${dir_list} ${dir_path})
	endforeach()
	list(REMOVE_DUPLICATES dir_list)
	set(${return_list} "${dir_list}")

#	foreach(hdir ${dir_list})
#		msg("Header dir: ${hdir}")
#	endforeach()

endmacro()

# Compile the library
function(setup_lib)

	msg("Configuring library ${lib_name}")

	file(GLOB_RECURSE lib_src ${lib_src_dir}/*.cpp ${lib_src_dir}/*.hpp)

	add_library(${lib_name} SHARED ${lib_src})

	set(link_libs ${CMAKE_THREAD_LIBS_INIT})

	if (${use_blas})
		list(APPEND ${link_libs} ${ARMADILLO_LIBRARIES})
	endif()

	target_link_libraries(${lib_name} ${link_libs})
	set_target_properties(${lib_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${lib_dir})
	target_include_directories(
		${lib_name}
		PRIVATE
		${lib_header_dirs}
		${dep_header_dirs}
		)

#	get_property(inc_dirs TARGET ${lib_name} PROPERTY INCLUDE_DIRECTORIES)
#	foreach(incdir ${inc_dirs})
#		msg("Included dir: ${incdir}")
#	endforeach()

endfunction()

# Compile all binaries and process all configuration templates
function(setup_bin bin_name)

	if(EXISTS ${bin_src_dir}/${bin_name})
		message(STATUS "*** Configuring executable ${bin_name}")

		set(bin_src_header_dirs "")
		header_dirs(bin_header_dirs ${bin_src_dir}/${bin_name})

		file(GLOB_RECURSE bin_src ${bin_src_dir}/${bin_name}/*.cpp ${bin_src_dir}/${bin_name}/*.hpp)

		add_executable(${bin_name} ${bin_src})
		target_link_libraries(${bin_name} ${lib_name})
		set_target_properties(${bin_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${bin_dir}/${bin_name})
		target_include_directories(
			${bin_name}
			PRIVATE
			${lib_header_dirs}
			${bin_header_dirs}
			${dep_header_dirs}
			)

#		get_property(inc_dirs TARGET ${bin_name} PROPERTY INCLUDE_DIRECTORIES)
#		foreach(incdir ${inc_dirs})
#			message(STATUS "*** Included dir: ${incdir}")
#		endforeach()

        if(NOT EXISTS ${bin_dir}/${bin_name})
			file(MAKE_DIRECTORY ${bin_dir}/${bin_name})
		endif()

		if(NOT EXISTS ${bin_dir}/${bin_name}/${config_template})
			msg("Configured template ${bin_dir}/${bin_name}/${config_template}")
			configure_file(${res_dir}/${config_template} ${bin_dir}/${bin_name}/${config_template})
		endif()
	else()
		warning("Directory ${bin_src_dir}/${_dir} doesn't exist")
	endif()

endfunction()

# Issue messages with various levels of severity
function(msg messages)
	foreach(message	${messages})
		message(STATUS "*** ${message}")
	endforeach()
endfunction()

function(warning messages)
	message(STATUS "### Warning ###")
	foreach(message	${messages})
		message(STATUS "### ${message}")
	endforeach()
endfunction()

function(error messages)
	message(STATUS "!!! Error !!!")
	foreach(message	${messages})
		message(STATUS "!!! ${message}")
	endforeach()
	message(FATAL_ERROR "!!! Exiting")
endfunction()
