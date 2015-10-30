@echo off

::
:: This script builds all binaries for UltraDefrag project
:: for all of the supported target platforms.
:: Copyright (c) 2007-2013 Dmitri Arkhangelski (dmitriar@gmail.com).
:: Copyright (c) 2010-2013 Stefan Pendl (stefanpe@users.sourceforge.net).
::
:: This program is free software; you can redistribute it and/or modify
:: it under the terms of the GNU General Public License as published by
:: the Free Software Foundation; either version 2 of the License, or
:: (at your option) any later version.
::
:: This program is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
:: GNU General Public License for more details.
::
:: You should have received a copy of the GNU General Public License
:: along with this program; if not, write to the Free Software
:: Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
::

rem Usage:
rem     build-targets [<compiler>]
rem
rem Available <compiler> values:
rem     --use-mingw     (default)
rem     --use-winsdk    (we use it for official releases)
rem     --use-mingw-x64 (experimental, produces wrong x64 code)
rem
rem Skip any processor architecture to reduce compile time
rem     --no-x86
rem     --no-amd64
rem     --no-ia64

rem NOTE: IA-64 targeting binaries were never tested by the authors 
rem due to missing appropriate hardware and appropriate 64-bit version 
rem of Windows.

rem call ParseCommandLine.cmd %*

:: create all directories required to store target binaries
rem mkdir lib
rem mkdir lib\amd64
rem mkdir lib\ia64
rem mkdir bin
rem mkdir bin\amd64
rem mkdir bin\ia64

rem :: copy source files to obj directory
rem (
rem     echo doxyfile
rem     echo .dox
rem     echo .html
rem     echo .mdsp
rem     echo .cbp
rem     echo .depend
rem     echo .layout
rem ) >"%~n0_exclude.txt"
rem 
rem xcopy .\bootexctrl  .\obj\bootexctrl  /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\console     .\obj\console     /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\console\res .\obj\console\res /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\dll\udefrag .\obj\udefrag     /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\dll\zenwinx .\obj\zenwinx     /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\wxgui       .\obj\wxgui       /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\wxgui\res   .\obj\wxgui\res   /I /Y /Q /S /EXCLUDE:%~n0_exclude.txt
rem xcopy .\hibernate   .\obj\hibernate   /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\include     .\obj\include     /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\lua5.1      .\obj\lua5.1      /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\lua         .\obj\lua         /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\lua-gui     .\obj\lua-gui     /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem xcopy .\native      .\obj\native      /I /Y /Q /EXCLUDE:%~n0_exclude.txt
rem 
rem del /f /q "%~n0_exclude.txt"

:: copy external files on which monolithic native interface depends
rem copy /Y .\obj\native\udefrag.c .\obj\native\udefrag-native.c
rem copy /Y .\obj\udefrag\*.* .\obj\native\
rem del /Q .\obj\native\udefrag.rc
rem copy /Y .\obj\native\volume.c .\obj\native\udefrag-volume.c
rem copy /Y .\obj\zenwinx\*.* .\obj\native\
rem del /Q .\obj\native\zenwinx.rc

:: copy header files to different locations
:: to make relative paths of them the same
:: as in /src directory
rem mkdir obj\dll
rem mkdir obj\dll\udefrag
rem copy /Y obj\udefrag\udefrag.h obj\dll\udefrag
rem mkdir obj\dll\zenwinx
rem copy /Y obj\zenwinx\*.h obj\dll\zenwinx\

:: build list of headers to produce dependencies
:: for MinGW/SDK makefiles from
cd native
dir /S /B *.h >headers || exit /B 1
rem dir /S /B *.h >headers || exit /B 1
rem copy /Y .\headers .\bootexctrl || exit /B 1
rem copy /Y .\headers .\console    || exit /B 1
rem copy /Y .\headers .\udefrag    || exit /B 1
rem copy /Y .\headers .\zenwinx    || exit /B 1
rem copy /Y .\headers .\wxgui      || exit /B 1
rem copy /Y .\headers .\hibernate  || exit /B 1
rem copy /Y .\headers .\lua5.1     || exit /B 1
rem copy /Y .\headers .\lua        || exit /B 1
rem copy /Y .\headers .\lua-gui    || exit /B 1
rem copy /Y .\headers .\native     || exit /B 1
rem cd ..

:: let's build all modules by selected compiler
rem if %UD_BLD_FLG_USE_COMPILER% equ 0 (
rem     echo No parameters specified, using defaults.
rem     goto mingw_build
rem )
rem 
rem if %UD_BLD_FLG_USE_COMPILER% equ %UD_BLD_FLG_USE_MINGW%   goto mingw_build
rem if %UD_BLD_FLG_USE_COMPILER% equ %UD_BLD_FLG_USE_MINGW64% goto mingw_x64_build
rem if %UD_BLD_FLG_USE_COMPILER% equ %UD_BLD_FLG_USE_WINSDK%  goto winsdk_build
rem 
rem :winsdk_build
rem 
rem     set BUILD_ENV=winsdk
rem     set OLD_PATH=%path%
rem 
rem     if %UD_BLD_FLG_BUILD_X86% neq 0 (
rem         echo --------- Target is x86 ---------
rem         set AMD64=
rem         set IA64=
rem         pushd ..
rem         call "%WINSDKBASE%\bin\SetEnv.Cmd" /Release /x86 /xp
rem         popd
rem         set UDEFRAG_LIB_PATH=..\..\lib
rem         call :build_modules X86 || exit /B 1
rem     )
rem     
rem     set path=%OLD_PATH%
rem 
rem     if %UD_BLD_FLG_BUILD_AMD64% neq 0 (
rem         echo --------- Target is x64 ---------
rem         set IA64=
rem         set AMD64=1
rem         pushd ..
rem         call "%WINSDKBASE%\bin\SetEnv.Cmd" /Release /x64 /xp
rem         popd
rem         set UDEFRAG_LIB_PATH=..\..\lib\amd64
rem         call :build_modules amd64 || exit /B 1
rem     )
rem     
rem     set path=%OLD_PATH%
rem 
rem     if %UD_BLD_FLG_BUILD_IA64% neq 0 (
rem         echo --------- Target is ia64 ---------
rem         set AMD64=
rem         set IA64=1
rem         pushd ..
rem         call "%WINSDKBASE%\bin\SetEnv.Cmd" /Release /ia64 /xp
rem         popd
rem         set BUILD_DEFAULT=-nmake -i -g -P
rem         set UDEFRAG_LIB_PATH=..\..\lib\ia64
rem         call :build_modules ia64 || exit /B 1
rem     )
rem     
rem     :: remove perplexing manifests
rem     del /S /Q .\bin\*.manifest
rem     
rem     :: get rid of annoying dark green color
rem     color
rem     
rem     set path=%OLD_PATH%
rem     set OLD_PATH=
rem     
rem exit /B 0
rem 
rem 
rem :mingw_x64_build
rem 
rem     set OLD_PATH=%path%
rem 
rem     echo --------- Target is x64 ---------
rem     set AMD64=1
rem     set path=%MINGWx64BASE%\bin;%path%
rem     set BUILD_ENV=mingw_x64
rem     set UDEFRAG_LIB_PATH=..\..\lib\amd64
rem     call :build_modules amd64 || exit /B 1
rem 
rem     set path=%OLD_PATH%
rem     set OLD_PATH=
rem 
rem exit /B 0
rem 
rem 
rem :mingw_build

set OLD_PATH=%path%

set path=%MINGWBASE%\bin;%path%
set BUILD_ENV=mingw
set UDEFRAG_LIB_PATH=..\..\lib
call :build_modules X86 || exit /B 1

cd ..
exit /B 0


:: Builds all UltraDefrag modules
:: Example: call :build_modules X86
:build_modules
    rem update manifests
    rem call make-manifests.cmd %1 || exit /B 1

    if %WindowsSDKVersionOverride%x neq v7.1x goto NoWin7SDK
    if x%CommandPromptType% neq xCross goto NoWin7SDK
    set path=%PATH%;%VS100COMNTOOLS%\..\..\VC\Bin

    :NoWin7SDK
    rem rebuild modules
    set UD_BUILD_TOOL=lua ..\tools\mkmod.lua
    set WXWIDGETS_INC_PATH=%WXWIDGETSDIR%\include
    set WX_CONFIG=%BUILD_ENV%-%1
    if %BUILD_ENV% equ winsdk if %1 equ X86 (
        set WXWIDGETS_LIB_PATH=%WXWIDGETSDIR%\lib\vc_lib%WX_CONFIG%
    )
    if %BUILD_ENV% equ winsdk if %1 equ amd64 (
        set WXWIDGETS_LIB_PATH=%WXWIDGETSDIR%\lib\vc_amd64_lib%WX_CONFIG%
    )
    if %BUILD_ENV% equ winsdk if %1 equ ia64 (
        set WXWIDGETS_LIB_PATH=%WXWIDGETSDIR%\lib\vc_ia64_lib%WX_CONFIG%
    )
    if %BUILD_ENV% equ mingw (
        set WXWIDGETS_LIB_PATH=%WXWIDGETSDIR%\lib\gcc_lib%WX_CONFIG%
    )
    if %BUILD_ENV% equ mingw_x64 (
        set WXWIDGETS_LIB_PATH=%WXWIDGETSDIR%\lib\gcc_lib%WX_CONFIG%
    )
    set WXWIDGETS_INC2_PATH=%WXWIDGETS_LIB_PATH%\mswu
    
    rem pushd obj\native
    %UD_BUILD_TOOL% native.build || goto fail
    rem pushd obj\zenwinx
    rem %UD_BUILD_TOOL% zenwinx.build || goto fail
    rem cd ..\udefrag
    rem %UD_BUILD_TOOL% udefrag.build || goto fail
    rem echo Compile monolithic native interface...
    rem cd ..\native
    rem %UD_BUILD_TOOL% defrag_native.build || goto fail
    rem cd ..\lua5.1
    rem %UD_BUILD_TOOL% lua.build || goto fail
    rem cd ..\lua
    rem %UD_BUILD_TOOL% lua.build || goto fail
    rem cd ..\lua-gui
    rem %UD_BUILD_TOOL% lua-gui.build || goto fail
    rem cd ..\bootexctrl
    rem %UD_BUILD_TOOL% bootexctrl.build || goto fail
    rem cd ..\hibernate
    rem %UD_BUILD_TOOL% hibernate.build || goto fail
    rem cd ..\console
    rem %UD_BUILD_TOOL% console.build || goto fail
    rem cd ..\wxgui
    rem %UD_BUILD_TOOL% wxgui.build || goto fail

    rem :success
    rem set UD_BUILD_TOOL=
    rem set WX_CONFIG=
    rem set WXWIDGETS_INC_PATH=
    rem set WXWIDGETS_INC2_PATH=
    rem set WXWIDGETS_LIB_PATH=
    rem popd
    rem exit /B 0
    rem 
    rem :fail
    rem set UD_BUILD_TOOL=
    rem set WX_CONFIG=
    rem set WXWIDGETS_INC_PATH=
    rem set WXWIDGETS_INC2_PATH=
    rem set WXWIDGETS_LIB_PATH=
    rem popd
    rem exit /B 1
    :fail
    cd ..
    exit /B 1
    
exit /B 0
