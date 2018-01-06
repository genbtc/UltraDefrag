# configurations
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "zenwinx" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

set(BUILD_SHARED_LIBS 1)    #use dlls 
#_NO_CRT_STDIO_INLINE /D _CRT_NON_CONFORMING_SWPRINTFS /D DEBUG /D _DEBUG /D BUILD_DLL /D WIN32 /D _WIN32 /D _MBCS /D USE_WINSDK /D _CRT_SECURE_NO_WARNINGS /D _CONSOLE /D _USRDLL /D ZENWINX_EXPORTS /D _WINDLL /D _UNICODE /D UNICODE 
add_definitions(
    -D_CONSOLE
    -DUNICODE
    -D_UNICODE
    -D_MBCS
    -DWIN32
    -D_WIN32
    -D_WINDOWS
    -D_WINDLL    
)
add_definitions(
    -D_NO_CRT_STDIO_INLINE
    -D_CRT_NON_CONFORMING_SWPRINTFS
    -D_CRT_SECURE_NO_WARNINGS
    -DBUILD_DLL
    -D_USRDLL
    -D_WINDLL
    -DUSE_WINSDK        
    -DZENWINX_EXPORTS
)
set(CMAKE_C_FLAGS_DEBUG "/Zi /nologo /W3 /WX- /Ob0 /Od /GF /Gm /EHsc /MDd /GS- /fp:precise /TC")
if(Debug)
    add_definitions(
        -DATTACH_DEBUGGER
        -DDEBUG
        -D_DEBUG)
else()
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

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
    prec.h
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
    zenwinx.rc
)
#Add .H Files
target_sources(${PACKAGE_NAME} PRIVATE prec.h case-tables.h ntfs.h ntndk.h prb.h z-defs.h zenwinx.h z-structs.h )
#Link Libraries, Make the DLL
target_link_libraries(${PACKAGE_NAME} PRIVATE
    ntdll
)
#Set Link Flags:
#1.   Disable the default manifest because we provide our own in zenwinx.rc resource file above^
#2.   Disable incremental build.
#3.   Set Subsystem
#4.   No Default Libs
#5.   Set Entry Point 
set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO /NODEFAULTLIB /ENTRY:\"DllMain\"")