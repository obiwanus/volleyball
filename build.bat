@echo off

pushd w:\volleyball

set CommonCompilerFlags= -DLL -MTd -nologo -Gm- -GR- -EHa- -Od -Oi -WX -W4 -wd4201 -wd4100 -wd4189 -wd4505 -wd4706 -DBUILD_INTERNAL=1 -DBUILD_SLOW=1 -DBUILD_WIN32=1 -FC -Z7 -Fm
set CommonLinkerFlags= -incremental:no -opt:ref winmm.lib user32.lib gdi32.lib

if not defined DevEnvDir (
    call misc\shell.bat
)

IF NOT EXIST ..\build mkdir ..\build
pushd ..\build

@del /Q *.pdb > NUL 2> NUL
@del /Q *.hmi > NUL 2> NUL

REM 32-bit build
REM cl %CommonCompilerFlags% ..\volleyball\src\win32_volleyball.cpp /link -subsystem:windows,5.1 %CommonLinkerFlags%

REM 64-bit build
cl %CommonCompilerFlags% ..\volleyball\src\volleyball.cpp -LD /link -incremental:no -PDB:volleyball_%random%.pdb /EXPORT:GameUpdateAndRender
cl %CommonCompilerFlags% ..\volleyball\src\win32_volleyball.cpp /link %CommonLinkerFlags%

popd
popd