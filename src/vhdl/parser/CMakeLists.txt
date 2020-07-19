file(GLOB all_source_files_in_directory "*.cpp")
add_library(lib_vhdl_parser ${all_source_files_in_directory})
