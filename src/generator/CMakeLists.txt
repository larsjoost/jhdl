file(GLOB all_source_files_in_directory "*.cpp")
add_library(lib_generator ${all_source_files_in_directory})
