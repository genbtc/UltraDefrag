@echo off

:: This script creates the doxygen docs.

echo Creating doxygen HTML docs
echo.
set PATH=C:\Program Files (x86)\Graphviz 2.28\bin;%PATH%
cd C:\Software\Udefrag\wxgui\doxygen
doxygen
cd C:\Software\Udefrag\native
doxygen
cd ..