#
# CMake Emscripten Toolchain
#

SET (EMSCRIPTEN_ROOT "/opt/emscripten")
SET (EMSCRIPTEN "True")

SET (CMAKE_SYSTEM_NAME Linux)
SET (CMAKE_SYSTEM_VERSION 1)

IF (NOT EXISTS "${EMSCRIPTEN_ROOT}")
  MESSAGE(FATAL_ERROR "Can't find emscripten toolchain directory")
ENDIF()

IF (CMAKE_HOST_APPLE)
  SET( TOOL_OS_SUFFIX "" )
ELSEIF (CMAKE_HOST_WIN32)
  SET( TOOL_OS_SUFFIX ".bat" )
ELSEIF (CMAKE_HOST_UNIX)
  SET( TOOL_OS_SUFFIX "" )
ELSE()
  MESSAGE(FATAL_ERROR "emscripten compilation not supported on this platform")
ENDIF()

ADD_DEFINITIONS( -DEMSCRIPTEN )

SET (EMSCRIPTEN_C_FLAGS "-Wno-warn-absolute-paths -s TOTAL_MEMORY=50331648 -s ASM_JS=0 -s WARN_ON_UNDEFINED_SYMBOLS=1")

SET (EMSCRIPTEN_CXX_FLAGS "${EMSCRIPTEN_C_FLAGS} -I${EMSCRIPTEN_ROOT}/system/lib/libcxxabi/include")

SET (CMAKE_AR                       "${EMSCRIPTEN_ROOT}/emar${TOOL_OS_SUFFIX}" CACHE PATH "archive")
SET (CMAKE_RANLIB                   "${EMSCRIPTEN_ROOT}/emranlib${TOOL_OS_SUFFIX}" CACHE PATH "ranlib")
SET (CMAKE_LINKER                   "${EMSCRIPTEN_ROOT}/em++${TOOL_OS_SUFFIX}" CACHE PATH "linker")
SET (CMAKE_C_COMPILER               "${EMSCRIPTEN_ROOT}/emcc${TOOL_OS_SUFFIX}" CACHE PATH "gcc")
SET (CMAKE_CXX_COMPILER             "${EMSCRIPTEN_ROOT}/em++${TOOL_OS_SUFFIX}" CACHE PATH "g++")
SET (CMAKE_ASM_COMPILER             "${EMSCRIPTEN_ROOT}/emcc${TOOL_OS_SUFFIX}" CACHE PATH "Assembler")

SET (CMAKE_C_FLAGS                  "${EMSCRIPTEN_C_FLAGS} -Wall -std=c99" CACHE STRING "c flags")
SET (CMAKE_C_FLAGS_DEBUG            "-O0 -g -DDEBUG" CACHE STRING "c Debug flags")
SET (CMAKE_C_FLAGS_RELEASE          "-O2 -DNDEBUG" CACHE STRING "c Release flags")
SET (CMAKE_C_LINK_FLAGS             "-O2" CACHE STRING " c Link flags")

SET (CMAKE_CXX_FLAGS                "${EMSCRIPTEN_CXX_FLAGS} -Wall -std=gnu++0x" CACHE STRING "c++ flags")
SET (CMAKE_CXX_FLAGS_DEBUG          "-O0 -g -DDEBUG" CACHE STRING "c++ Debug flags")
SET (CMAKE_CXX_FLAGS_RELEASE        "-O2 -DNDEBUG" CACHE STRING "c++ Release flags")
SET (CMAKE_CXX_LINK_FLAGS           "-O2" CACHE STRING " c Link flags")

# Force set compilers because standard identification works badly for us
include( CMakeForceCompiler )
CMAKE_FORCE_C_COMPILER( "${CMAKE_C_COMPILER}" GNU )
set( CMAKE_C_PLATFORM_ID Linux )
set( CMAKE_C_SIZEOF_DATA_PTR 4 )
set( CMAKE_C_HAS_ISYSROOT 1 )
set( CMAKE_C_COMPILER_ABI ELF )
CMAKE_FORCE_CXX_COMPILER( "${CMAKE_CXX_COMPILER}" GNU )
set( CMAKE_CXX_PLATFORM_ID Linux )
set( CMAKE_CXX_SIZEOF_DATA_PTR 4 )
set( CMAKE_CXX_HAS_ISYSROOT 1 )
set( CMAKE_CXX_COMPILER_ABI ELF )
# force ASM compiler (required for CMake < 2.8.5)
set( CMAKE_ASM_COMPILER_ID_RUN TRUE )
set( CMAKE_ASM_COMPILER_ID GNU )
set( CMAKE_ASM_COMPILER_WORKS TRUE )
set( CMAKE_ASM_COMPILER_FORCED TRUE )
set( CMAKE_COMPILER_IS_GNUASM 1)
