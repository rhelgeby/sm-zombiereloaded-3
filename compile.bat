@echo off

set SOURCEDIR=src
set SMINCLUDES=env\include
set BUILDDIR=build
set SPCOMP=env\win32\bin\spcomp-1.7.0.exe
set VERSIONDUMP=updateversion.bat

:: Dump version and revision information first.
echo Updating version and revision info...
start /wait %VERSIONDUMP%

:: Make build directory.
if not exist "%BUILDDIR%" (
    mkdir %BUILDDIR%
)

:: Compile.
echo Starting compiler:
%SPCOMP% -i%SOURCEDIR% -i%SOURCEDIR%/include -i%SMINCLUDES% -o%BUILDDIR%/zombiereloaded.smx %SOURCEDIR%\zombiereloaded.sp

echo Compiling done. This script is looped, close if you're done.
pause

compile.bat
