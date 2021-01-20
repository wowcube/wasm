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

macro(DualCompilation name lsrc lhdr)
  add_library(${name} SHARED ${lsrc} ${lhdr})
  if(WIN32)
    message("DualCompilation ${name}: ${lsrc}, ${lhdr}")
    string (REPLACE ";" " " src "${lsrc}")
    string (REPLACE ";" " " hdr "${lhdr}")
    message("DualCompilation ${name}: ${src}, ${hdr}")

    set_source_files_properties(
      ${src}
      PROPERTIES
      COMPILE_FLAGS "/wd4996"
    )


    #EMMAKEN_CFLAGS
    set(WASM_CMD "emcc -I ${CMAKE_CURRENT_LIST_DIR}/.. -Wno-return-type -Wno-implicit-int ${src} -s STANDALONE_WASM -s ERROR_ON_UNDEFINED_SYMBOLS=0 -O3 --no-entry -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}.wasm")
    add_custom_target(wasm_${name} ALL
      COMMAND cmd /c "${WASM_CMD}"
      DEPENDS ${src} ${hdr}
      COMMENT ${WASM_CMD}
      USES_TERMINAL
      VERBATIM
    )
  endif()
  add_dependencies(${name} wasm_${name})
endmacro()

add_subdirectory(example)
add_subdirectory(rubic)
add_subdirectory(race)
add_subdirectory(packman)
add_subdirectory(trbl_example)

