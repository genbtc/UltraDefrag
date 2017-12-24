# CMakeLists.txt configurations - for UltraDefrag's wxGUI based on WXWIDGETS
#Dependencies (external) = WxWidgets (for wxGUI), Boost 1.65 (for stopgap.dll)
#internal dependencies: udefrag.dll, zenwinx.dll, stopgap.dll, and lua5.1
#created by genBTC - December 23, 2017
cmake_minimum_required(VERSION 3.8)
#Include Dependencies (each is a sub-project):
add_subdirectory(../dll/zenwinx ${CMAKE_BINARY_DIR}/zenwinx EXCLUDE_FROM_ALL)
add_subdirectory(../dll/udefrag ${CMAKE_BINARY_DIR}/udefrag EXCLUDE_FROM_ALL)
add_subdirectory(../lua5.1 ${CMAKE_BINARY_DIR}/lua5.1a EXCLUDE_FROM_ALL)
add_subdirectory(../stopgap ${CMAKE_BINARY_DIR}/stopgap EXCLUDE_FROM_ALL)

#Declare the WXGUI project.
set( PACKAGE_NAME "wxgui" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.3" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.3" )

#set(CMAKE_BINARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})
#set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR})

#Set your $WXWIDGETS env variable or just set the dir here:
set(wxWidgets_ROOT_DIR ${WXWIDGETS})
set(wxWidgets_USE_UNICODE 1)    #use U always
set(BUILD_SHARED_LIBS 1)    #use dlls 
#COMMON TO ALL DEBUG CONFIGS
if (Debug)
    set(wxWidgets_USE_DEBUG 1)
    add_definitions(
        -DDEBUG
        -D_DEBUG
    )
endif()
#WXWidgets is required.
find_package(wxWidgets REQUIRED net gl core base)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})  #wx/setup.h?
endif()

#if using Microsoft VS2017 Cmake
if(MSVC)
    #fixes the wxUSE_WEBVIEW has no backend error
    add_definitions(
        -D__WXMSW__
    )
endif()

################# Flags ################
# Defines Flags for Windows and Linux. # 
########################################
#(copied from somewhere, edit as you please)
#if(MSVC)
#   set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /W3 /MD /Zi /EHsc")
#   set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /W3 /GL /Gy")
#endif(MSVC)
#if(NOT MSVC)
#   set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
#   if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
#       set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
#   endif()
#endif(NOT MSVC)

#This is being wrongly propagated to everything for some reason
add_definitions(
	-DOFFICIAL_RELEASE
    -DUNICODE
    -D_UNICODE
)

#Compile the EXE
add_executable(${PACKAGE_NAME} WIN32 
	about.cpp
	btd.cpp
	config.cpp
	crash.cpp
	fileslist.cpp
	i18n.cpp
	job.cpp
	LCN.cpp
	log.cpp
	main.cpp
	map.cpp
	menu.cpp
	prec.cpp
	query.cpp
	report.cpp
	shutdown.cpp
	LegendTransientPopup.cpp
	statbar.cpp
	taskbar.cpp
	toolbar.cpp
	tray.cpp
	upgrade.cpp
	utils.cpp
	vollist.cpp
)
#C++11 features:
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_11)
#Link Libraries
target_link_libraries(${PACKAGE_NAME} ${wxWidgets_LIBRARIES} udefrag zenwinx lua5.1a stopgap)