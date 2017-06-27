@echo off

REM Build for Visual Studio compiler. Run your copy of vcvarsall.bat to setup command-line compiler.

REM Check if build tool is on path
REM >nul, 2>nul will remove the output text from the where command
where /q cl.exe
if %errorlevel%==1 (
	echo MSVC CL not on path, please add it to path to build by command line.
	goto end
)

set ProjectName=MinimalWin32OpenGL
IF NOT EXIST bin mkdir bin
pushd bin

REM ////////////////////////////////////////////////////////////////////////////
REM Compile Switches
REM ////////////////////////////////////////////////////////////////////////////
REM EHa-   disable exception handling
REM GR-    disable c runtime type information (we don't use) (i.e. typeof, dynamic cast)
REM Oi     enable intrinsics optimisation, let us use CPU intrinsics if there is one
REM        instead of generating a call to external library (i.e. CRT).
REM Zi     enables debug data, Z7 combines the debug files into one.
REM W4     warning level 4
REM WX     treat warnings as errors
REM wd4201 nonstandard extension used: nameless struct/union
REM wd4505 unreferenced local function not used will be removed
set CompileFlags=-EHha- -GR- -Oi -Z7 -W4 -WX -wd4201 -wd4505 -FAsc
set Win32Flags=/Fm%ProjectName% /Fe%ProjectName%

REM Link libraries
set LinkLibraries=user32.lib kernel32.lib gdi32.lib opengl32.lib

REM incremental:no,   turn incremental builds off
REM opt:ref,          try to remove functions from libs that are not referenced at all
set LinkFlags=-incremental:no -opt:ref -subsystem:WINDOWS -machine:x64 -nologo

set DebugMode=1
if %DebugMode%==1 goto :DebugFlags
goto :ReleaseFlags

REM MT     use static runtime library, so build and link it into exe
REM Od     disables optimisations
REM RTC1   runtime error checks, only possible with optimisations disabled

REM opt:icf,          COMDAT folding for debugging release build
REM DEBUG:[FULL|NONE] enforce debugging for release build
:DebugFlags
set CompileFlags=%CompileFlags% -Od -RTC1 -MTd
goto compile

:ReleaseFlags
set CompileFlags=%CompileFlags% -O2 -MT
set LinkFlags=%LinkFlags%

REM ////////////////////////////////////////////////////////////////////////////
REM Compile
REM ////////////////////////////////////////////////////////////////////////////
:compile
cl %CompileFlags% %Win32Flags% ..\Win32.cpp /link %LinkLibraries% %LinkFlags%

popd
set LastError=%ERRORLEVEL%

:end
exit /b %LastError%
