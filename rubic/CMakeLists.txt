cmake_minimum_required(VERSION 3.5)

project(rubic LANGUAGES C CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${CMAKE_ENABLE_C99}")

file(GLOB CORE_HDR *.h)
file(GLOB CORE_SRC *.c)

set(CORE_SRC ${CORE_SRC} ${CMAKE_CURRENT_LIST_DIR}/PawnRunner.cpp)

#[[
set_source_files_properties(
  ${CORE_SRC}
  PROPERTIES
  COMPILE_FLAGS "/wd4996"
)
]]

set_source_files_properties(
  ${CORE_SRC}
  PROPERTIES
  COMPILE_FLAGS "/wd4033 /wd4716 /wd4996"
)

include_directories(..)
DualCompilation(rubic "${CORE_SRC}" "${CORE_HDR}")
#add_library(rubic SHARED ${CORE_SRC} ${CORE_HDR})



