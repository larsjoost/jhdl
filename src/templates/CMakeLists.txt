# Copy from ${JHDL}/src/templates/CMakeLists.txt

cmake_minimum_required(VERSION 3.1)

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

set(_executable test)

add_executable(${_executable} test.cpp)

set(_include_directories ${SYSTEMC}/include ${JHDL}/src/kernel/lib ${JHDL}/src/output ${JHDL}/src/debug  ${JHDL}/src/exceptions ${JHDL}/std) 
set(_source_directories ${JHDL}/src/kernel ${JHDL}/src/output ${JHDL}/src/debug  ${JHDL}/src/exceptions ${JHDL}/std) 

include_directories(${_include_directories})
add_subdirectory(${_source_directories})
link_directories(${_source_directories})

foreach(_directory ${_source_directories})
  get_filename_component(_name ${_directory})
  target_link_libraries(
    ${_executable} PUBLIC "lib_${_name}" 
    )
endforeach()

link_directories(${SYSTEMC}/lib64)
target_link_libraries(${_executable} libsystemc)

