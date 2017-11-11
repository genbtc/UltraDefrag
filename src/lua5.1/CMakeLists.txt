# configurations
if(Debug)
	include_directories(
	)

	add_definitions(
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DATTACH_DEBUGGER
		-D$(NOINHERIT)
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(lua5.1a
		lapi.c
		lauxlib.c
		lbaselib.c
		lcode.c
		ldblib.c
		ldebug.c
		ldo.c
		ldump.c
		lfunc.c
		lgc.c
		linit.c
		liolib.c
		llex.c
		lmathlib.c
		lmem.c
		loadlib.c
		lobject.c
		lopcodes.c
		loslib.c
		lparser.c
		lstate.c
		lstring.c
		lstrlib.c
		ltable.c
		ltablib.c
		ltm.c
		lundump.c
		lvm.c
		lzio.c
	)

	link_directories(
		
	)

endif()

if(Debug)
	include_directories(
	)

	add_definitions(
		-DWIN64
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
        -DATTACH_DEBUGGER
		-D$(NOINHERIT)
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(lua5.1a
		lapi.c
		lauxlib.c
		lbaselib.c
		lcode.c
		ldblib.c
		ldebug.c
		ldo.c
		ldump.c
		lfunc.c
		lgc.c
		linit.c
		liolib.c
		llex.c
		lmathlib.c
		lmem.c
		loadlib.c
		lobject.c
		lopcodes.c
		loslib.c
		lparser.c
		lstate.c
		lstring.c
		lstrlib.c
		ltable.c
		ltablib.c
		ltm.c
		lundump.c
		lvm.c
		lzio.c
	)

	link_directories(
		
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DOFFICIAL_RELEASE
		-D$(NOINHERIT)
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(lua5.1a
		lapi.c
		lauxlib.c
		lbaselib.c
		lcode.c
		ldblib.c
		ldebug.c
		ldo.c
		ldump.c
		lfunc.c
		lgc.c
		linit.c
		liolib.c
		llex.c
		lmathlib.c
		lmem.c
		loadlib.c
		lobject.c
		lopcodes.c
		loslib.c
		lparser.c
		lstate.c
		lstring.c
		lstrlib.c
		ltable.c
		ltablib.c
		ltm.c
		lundump.c
		lvm.c
		lzio.c
	)

	link_directories(
		
	)

endif()

if(Release)
	include_directories(
	)

	add_definitions(
		-DUSE_WINSDK
		-D_CRT_SECURE_NO_WARNINGS
		-DOFFICIAL_RELEASE
        -D$(NOINHERIT)
	)

	set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)
	set(ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/lib)

	add_library(lua5.1a
		lapi.c
		lauxlib.c
		lbaselib.c
		lcode.c
		ldblib.c
		ldebug.c
		ldo.c
		ldump.c
		lfunc.c
		lgc.c
		linit.c
		liolib.c
		llex.c
		lmathlib.c
		lmem.c
		loadlib.c
		lobject.c
		lopcodes.c
		loslib.c
		lparser.c
		lstate.c
		lstring.c
		lstrlib.c
		ltable.c
		ltablib.c
		ltm.c
		lundump.c
		lvm.c
		lzio.c
	)

	link_directories(
		
	)

endif()

