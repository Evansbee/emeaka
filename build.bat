@echo off

if not exist ".\bin" mkdir .\bin
pushd .\bin
cl -DEMEAKA_WIN32=1 -Zi -FC -DEMEAKA_WIN32=1 ..\src\win32_emeaka.cpp -I..\inc gdi32.lib user32.lib
popd