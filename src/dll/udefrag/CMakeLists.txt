# configurations
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "udefrag" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

set(BUILD_SHARED_LIBS 1)    #use dlls 
#/D _NO_CRT_STDIO_INLINE /D _DEBUG /D DEBUG /D BUILD_DLL /D _WIN32 /D WIN32 /D _MBCS /D USE_WINSDK /D _CRT_SECURE_NO_WARNINGS /D ATTACH_DEBUGGER /D _CONSOLE /D _USRDLL /D UDEFRAG_EXPORTS /D _WINDLL /D _UNICODE /D UNICODE
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
    -D_CRT_SECURE_NO_WARNINGS
    -DBUILD_DLL
    -D_USRDLL
    -D_WINDLL
    -DUSE_WINSDK    
    -DUDEFRAG_EXPORTS
)
set(CMAKE_C_FLAGS_DEBUG "/Zi /nologo /W3 /WX- /Ob0 /Od /Oy- /GF /Gm /EHsc /MDd /GS- /Gy /fp:precise /Gd /TC")
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
    udefrag.def
    udefrag.rc
)
#Disable the default manifest because we provide our own in udefrag.rc resource file above^
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
target_link_libraries(${PACKAGE_NAME} PRIVATE
    ntdll
    zenwinx
)
set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "/NODEFAULTLIB /SUBSYSTEM:WINDOWS /ENTRY:\"DllMain\"")