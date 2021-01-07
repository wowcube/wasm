cmake_minimum_required(VERSION 3.5)

project(race LANGUAGES CXX)

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
#add_library(race SHARED ${CORE_SRC} ${CORE_HDR})
DualCompilation(race "${CORE_SRC}" "${CORE_HDR}")

