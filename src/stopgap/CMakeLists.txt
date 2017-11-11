# configurations
if(Debug)
	include_directories(
	)

	add_definitions(
		-D_CONSOLE
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(stopgap
		guicon.cpp
		main.cpp
		op.cpp
		util.cpp
		zen.cpp
	)

	link_directories(
		
	)

	target_link_libraries(stopgap
		kernel32
		user32
		ntdll
		version
		shlwapi
		advapi32
		msvcrtd
		msvcprtd
		$(NOINHERIT)
	)

endif()

if(Debug)
	include_directories(
	)

	add_definitions(
		-D_CONSOLE
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(stopgap
		guicon.cpp
		main.cpp
		op.cpp
		util.cpp
		zen.cpp
	)

	link_directories(
		C:\Software\Boost165\lib64-msvc-12.0
	)

	target_link_libraries(stopgap
		kernel32
		user32
		ntdll
		version
		shlwapi
		advapi32
		msvcrtd
		msvcprtd
		$(NOINHERIT)
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-D_CONSOLE
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(stopgap
		guicon.cpp
		main.cpp
		op.cpp
		util.cpp
		zen.cpp
	)

	link_directories(
		$(NOINHERIT)
	)

	target_link_libraries(stopgap
		kernel32
		user32
		ntdll
		version
		shlwapi
		advapi32
		msvcrt
		msvcprt
		$(NOINHERIT)
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-D_CONSOLE
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(stopgap
		guicon.cpp
		main.cpp
		op.cpp
		util.cpp
		zen.cpp
	)

	link_directories(
		C:\Boost\lib64
		C:\WinDDK\7600.16385.1\lib\win7\amd64
	)

	target_link_libraries(stopgap
		kernel32
		user32
		ntdll
		version
		shlwapi
		advapi32
		msvcrt
		msvcprt
		$(NOINHERIT)
	)

endif()

