# CMakeLists.txt configurations - for UltraDefrag's wxGUI based on WXWIDGETS
#Dependencies (external) = WxWidgets (for wxGUI), Boost 1.65 (for stopgap.dll)
#internal dependencies: udefrag.dll, zenwinx.dll, stopgap.dll, and lua5.1
#created by genBTC - January 1, 2018
cmake_minimum_required(VERSION 3.8)

#Declare the WXGUI project.
set( PACKAGE_NAME "wxgui" )
project(${PACKAGE_NAME})
set( PACKAGE_STRING "${PACKAGE_NAME} 7.0.4" )
set( PACKAGE_TARNAME "${PACKAGE_NAME}" )
set( PACKAGE_VERSION "7.0.4" )

set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

#Set your $WXWIDGETS env variable or just set the dir here:
set(WXWIDGETS "A:/wxWidgets")
set(wxWidgets_ROOT_DIR "${WXWIDGETS}")
message("${wxWidgets_ROOT_DIR}")
#TODO: Replace this forced hack if/when FindwxWidgets.cmake Line 501 
#       is changed from if(MSVC_VERSION EQUAL 1910) to GREATER_EQUAL
#So it can find the toolset as 141 (otherwise it detects as 1912 in VC2017 Preview6)
if(MSVC_VERSION GREATER_EQUAL 1910)
    set(MSVC_VERSION 1910)
elseif(MSVC_VERSION EQUAL 1600)
    set(wxWidgets_LIB_DIR "${wxWidgets_ROOT_DIR}/lib/vcWindows7.1SDK_x64_dll")
    message("${wxWidgets_LIB_DIR}")
    add_definitions(-D_USING_V110_SDK71_)
endif()
set(wxWidgets_USE_UNICODE 1)    #use U always
set(BUILD_SHARED_LIBS 1)    #use dlls 

add_definitions(
    -DOFFICIAL_RELEASE
    -DUNICODE
    -D_UNICODE
    -DWXUSINGDLL
    -D_CRTDBG_MAP_ALLOC
    -DWIN32
    -D_WIN32
    -D_WINDOWS
    -D_WINDLL
)
#/Zi /nologo /W3 /WX- /Od /GF /Gm /EHsc /RTC1 /MDd /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Yu"prec.h" /Fp"x64\Debug\UltraDefrag.pch" /Fo"x64\Debug\\" /Fd"x64\Debug\vcWindows7.1SDK.pdb" /Gd /TP
#add_compile_options(/Zi /nologo /W3 /WX- /Od /GF /Gm /EHsc /MDd /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /TP)
set(CMAKE_CXX_FLAGS_DEBUG "/Zi /nologo /W3 /WX- /Ob0 /Od /GF /Gm /EHsc /MDd /GS /Gy /fp:precise /Zc:wchar_t /Zc:forScope /Gd /TP")
#COMMON TO ALL DEBUG CONFIGS
if (Debug)
    set(wxWidgets_USE_DEBUG 1)
    add_definitions(
        -DDEBUG
        -D_DEBUG
        -D__WXDEBUG__
    )
endif()
#WXWidgets is required.
find_package(wxWidgets REQUIRED net adv core base)
if(wxWidgets_FOUND)
    include(${wxWidgets_USE_FILE})  #wx/setup.h?
endif()

#if using Microsoft VS2017 Cmake
if(MSVC)
    #Required. fixes the wxUSE_WEBVIEW has no backend error
    add_definitions(
        -D__WXMSW__
    )
endif()


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
    LegendTransientPopup.cpp
    log.cpp
    main.cpp
    map.cpp
    menu.cpp
    prec.cpp
    prec.h
    query.cpp
    report.cpp
    shutdown.cpp    
    statbar.cpp
    taskbar.cpp
    toolbar.cpp
    tray.cpp
    upgrade.cpp
    utils.cpp
    vollist.cpp
    wxgui.rc
)
target_sources(${PACKAGE_NAME} PRIVATE prec.h)
#Disable the default manifest because we provide our own in wxgui.rc resource file above^
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
#C++11 features:
target_compile_features(${PACKAGE_NAME} INTERFACE cxx_std_14)
#Link Libraries, Make the EXE
target_link_libraries(${PACKAGE_NAME} PUBLIC ${wxWidgets_LIBRARIES} udefrag zenwinx lua5.1a stopgap)
set_target_properties(${PACKAGE_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")