file(GLOB all_source_files_in_directory "*.cpp")
add_library(lib_ast ${all_source_files_in_directory})
target_link_libraries(lib_ast lib_exceptions)
