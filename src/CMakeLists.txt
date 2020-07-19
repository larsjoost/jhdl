cmake_minimum_required(VERSION 3.1)

project(Jhdl VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

add_executable(hdlc hdlc.cpp)

function(_add_library dir) 
  add_subdirectory(${dir})
  target_include_directories(
    hdlc PUBLIC
    "${PROJECT_BINARY_DIR}"
    "${PROJECT_SOURCE_DIR}/${dir}"
    )

  target_link_libraries(
    hdlc PUBLIC "lib_${dir}" 
    )
  
endfunction()

set(_libraries debug version generator generator/expression parser ast output vhdl/scanner vhdl/parser)

foreach(_library ${_libraries})
  _add_library(${_library})
endforeach()
