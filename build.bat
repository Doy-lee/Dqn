@echo OFF

set code_dir=%~dp0
if not exist Build mkdir Build
pushd Build

    REM ------------------------------------------------------------------------
    REM Flags
    REM ------------------------------------------------------------------------
    REM MT     Static CRT
    REM EHa-   Disable exception handling
    REM GR-    Disable C RTTI
    REM O2     Optimisation Level 2
    REM Oi     Use CPU Intrinsics
    REM Z7     Combine multi-debug files to one debug file
    REM wd4201 Nonstandard extension used: nameless struct/union
    REM Tp     Treat header file as CPP source file
    set flags=-MT -EHa -GR- -Od -Oi -Z7 -wd4201 -D DQN_TEST_WITH_MAIN %code_dir%Dqn_Tests.cpp -link -nologo
    set msvc_flags=-fsanitize=address -D STBSP__ASAN=__declspec(no_sanitize_address)
    set clang_flags=-fsanitize=address -fsanitize=undefined

    REM ------------------------------------------------------------------------
    REM Compiler: MSVC cl
    REM ------------------------------------------------------------------------
    where /q cl || (
        echo [ERROR] cl is not found, please put MSVC's cl on the path
        exit /b 1
    )

    if not exist msvc mkdir msvc
    pushd msvc
    cl %msvc_flags% %flags%
    popd

    REM ------------------------------------------------------------------------
    REM Compiler: clang-cl
    REM ------------------------------------------------------------------------
    where /q clang-cl || (
        echo [WARN] Optional clang compile via clang-cl if it's in the path, please put clang-cl on the path for this feature
        exit /b 0
    )

    if not exist clang mkdir clang
    pushd clang
    clang-cl %clang_flags% %flags%
    popd
popd
