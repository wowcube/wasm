cmake_minimum_required(VERSION 3.5)

project(example LANGUAGES CXX)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32) 

endif()

if (UNIX)

endif()

file(GLOB CORE_HDR *.h)
file(GLOB CORE_SRC *.cpp) 

set_source_files_properties(
  ${CORE_SRC}
  PROPERTIES
  COMPILE_FLAGS "/wd4996"
)

set_source_files_properties(
  rubic/rubic.c
  PROPERTIES
  COMPILE_FLAGS "/wd4033 /wd4716"
)

add_library(example SHARED ${CORE_SRC} ${CORE_HDR} rubic/rubic.c)



