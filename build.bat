@echo off
setlocal
set "QT=D:\Qt_Develop_Tool"
set "CMAKE=D:\Qt_Develop_Tool\Tools\CMake_64\bin"
set "PATH=%QT%\Tools\Ninja;%CMAKE%;%PATH%"
cd /d "%~dp0"
cmake --preset mingw-debug || exit /b 1
cmake --build --preset mingw-debug || exit /b 1