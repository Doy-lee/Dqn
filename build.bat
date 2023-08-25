@echo OFF
setlocal

set code_dir=%~dp0
if not exist Build mkdir Build
pushd Build

    REM Flags ==================================================================
    REM MT     Static CRT
    REM EHa-   Disable exception handling
    REM GR-    Disable C RTTI
    REM O2     Optimisation Level 2
    REM Oi     Use CPU Intrinsics
    REM Z7     Combine multi-debug files to one debug file
    REM wd4201 Nonstandard extension used: nameless struct/union
    REM Tp     Treat header file as CPP source file
    set compile_flags=-MT -EHa -GR- -Od -Oi -Z7 -wd4201 -W4 -WX -D DQN_TEST_WITH_MAIN -nologo -analyze
    set linker_flags=-link -nologo
    set msvc_flags=-fsanitize=address
    set clang_flags=-fsanitize=address,undefined

    REM Compiler: MSVC cl ======================================================
    where /q cl || (
        echo [ERROR] cl is not found, please put MSVC's cl on the path
        exit /b 1
    )
    cl %compile_flags% %msvc_flags% %code_dir%\Misc\dqn_unit_tests.cpp -I %code_dir% /Fe:dqn_unit_tests_msvc %link_flags% || exit /b 1

    REM Compiler: clang-cl =====================================================
    where /q clang-cl || (
        echo [WARN] Optional clang compile via clang-cl if it's in the path, please put clang-cl on the path for this feature
        exit /b 0
    )
    clang-cl -D DQN_TEST_WITH_MAIN %code_dir%\Misc\dqn_unit_tests.cpp -I %code_dir% /Fe:dqn_unit_tests_clang -link
popd
