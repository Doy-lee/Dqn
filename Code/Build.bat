@echo OFF

set code_dir=%~dp0
if not exist ..\Build mkdir ..\Build
pushd ..\Build

    REM MT     Static CRT
    REM EHa-   Disable exception handling
    REM GR-    Disable C RTTI
    REM O2     Optimisation Level 2
    REM Oi     Use CPU Intrinsics
    REM Z7     Combine multi-debug files to one debug file
    REM wd4201 Nonstandard extension used: nameless struct/union
    REM Tp     Treat header file as CPP source file

    if not exist msvc mkdir msvc
    if not exist clang mkdir clang

    set flags=/MT /EHa /GR- /Od /Oi /Z7 /wd4201 /D DQN_TEST_WITH_MAIN %code_dir%Dqn_Tests.cpp /link /nologo

    pushd msvc
    cl %flags%
    popd

    pushd clang
    clang-cl %flags%
    popd
popd
