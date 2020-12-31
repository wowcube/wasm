cmake_minimum_required(VERSION 3.5)

project(example LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if(WIN32) 

endif()

if (UNIX)

endif()

macro(DualCompilation name lsrc lhdr)
  message("DualCompilation ${name}: ${lsrc}, ${lhdr}")
  string (REPLACE ";" " " src "${lsrc}")
  string (REPLACE ";" " " hdr "${lhdr}")
  message("DualCompilation ${name}: ${src}, ${hdr}")

  set_source_files_properties(
    ${src}
    PROPERTIES
    COMPILE_FLAGS "/wd4996"
  )

  add_library(${name} SHARED ${lsrc} ${lhdr})

  #EMMAKEN_CFLAGS
  set(WASM_CMD "emcc -I ${CMAKE_CURRENT_LIST_DIR}/.. -Wno-return-type ${src} -s STANDALONE_WASM -s ERROR_ON_UNDEFINED_SYMBOLS=0 -O3 --no-entry -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}.wasm")
  add_custom_target(wasm_${name} ALL
    COMMAND cmd /c "${WASM_CMD}"
    DEPENDS ${src} ${hdr}
    COMMENT ${WASM_CMD}
    USES_TERMINAL
    VERBATIM
  )
  add_dependencies(${name} wasm_${name})

endmacro()

file(GLOB CORE_HDR *.h)
file(GLOB CORE_SRC *.cpp)

DualCompilation(example ${CORE_SRC} ${CORE_HDR})

add_subdirectory(rubic)
add_subdirectory(race)


