file(GLOB all_source_files_in_directory "*.cpp")
add_library(lib_expression ${all_source_files_in_directory})
