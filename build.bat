@echo off
REM Check how teh windows subsystem works --  5.1 is for XP       -subsystem:windows,5.2


set CommonCompilerFlags=-nologo -MT -Gm- -GR- -EHa -Oi -W4 -WX -wd4201 -wd4189 -wd4100 -wd4505 -Zi -FC 
set EmeakaCompilerFlags=-DEMEAKA_INTERNAL=1 -DEMEAKA_SLOW=1 -DEMEAKA_WIN32=1
set CommonLinkerFlags=-opt:ref gdi32.lib user32.lib winmm.lib
set LinkOpts64Bit=-subsystem:windows,5.2
set LinkOpts32Bit=-subsystem:windows,5.1


if not exist ".\bin" mkdir .\bin
pushd .\bin
cl  %EmeakaCompilerFlags% %CommonCompilerFlags% -Fmwin32_emeaka.map ..\src\win32_emeaka.cpp -I..\inc /link %LinkOpts64Bit% %CommonLinkerFlags%
REM cl  %EmeakaCompilerFlags% %CommonCompilerFlags% -Fmemeaka.map /ld ..\src\emeaka.cpp -I..\inc /link %LinkOpts64Bit% %CommonLinkerFlags%
popd