@echo off
rem Name:        build_oracle.cmd
rem Description: builds MAME's V60 disassembler as a standalone reference tool (MSVC)
rem Version:     0.1.0
rem Created:     11-10-2026
rem Modified:    11-10-2026
rem Author:      Sebastian Toyos
setlocal EnableDelayedExpansion
set "HERE=%~dp0"
set "OUT=%HERE%..\..\build\reference"
if not exist "%OUT%" mkdir "%OUT%"

rem Delayed expansion keeps the ")" of "Program Files (x86)" from closing the for clause
set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"
set "VS="
for /f "usebackq delims=" %%i in (`"!VSWHERE!" -latest -products * -property installationPath`) do set "VS=%%i"
if not defined VS (
	echo Visual Studio Build Tools not found
	exit /b 1
)
rem vcvars64 calls vswhere by name, so its folder has to be on PATH
for %%d in ("!VSWHERE!") do set "PATH=!PATH!;%%~dpd"
call "!VS!\VC\Auxiliary\Build\vcvars64.bat" >nul || exit /b 1

cl /nologo /std:c++17 /O2 /EHsc /W3 /wd4996 /I"%HERE%stub" /I"%HERE%mame" /Fo"%OUT%\\" /Fe"%OUT%\oracle.exe" "%HERE%oracle.cpp" "%HERE%mame\v60d.cpp"
