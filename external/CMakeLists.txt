cmake_minimum_required(VERSION 3.1)

#message(STATUS "Subproject: PODD...")
# Download and unpack gflags at configure time
configure_file(
  ${CMAKE_CURRENT_SOURCE_DIR}/PODD.CMakeLists.txt
  ${CMAKE_BINARY_DIR}/PODD-download/CMakeLists.txt)

execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PODD-download)
if(result)
  message(FATAL_ERROR "CMake step for PODD failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PODD-download)
if(result)
  message(FATAL_ERROR "Download step for exteranl PODD build failed: ${result}")
endif()

add_subdirectory(
  ${CMAKE_BINARY_DIR}/hallac_PODD-src
  ${CMAKE_BINARY_DIR}/hallac_PODD-build)

#execute_process(COMMAND "rm -r *"
#  RESULT_VARIABLE result
#  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/PODD-download)

message(STATUS "Subproject: PODD...DONE")

