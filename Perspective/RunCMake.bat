@echo off

set BUILD_CONFIG=%1
set BUILD_DIR=%2
set CMAKE_EXE="tools\\cmake\\bin\\cmake.exe"

IF NOT DEFINED BUILD_CONFIG (SET BUILD_CONFIG="RelWithDebInfo") 
IF NOT DEFINED BUILD_DIR (SET BUILD_DIR="vsbuild") 

%CMAKE_EXE% -G "Visual Studio 16 2019" -A x64 -S . -B %BUILD_DIR%


