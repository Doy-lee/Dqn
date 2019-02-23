@echo OFF

if not exist ..\Bin mkdir ..\Bin
pushd ..\Bin

REM MT     Static CRT
REM EHa-   Disable exception handling
REM GR-    Disable C RTTI
REM O2     Optimisation Level 2
REM Oi     Use CPU Intrinsics
REM Z7     Combine multi-debug files to one debug file
REM wd4201 Nonstandard extension used: nameless struct/union
REM Tp     Treat header file as CPP source file
cl /MT /EHa /GR- /O2 /Oi /Z7 /W4 /WX /D DQN_INSPECT_EXECUTABLE_IMPLEMENTATION /Tp ../Code/DqnInspect.h /link /nologo

popd
