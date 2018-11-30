@echo off

if not exist ".\bin" mkdir .\bin
pushd .\bin
cl -Zi -DEMEAKA_WIN32=1 ..\src\emeaka_platform_win32.cpp -I..\inc gdi32.lib user32.lib
popd