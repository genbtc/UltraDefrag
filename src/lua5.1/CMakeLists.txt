#CMake Config, Builds OK. No dependencies or anything really. Simple.
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "lua5.1a" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME}" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "5.1a" )

set(BUILD_SHARED_LIBS 1)    #use dlls 
#/D WIN64 /D NDEBUG /D _MBCS /D USE_WINSDK /D _CRT_SECURE_NO_WARNINGS /D OFFICIAL_RELEASE /D ATTACH_DEBUGGER
add_definitions(
    -DUSE_WINSDK
    -D_CRT_SECURE_NO_WARNINGS
    -DOFFICIAL_RELEASE
    -D_MBCS
    -DWIN64
    -DWIN32
    -D_WIN32
    -D_WINDOWS    
)
#/Zi /nologo /W3 /WX- /Od /GF /Gm /EHsc /RTC1 /MDd /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd
set(CMAKE_C_FLAGS_DEBUG "/Zi /nologo /W3 /WX- /Ob0 /Od /GF /Gm /EHsc /MDd /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /TC ")
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
    lua5.1.def
)
#Set Link Flags:
#1.   Disable the default manifest because we provide our own in zenwinx.rc resource file above^
#2.   Disable incremental build.
#3.   Set Subsystem
#4.   No Default Libs
#5.   Set Entry Point 
set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO /INCREMENTAL:NO /SUBSYSTEM:WINDOWS /NODEFAULTLIB")