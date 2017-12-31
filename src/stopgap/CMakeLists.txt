# confitgurations CMake for Stopgap - (Dependencies: Boost) 
# created by genBTC - December 23 2017
cmake_minimum_required(VERSION 3.8)
#Package definitions
set( PACKAGE_NAME "stopgap" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.3" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.3" )

add_definitions(
	-D_CONSOLE
    -DUNICODE
    -D_UNICODE
)
#COMMON TO ALL DEBUG CONFIGS
if (Debug)
    add_definitions(
        -DDEBUG
        -D_DEBUG
    )
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PACKAGE_NAME}
	guicon.cpp
	main.cpp
	op.cpp
	util.cpp
	zen.cpp
    prec.cpp
)

#Include Boost, (mandatory)
set(Boost_USE_STATIC_LIBS ON)
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
message("${Boost_LIBRARIES}")
#C++14 features:
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_14)
#Compile it.
target_link_libraries(${PACKAGE_NAME} INTERFACE
	kernel32
	user32
	ntdll
	version
	shlwapi
	advapi32
	msvcrtd
	msvcprtd
    Boost::boost
    ${Boost_LIBRARIES}
)
