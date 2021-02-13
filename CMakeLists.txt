cmake_minimum_required(VERSION 3.5)

project(example LANGUAGES CXX)

if(CMAKE_RUNTIME_OUTPUT_DIRECTORY)
else()
  file(MAKE_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
  set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)
endif()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32) 

endif()

if (UNIX)

endif()

add_subdirectory(example)
add_subdirectory(rubic)
add_subdirectory(race)
add_subdirectory(trbl)
add_subdirectory(pipes)
add_subdirectory(bonbon)
#add_subdirectory(cocos)
