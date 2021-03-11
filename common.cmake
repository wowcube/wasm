
macro(DualCompilation name lsrc lhdr wasm_ext)
  add_library(${name} SHARED ${lsrc} ${lhdr})
  if(WIN32)
    message("DualCompilation ${name}: ${lsrc}, ${lhdr}")
    string (REPLACE ";" " " src "${lsrc}")
    string (REPLACE ";" " " hdr "${lhdr}")
    message("DualCompilation ${name}: ${src}, ${hdr}")

    set_source_files_properties(
      ${src}
      PROPERTIES
      COMPILE_FLAGS "/wd4996 /wd4090 /wd4133 /std:c++17"
    )
    set(EMCC_WARNINGS "-Wno-unsequenced -Wno-return-type -Wno-implicit-int -Wno-incompatible-pointer-types-discards-qualifiers -Wno-incompatible-pointer-types -Wno-c++17-extensions")
    set(EMCC_FLAGS "-I ${CMAKE_SOURCE_DIR}/wasm/wasm_scripts ${EMCC_WARNINGS} ${src} -s STANDALONE_WASM -s ERROR_ON_UNDEFINED_SYMBOLS=0 --no-entry")
    # -s SAFE_HEAP=1 -s SAFE_HEAP_LOG=1 | -fsanitize=address -s ALLOW_MEMORY_GROWTH & -s ASSERTIONS=2 -s STACK_OVERFLOW_CHECK=2
    set(EMCC_FLAGS_DEBUG "-g4 -s ASSERTIONS=1 -s STACK_OVERFLOW_CHECK=1 -s DEMANGLE_SUPPORT=1")
    set(EMCC_FLAGS_RELEASE "-O2")

    #EMMAKEN_CFLAGS
    set(WASM_CMD "emcc ${EMCC_FLAGS} ${EMCC_FLAGS_DEBUG} ${wasm_ext} -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}_d.wasm && emcc ${EMCC_FLAGS} ${EMCC_FLAGS_RELEASE} ${wasm_ext} -o ${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/${name}_r.wasm")
    string(REPLACE " " ";" WASM_DEPENDS "${src} ${hdr}")
    add_custom_target(wasm_${name} ALL
      COMMAND cmd /c "${WASM_CMD}"
      DEPENDS ${WASM_DEPENDS}
      COMMENT ${WASM_CMD}
      USES_TERMINAL
      VERBATIM
    )
  endif()
  add_dependencies(wasm_${name} ${name})
endmacro()
