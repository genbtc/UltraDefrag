# configurations CMake for Stopgap - (Dependencies: Boost) 
# created by genBTC - December 23 2017
cmake_minimum_required(VERSION 3.8)
#Package definitions
set( PACKAGE_NAME "stopgap" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

set(BUILD_SHARED_LIBS 1)    #use dlls 

add_definitions(
    -D_CONSOLE
    -DUNICODE
    -D_UNICODE
    -DWIN32
    -D_WIN32
    -D_WINDOWS
    -D_WINDLL
)
#Set C++ Compile Flags
set(CMAKE_CXX_FLAGS_DEBUG "/GS /W3 /Gy /Zc:wchar_t /Zi /Gm /Ob0 /Od /fp:precise /errorReport:prompt /GF /WX- /Zc:forScope /Gd /MDd /EHsc /nologo /TP")
#COMMON TO ALL DEBUG CONFIGS
if(Debug)
    add_definitions(
        -DATTACH_DEBUGGER
        -DDEBUG
        -D_DEBUG)
    set(Boost_DEBUG 1)
else()
    add_definitions(-DNDEBUG)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#Include Boost, (mandatory)
#set(Boost_USE_STATIC_LIBS OFF)
set(BOOST_ROOT "A:/boostlib165")
set(BOOST_INCLUDEDIR "${BOOST_ROOT}/include")
if(MSVC_VERSION GREATER_EQUAL 1910)
    set(Boost_LIBRARY_DIR "${BOOST_ROOT}/lib64-msvc-14.1")
elseif(MSVC_VERSION EQUAL 1600)
    set(Boost_LIBRARY_DIR "${BOOST_ROOT}/lib64-msvc-10.0")
    add_definitions(-D_USING_V110_SDK71_)
endif()
find_package(Boost 1.65 REQUIRED program_options regex)
include_directories(${Boost_INCLUDE_DIRS}) 
link_directories(${Boost_LIBRARY_DIRS})
message("${Boost_LIBRARY_DIRS}")
message("${Boost_LIBRARIES}")

#Source Files:
add_library(${PACKAGE_NAME} SHARED
    prec.cpp
    prec.h
    util.cpp
    zen.cpp    
    op.cpp
    main.cpp
    stopgap.def
    stopgap.rc
)
#Add .H Files
target_sources(${PACKAGE_NAME} PRIVATE prec.h main.hpp op.hpp resource.h stopgap.hpp util.hpp zen.hpp)
#C++14 features:
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_14)
#Compile it.
target_link_libraries(${PACKAGE_NAME} PRIVATE
    kernel32
    user32
    ntdll
    version
    shlwapi
    advapi32
    msvcrtd
    msvcprtd
    Boost::boost
    Boost::dynamic_linking
    Boost::program_options
    Boost::regex
    ${Boost_LIBRARIES}
    zenwinx
    ntdll
)
#Set Link Flags:
#1.   Disable the default manifest because we provide our own in stopgap.rc resource file above^
#2.   Disable incremental build.
#3.   Set Subsystem
set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "/MANIFEST:NO /INCREMENTAL:NO /SUBSYSTEM:CONSOLE")