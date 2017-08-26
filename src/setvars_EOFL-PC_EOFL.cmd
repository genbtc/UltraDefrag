@echo off
echo Set common environment variables...

:: UltraDefrag version
set VERSION_MAJOR=7
set VERSION_MINOR=0
set VERSION_REVISION=1

:: alpha1, beta2, rc3, etc.
:: unset for the final releases
set RELEASE_STAGE=test1

:: paths to development tools
set WINSDKBASE=C:\Program Files (x86)\Microsoft SDKs\Windows\v7.1A
set MINGWBASE=C:\Program Files (x86)\CodeBlocks\MinGW
set MINGWx64BASE=C:\Program Files\Git\mingw64
set WXWIDGETSDIR=C:\Software\wxWidgets-2.8.12
set NSISDIR=C:\Program Files (x86)\NSIS
set SEVENZIP_PATH=C:\Program Files (x86)\7-Zip\
set GNUWIN32_DIR=D:\DoesNotExist\Software\GnuWin32\bin
set CODEBLOCKS_EXE=C:\Program Files (x86)\CodeBlocks\codeblocks.exe

:: auxiliary stuff
set VERSION=%VERSION_MAJOR%,%VERSION_MINOR%,%VERSION_REVISION%,0
set VERSION2="%VERSION_MAJOR%, %VERSION_MINOR%, %VERSION_REVISION%, 0\0"
set ULTRADFGVER=%VERSION_MAJOR%.%VERSION_MINOR%.%VERSION_REVISION%
if "%RELEASE_STAGE%" neq "" (
    set UDVERSION_SUFFIX=%ULTRADFGVER%-%RELEASE_STAGE%
) else (
    set UDVERSION_SUFFIX=%ULTRADFGVER%
)
