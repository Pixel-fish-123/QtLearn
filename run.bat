@echo off
setlocal
set "QT=D:\Qt_Develop_Tool"
set "PATH=%QT%\6.11.2\mingw_64\bin;%PATH%"
set "QT_QPA_PLATFORM_PLUGIN_PATH=%QT%\6.11.2\mingw_64\plugins\platforms"
set "QT_PLUGIN_PATH=%QT%\6.11.2\mingw_64\plugins"
cd /d "%~dp0"
call build.bat || exit /b 1
start "" "%~dp0build\mingw-debug\QtLearn.exe"
