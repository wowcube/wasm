cmake_minimum_required(VERSION 3.5)

project(cocos LANGUAGES CXX)

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
set(COCOS2D "c:/Users/alrau/git/emsdk/upstream/emscripten/cache/wasm/include/cocos2d/cocos2dx")
include_directories("${COCOS2D}" "${COCOS2D}/platform/win32" "${COCOS2D}/platform" "${COCOS2D}/kazmath/include" "${COCOS2D}/cocoa" "${COCOS2D}/include" "C:/Users/alrau/git/emsdk/upstream/emscripten/system/include")


DualCompilation(cocos "${CORE_SRC}" "${CORE_HDR}" "-s USE_COCOS2D=3")

