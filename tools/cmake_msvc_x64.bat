@echo off
set CurrentPath=%~dp0

set PrjDIR=%CurrentPath%..\prj_msvc
set SrcDIR=%CurrentPath%..\src

echo cmake -S %SrcDIR% -B %PrjDIR%

cmake -S %SrcDIR% -B %PrjDIR% -G "Visual Studio 16 2019"   -DCMAKE_SYSTEM_VERSION=10.0  -Wno-dev

