# configurations
if(Debug)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DATTACH_DEBUGGER
		-D_CONSOLE
		-DUDEFRAG_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(udefrag
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

	link_directories(
		
	)

	target_link_libraries(udefrag
		ntdll
		$(NOINHERIT)
	)

endif()

if(Debug)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DATTACH_DEBUGGER
		-D_CONSOLE
		-DUDEFRAG_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(udefrag
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

	link_directories(
		
	)

	target_link_libraries(udefrag
		ntdll
		$(NOINHERIT)
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DOFFICIAL_RELEASE
		-D_CONSOLE
		-DUDEFRAG_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(udefrag
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

	link_directories(
		
	)

	target_link_libraries(udefrag
		ntdll
		$(NOINHERIT)
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DOFFICIAL_RELEASE
		-D_CONSOLE
		-DUDEFRAG_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(udefrag
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

	link_directories(
		
	)

	target_link_libraries(udefrag
		ntdll
		$(NOINHERIT)
	)

endif()

