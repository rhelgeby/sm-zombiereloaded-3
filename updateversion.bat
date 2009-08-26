@echo off
REM Note: Copy this script to the source code repositroy and execute it
REM       from that location.

REM To avoid complicated and maybe impossible tasks in windows batch scripts,
REM this script will execute another linux shell script using cygwin.

set BASH=%ZRDEVENV%\win32\bin\bash.exe
set DATEPATH=%ZRDEVENV%\win32\bin\date.exe

REM Converts a windows path to a linux path for cygwin (/cygdrive/c/...).
for /f %%s in ('%ZRDEVENV%\win32\bin\cygpath.exe -u "%DATEPATH%"') do set DATEPATH=%%s

%BASH% updateversion.sh %DATEPATH%
