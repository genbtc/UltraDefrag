# configurations
if(Debug)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-D_CRT_NON_CONFORMING_SWPRINTFS
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-D_CONSOLE
		-DZENWINX_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(zenwinx
		dbg.c
		entry.c
		env.c
		event.c
		file.c
		ftw.c
		ftw_ntfs.c
		int64.c
		keyboard.c
		keytrans.c
		ldr.c
		list.c
		lock.c
		mem.c
		misc.c
		mutex.c
		path.c
		prb.c
		privilege.c
		reg.c
		stdio.c
		string.c
		thread.c
		time.c
		utf8.c
		volume.c
		zenwinx.c
	)

	link_directories(
		..\bin
		..\lib
		$(SolutionDir)$(Configuration)
	)

	target_link_libraries(zenwinx
		ntdll
		$(NOINHERIT)
	)

endif()

if(Debug)
	include_directories(
	)

	add_definitions(
		-D_NO_CRT_STDIO_INLINE
		-D_CRT_NON_CONFORMING_SWPRINTFS
		-DBUILD_DLL
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-D_CONSOLE
		-DZENWINX_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(zenwinx
		dbg.c
		entry.c
		env.c
		event.c
		file.c
		ftw.c
		ftw_ntfs.c
		int64.c
		keyboard.c
		keytrans.c
		ldr.c
		list.c
		lock.c
		mem.c
		misc.c
		mutex.c
		path.c
		prb.c
		privilege.c
		reg.c
		stdio.c
		string.c
		thread.c
		time.c
		utf8.c
		volume.c
		zenwinx.c
	)

	link_directories(
		..\bin
		..\lib
		$(SolutionDir)$(Configuration)
	)

	target_link_libraries(zenwinx
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
		-D_CONSOLE
		-DZENWINX_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(zenwinx
		dbg.c
		entry.c
		env.c
		event.c
		file.c
		ftw.c
		ftw_ntfs.c
		int64.c
		keyboard.c
		keytrans.c
		ldr.c
		list.c
		lock.c
		mem.c
		misc.c
		mutex.c
		path.c
		prb.c
		privilege.c
		reg.c
		stdio.c
		string.c
		thread.c
		time.c
		utf8.c
		volume.c
		zenwinx.c
	)

	link_directories(
		
	)

	target_link_libraries(zenwinx
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
		-D_CONSOLE
		-DZENWINX_EXPORTS
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(zenwinx
		dbg.c
		entry.c
		env.c
		event.c
		file.c
		ftw.c
		ftw_ntfs.c
		int64.c
		keyboard.c
		keytrans.c
		ldr.c
		list.c
		lock.c
		mem.c
		misc.c
		mutex.c
		path.c
		prb.c
		privilege.c
		reg.c
		stdio.c
		string.c
		thread.c
		time.c
		utf8.c
		volume.c
		zenwinx.c
	)

	link_directories(
		
	)

	target_link_libraries(zenwinx
		ntdll
		$(NOINHERIT)
	)

endif()

