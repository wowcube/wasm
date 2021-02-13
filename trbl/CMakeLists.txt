cmake_minimum_required(VERSION 3.5)

project(trbl LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32) 
    set_source_files_properties(
      ${CORE_SRC}
      PROPERTIES
      COMPILE_FLAGS "/wd4996"
    )
endif()

if (UNIX)

endif()

file(GLOB CORE_HDR *.h)
file(GLOB CORE_SRC *.cpp) 


include_directories(..)

DualCompilation(trbl "${CORE_SRC}" "${CORE_HDR}" "")

