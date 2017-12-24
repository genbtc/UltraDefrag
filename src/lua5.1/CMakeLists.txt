#CMake Config, Builds OK. No dependencies or anything really.
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "lua5.1a" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME}" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "5.1a" )

#include_directories( )

add_definitions(
    -DWIN64
    -DUSE_WINSDK
    -D_CRT_SECURE_NO_WARNINGS
)
if(Debug)
    add_definitions(-DATTACH_DEBUGGER)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PACKAGE_NAME}
    lapi.c
    lauxlib.c
    lbaselib.c
    lcode.c
    ldblib.c
    ldebug.c
    ldo.c
    ldump.c
    lfunc.c
    lgc.c
    linit.c
    liolib.c
    llex.c
    lmathlib.c
    lmem.c
    loadlib.c
    lobject.c
    lopcodes.c
    loslib.c
    lparser.c
    lstate.c
    lstring.c
    lstrlib.c
    ltable.c
    ltablib.c
    ltm.c
    lundump.c
    lvm.c
    lzio.c
)

#link_directories( )
