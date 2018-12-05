@echo off

if not exist ".\bin" mkdir .\bin
pushd .\bin
cl -DEMEAKA_INTERNAL=1 -DEMEAKA_SLOW=1 -DEMEAKA_WIN32=1 -Zi -FC ..\src\win32_emeaka.cpp -I..\inc gdi32.lib user32.lib
popd