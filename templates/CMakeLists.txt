# Copy from ${JHDL}/templates/CMakeLists.txt

cmake_minimum_required(VERSION 3.1)
project(vhdl)

if (NOT DEFINED ENV{JHDL})
  message( FATAL_ERROR "JHDL is not defined" )
else()
  set(JHDL $ENV{JHDL})
endif()

if (NOT DEFINED ENV{SYSTEMC})
  set(SYSTEMC "/opt/systemc")
else()
  set(SYSTEMC $ENV{SYSTEMC})
endif()

SET(LIBRARY "work" CACHE STRING "Specifies the library name")
SET(ENTITY_NAME "" CACHE STRING "Specifies the entity name")
SET(FILE_NAME "" CACHE STRING "Specifies the file name")

set(_executable ${ENTITY_NAME})

add_executable(${_executable} test.cpp)

file(GLOB _vhdl_files "*.vhd" "*.vhdl")

foreach(_file ${_vhdl_files})
  string(REGEX REPLACE "[.]$" ".hpp" _output_file ${_file})
  add_custom_command(OUTPUT ${_output_file}
    COMMAND hdlc ${_file}
    COMMENT "Parsing ${_file}"
    )
endforeach()

set(_include_directories ${SYSTEMC}/include ${JHDL}/src/kernel ${JHDL}/src/output ${JHDL}/src/debug  ${JHDL}/src/exceptions) 
set(_source_directories ${JHDL}/src/kernel ${JHDL}/src/output ${JHDL}/src/debug  ${JHDL}/src/exceptions) 

include_directories(${_include_directories})
link_directories(${_source_directories})

foreach(_directory ${_source_directories})
  add_subdirectory(${_directory} ${_directory}/build)
  get_filename_component(_name ${_directory} NAME)
  target_link_libraries(
    ${_executable} PUBLIC "lib_${_name}" 
    )
endforeach()

link_directories(${SYSTEMC}/lib64)
target_link_libraries(${_executable} libsystemc)

