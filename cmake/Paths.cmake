set(src_root_dir_name "src" CACHE INTERNAL "Directory name for source files")

set(gen_root_dir_name "gen" CACHE INTERNAL "Directory name for generated files")

set(bin_dir_name "bin" CACHE INTERNAL "Directory name for binaries")

set(dep_dir_name "dep" CACHE INTERNAL "Directory name for dependencies")

set(lib_dir_name "lib" CACHE INTERNAL "Directory name for libraries")

set(test_dir_name "test" CACHE INTERNAL "Directory name for unit tests")

set(doc_dir_name "doc" CACHE INTERNAL "Directory name for documentation")

set(core_lib_name "cortex" CACHE INTERNAL "Name of the core library")

#############################
# Root paths for source files
#############################

set(src_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/${src_root_dir_name}" CACHE INTERNAL "Root directory for all sources")

set(bin_src_root_dir "${src_root_dir}/${bin_dir_name}" CACHE INTERNAL "Root directory containing source files for binaries")

set(dep_src_root_dir "${src_root_dir}/${dep_dir_name}" CACHE INTERNAL "Root directory containing sources for external dependencies")

set(lib_src_root_dir "${src_root_dir}/${lib_dir_name}" CACHE INTERNAL "Root directory containing sources for libraries")

set(test_src_root_dir "${src_root_dir}/${test_dir_name}" CACHE INTERNAL "Root directory containing sources for unit tests")

################################
# Root paths for generated files
################################

set(gen_root_dir "${CMAKE_CURRENT_SOURCE_DIR}/${gen_root_dir_name}" CACHE INTERNAL "Root directory for all sources")

set(bin_gen_root_dir "${gen_root_dir}/${bin_dir_name}" CACHE INTERNAL "Root directory for generated binaries")

set(doc_gen_root_dir "${gen_root_dir}/${doc_dir_name}" CACHE INTERNAL "Root directory for generated documentation")

set(lib_gen_root_dir "${gen_root_dir}/${lib_dir_name}" CACHE INTERNAL "Root directory for generated libraries")

set(test_gen_root_dir "${gen_root_dir}/${test_dir_name}" CACHE INTERNAL "Root directory for generated unit tests")

