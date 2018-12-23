# Issue messages with various levels of severity
function(msg messages)
	foreach(message	${messages})
		message(STATUS "*** ${message}")
	endforeach()
endfunction()

function(warning messages)
	foreach(message	${messages})
		message(STATUS ">>> ${message}")
	endforeach()
endfunction()

function(error messages)
	foreach(message	${messages})
		message(STATUS "!!! ${message}")
	endforeach()
	message(FATAL_ERROR "!!! Exiting")
endfunction()

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

macro(find_headers return_list dir)
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

# Configure a library for compilation
function(setup_lib lib_name)

	msg("Configuring library ${lib_name}")

	file(GLOB_RECURSE lib_source_files ${lib_src_root_dir}/*.cpp ${lib_src_root_dir}/*.hpp)
#	file(GLOB_RECURSE lib_header_files ${lib_src_root_dir}/*.hpp)

    set(link_libs "${CMAKE_THREAD_LIBS_INIT};")

	if (generate_python_bindings)

		add_subdirectory(${dep_src_root_dir}/pybind11)
		list(APPEND ${link_libs} pybind11::module)

		file(GLOB_RECURSE python_bindings_sources ${lib_src_root_dir}/bind/python/*.cpp)

		foreach (src ${python_bindings_sources})
			list(APPEND ${lib_source_files} ${src})
		endforeach()
	endif()

#	msg("Sources for library ${lib_name}:")
#	foreach(src ${lib_source_files})
#		msg("\t${src}")
#	endforeach()

    add_library(${lib_name} SHARED ${lib_source_files})
	target_link_libraries(${lib_name} PRIVATE ${link_libs})
	msg("${}")
	set_target_properties(${lib_name} PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${lib_gen_root_dir})
	set_target_properties(${lib_name} PROPERTIES PREFIX "" SUFFIX ${CMAKE_SHARED_LIBRARY_SUFFIX})

	target_include_directories(
		${lib_name}
		PRIVATE
		${lib_header_dirs}
		${dep_header_dirs}
		)

#	get_property(inc_dirs TARGET ${lib_name} PROPERTY INCLUDE_DIRECTORIES)
#	msg("Include directories for library ${lib_name}:")
#	foreach(incdir ${inc_dirs})
#		msg("\t${incdir}")
#	endforeach()

endfunction()

# Configure a binary for compilation
function(setup_bin bin_name)

	if(EXISTS ${bin_src_root_dir}/${bin_name})
		message(STATUS "*** Configuring binary ${bin_name}")

		find_headers(bin_header_dirs ${bin_src_root_dir}/${bin_name})

		file(GLOB_RECURSE bin_source_files ${bin_src_root_dir}/${bin_name}/*.cpp)

		set(link_libs "${core_lib_name};${Python_LIBRARIES}")

#		msg("Sources for binary ${bin_name}:")
#		foreach(src ${bin_source_files})
#			msg("\t${src}")
#		endforeach()

        add_executable(${bin_name} ${bin_source_files})
		target_link_libraries(${bin_name} PRIVATE ${link_libs})
		set_target_properties(${bin_name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${bin_gen_root_dir}/${bin_name})
		target_include_directories(
			${bin_name}
			PRIVATE
			${lib_header_dirs}
			${dep_header_dirs}
			${bin_header_dirs}
			)

#	get_property(inc_dirs TARGET ${bin_name} PROPERTY INCLUDE_DIRECTORIES)
#	msg("Include directories for binary ${bin_name}:")
#	foreach(incdir ${inc_dirs})
#		msg("\t${incdir}")
#	endforeach()

        if(NOT EXISTS ${bin_gen_root_dir}/${bin_name})
			file(MAKE_DIRECTORY ${bin_gen_root_dir}/${bin_name})
		endif()

	else()
		error("Directory ${bin_src_root_dir}/${bin_name} doesn't exist")
	endif()
endfunction()
