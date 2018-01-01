cmake_minimum_required(VERSION 3.8)
#Include Dependencies (each is a sub-project):

set(CMAKE_BINARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
message("${CMAKE_BINARY_OUTPUT_DIRECTORY}")

add_subdirectory(lua5.1 ${CMAKE_BINARY_DIR}/lua5.1a)        #Deps: None
add_subdirectory(dll/zenwinx ${CMAKE_BINARY_DIR}/zenwinx)   #Deps: ntdll
add_subdirectory(dll/udefrag ${CMAKE_BINARY_DIR}/udefrag)   #Deps: zenwinx, ntdll
add_subdirectory(stopgap ${CMAKE_BINARY_DIR}/stopgap)       #Deps: Boost, msvcrtd, msvcprtd, ntdll
add_subdirectory(wxgui ${CMAKE_BINARY_DIR}/wxgui)       #Deps: wxWidgets udefrag zenwinx lua5.1a stopgap

#Declare the WXGUI project.
set( PACKAGE_NAME "UltraDefrag" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )
