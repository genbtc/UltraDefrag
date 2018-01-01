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
)
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
endif()
find_package(Boost 1.65 REQUIRED program_options regex)
include_directories(${Boost_INCLUDE_DIRS}) 
link_directories(${Boost_LIBRARY_DIRS})
message("${Boost_LIBRARY_DIRS}")
message("${Boost_LIBRARIES}")

#Source Files:
add_library(${PACKAGE_NAME} SHARED
    prec.cpp
    util.cpp
    zen.cpp    
    op.cpp
    main.cpp
    stopgap.def
    stopgap.rc
)
#Disable the default manifest because we provide our own in stopgap.rc resource file above^
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
#C++14 features:
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_14)
#Compile it.
target_link_libraries(${PACKAGE_NAME} PUBLIC
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
