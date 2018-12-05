@echo off
REM Check how teh windows subsystem works --  5.1 is for XP
if not exist ".\bin" mkdir .\bin
pushd .\bin
cl -DEMEAKA_INTERNAL=1 -DEMEAKA_SLOW=1 -DEMEAKA_WIN32=1 -nologo -MT -Gm- -GR- -EHa -Oi -W4 -WX -wd4201 -wd4100 -Zi -FC -Fmwin32_emeaka.map ..\src\win32_emeaka.cpp -I..\inc /link -opt:ref -subsystem:windows,5.2 gdi32.lib user32.lib
popd