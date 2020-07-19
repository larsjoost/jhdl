cmake_minimum_required(VERSION 3.1)

project(Jhdl VERSION 1.0)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(_executables hdlc hdlf)
set(_header_libraries version)
set(_source_libraries debug exceptions generator generator/expression generator/database parser ast output vhdl/scanner vhdl/parser verilog/scanner verilog/parser)

foreach(_library ${_header_libraries})
  include_directories(${_library})
endforeach()

foreach(_library ${_source_libraries})
  add_subdirectory(${_library})
  include_directories(${_library})
  link_directories(${_library})
endforeach()

foreach(_executable ${_executables})
  add_executable(${_executable} "${_executable}.cpp")

  function(_add_source_library dir) 
    string(REPLACE "/" "_" _library_name ${dir})
    target_link_libraries(
      ${_executable} PUBLIC "lib_${_library_name}" 
      )
  
  endfunction()

  foreach(_library ${_source_libraries})
    _add_source_library(${_library})
  endforeach()

endforeach()
