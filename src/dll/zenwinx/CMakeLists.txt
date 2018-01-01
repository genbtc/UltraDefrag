# configurations
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "zenwinx" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

#include_directories( )
set(BUILD_SHARED_LIBS 1)    #use dlls 
include (GenerateExportHeader)

add_definitions(
    -D_CONSOLE
    -DUNICODE
    -D_UNICODE
)
add_definitions(
    -D_CRT_SECURE_NO_WARNINGS
    -D_CRT_NON_CONFORMING_SWPRINTFS
    -DBUILD_DLL
    -DUSE_WINSDK        
    -DZENWINX_EXPORTS
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

add_library(${PACKAGE_NAME} SHARED
    dbg.c
    entry.c
    env.c
    event.c
    file.c
    ftw.c
    ftw_ntfs.c
    int64.c
    keyboard.c
    keytrans.c
    ldr.c
    list.c
    lock.c
    mem.c
    misc.c
    mutex.c
    path.c
    prec.c
    prb.c
    privilege.c
    reg.c
    stdio.c
    string.c
    thread.c
    time.c
    utf8.c
    volume.c
    zenwinx.c
    zenwinx.def
)

#link_directories( )
generate_export_header(${PACKAGE_NAME})
target_link_libraries(${PACKAGE_NAME}
    ntdll
    #Somethings like These are needed for _snprintf and vswprintf that kinda thing,
    #(or Win7SDK msvcrt100 or ucrtd from new win10.)
    #msvcrtd
    #msvcprtd
)