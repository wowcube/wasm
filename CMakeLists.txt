cmake_minimum_required(VERSION 3.5)

project(example LANGUAGES CXX)

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

add_library(example SHARED ${CORE_SRC} ${CORE_HDR})


set(WASM_CMD "emcc ${CORE_SRC} -s STANDALONE_WASM -s ERROR_ON_UNDEFINED_SYMBOLS=0 -O3 --no-entry -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/wasm_example.wasm")
add_custom_target(wasm_example ALL
  COMMAND cmd /c "${WASM_CMD}"
  DEPENDS ${CORE_SRC}
  COMMENT ${WASM_CMD}
  USES_TERMINAL
  VERBATIM
) #WORKING_DIRECTORY dir
add_dependencies(example wasm_example)

add_subdirectory(rubic)
add_subdirectory(race)


