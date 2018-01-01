# configurations
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "udefrag" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

#include_directories( )
set(BUILD_SHARED_LIBS 1)    #use dlls 

add_definitions(
    -D_CONSOLE
    -DUNICODE
    -D_UNICODE
)
add_definitions(
    -D_NO_CRT_STDIO_INLINE
    -D_CRT_SECURE_NO_WARNINGS
    -DBUILD_DLL
    -DUSE_WINSDK    
    -DUDEFRAG_EXPORTS
)
if(Debug)
    add_definitions(
        -DATTACH_DEBUGGER
        -DDEBUG
        -D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PACKAGE_NAME}
    analyze.c
    auxiliary.c
    defrag.c
    entry.c
    int64.c
    map.c
    move.c
    optimize.c
    options.c
    query.c
    reports.c
    search.c
    udefrag.c
    volume.c
)

#link_directories( )

target_link_libraries(${PACKAGE_NAME}
    ntdll
    zenwinx
)
