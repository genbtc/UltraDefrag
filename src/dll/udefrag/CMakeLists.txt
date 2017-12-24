# configurations
cmake_minimum_required(VERSION 3.8)
set( PACKAGE_NAME "udefrag" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.3" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.3" )

include_directories( )

add_definitions(
	-D_NO_CRT_STDIO_INLINE
	-DBUILD_DLL
	-DUSE_WINSDK
	-D_CRT_SECURE_NO_WARNINGS
	-D_CONSOLE
	-DUDEFRAG_EXPORTS
)
if(Debug)
    add_definitions(-DATTACH_DEBUGGER)
endif()

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

add_library(${PACKAGE_NAME}
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
)

link_directories( )

target_link_libraries(${PACKAGE_NAME}
	ntdll
)
