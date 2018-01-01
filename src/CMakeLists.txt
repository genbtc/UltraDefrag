cmake_minimum_required(VERSION 3.8)
#Include Dependencies (each is a sub-project):
add_subdirectory(lua5.1 ${CMAKE_BINARY_DIR}/lua5.1a)
add_subdirectory(dll/zenwinx ${CMAKE_BINARY_DIR}/zenwinx)
add_subdirectory(dll/udefrag ${CMAKE_BINARY_DIR}/udefrag)
add_subdirectory(stopgap ${CMAKE_BINARY_DIR}/stopgap)
add_subdirectory(wxgui ${CMAKE_BINARY_DIR}/wxgui)

#Declare the WXGUI project.
set( PACKAGE_NAME "UltraDefrag" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )
