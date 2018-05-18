@REM Build for Visual Studio compiler. Run your copy of vcvars32.bat or
@REM vcvarsall.bat to setup command-line compiler.

@echo OFF
set ProjectName=dqn_unit_test
set CompileEntryPoint=..\dqn_unit_test.cpp

REM Build tags file if you have ctags in path
where /q ctags
if %errorlevel%==0 (
REM When parsing a C++ member function definition (e.g. "className::function"),
REM ctags cannot determine whether the scope specifier is a class name or
REM a namespace specifier and always lists it as a class name in the scope
REM portion of the extension fields. Also, if a C++ function is defined outside
REM of the class declaration (the usual case), the access specification (i.e.
REM public, protected, or private) and implementation information (e.g. virtual,
REM pure virtual) contained in the function declaration are not known when the
REM tag is generated for the function definition. -c++-kinds=+p fixes that

REM The --fields=+iaS option:
REM    a   Access (or export) of class members
REM    i   Inheritance information
REM    S   Signature of routine (e.g. prototype or parameter list)
REM
REM The --extra=+q option:
REM    By default, ctags only generates tags for separate identifiers found in
REM    the source files. With --extras=+q option, then ctags will also generate
REM    a second, class-qualified tag for each class member
	ctags -R --c++-kinds=+p --fields=+iaS --extras=+q
)

REM Check if build tool is on path
REM >nul, 2>nul will remove the output text from the where command
where cl.exe >nul 2>nul
if %errorlevel%==1 call msvc86.bat

REM Drop compilation files into build folder
IF NOT EXIST bin mkdir bin
pushd bin

REM EHa- disable exception handling (but we use for algorithms so /EHsc)
REM GR- disable c runtime type information (we don't use)

REM MD use dynamic runtime library
REM MT use static runtime library, so build and link it into exe

REM Oi enable intrinsics optimisation, let us use CPU intrinsics if there is one
REM instead of generating a call to external library (i.e. CRT).

REM Zi enables debug data, Z7 combines the debug files into one.

REM W4 warning level 4
REM WX treat warnings as errors
REM wd4201 ignore: nonstandard extension used: nameless struct/union
set CompileFlags=-EHsc -GR- -Oi -MT -Z7 -W4 -WX -wd4201 -FC -Od -wd4127

REM Include directories
set IncludeFlags=

REM Link libraries
set LinkLibraries=user32.lib

REM incrmenetal:no, turn incremental builds off
REM opt:ref, try to remove functions from libs that are referenced at all
set LinkFlags=-incremental:no -opt:ref -machine:x64

cl %CompileFlags% %CompileEntryPoint% %IncludeFlags% /link %LinkLibraries% %LinkFlags% /nologo /OUT:"%ProjectName%.exe"
REM cl /P /c %CompileFlags% %CompileEntryPoint%

popd

