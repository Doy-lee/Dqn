@echo OFF
setlocal

set code_dir=%~dp0
if not exist Build mkdir Build
pushd Build
    REM Flags ======================================================================================
    REM MT     Static CRT
    REM EHa-   Disable exception handling
    REM GR-    Disable C RTTI
    REM O2     Optimisation Level 2
    REM Oi     Use CPU Intrinsics
    REM Z7     Combine multi-debug files to one debug file
    REM wd4201 Nonstandard extension used: nameless struct/union
    REM Tp     Treat header file as CPP source file
    set compile_flags=-MT -EHa -GR- -Od -Oi -Z7 -wd4201 -W4 -WX -D DQN_TEST_WITH_MAIN -nologo
    set link_flags=-link
    set msvc_flags=-fsanitize=address -analyze
    set clang_flags=-fsanitize=address,undefined

    REM Compiler: MSVC cl ==========================================================================
    where /q cl || (
        echo [ERROR] cl is not found, please put MSVC's cl on the path
        exit /b 1
    )
    cl %compile_flags% %msvc_flags% %code_dir%\Misc\dqn_unit_tests.cpp -I %code_dir% /Fe:dqn_unit_tests_msvc %link_flags% || exit /b 1

    REM Compiler: clang-cl =========================================================================
    where /q clang-cl || (
        echo [WARN] Optional clang compile via clang-cl if it's in the path, please put clang-cl on the path for this feature
        exit /b 0
    )
    clang-cl %compile_flags% %clang_flags% %code_dir%\Misc\dqn_unit_tests.cpp -I %code_dir% /Fe:dqn_unit_tests_clang %link_flags% || exit /b 1

    REM Compiler: zig ==============================================================================
    REM zig c++ -g -D DQN_TEST_WITH_MAIN -I %code_dir% %code_dir%\Misc\dqn_unit_tests.cpp -o dqn_unit_tests_zig || exit /b 1
popd
