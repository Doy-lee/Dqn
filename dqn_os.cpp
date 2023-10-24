DQN_API void Dqn_OS_Exit(uint32_t exit_code)
{
    #if defined(DQN_OS_WIN32)
    ExitProcess(exit_code);
    #else
    exit(exit_code);
    #endif
}

// NOTE: [$EXEC] Dqn_OSExec ========================================================================
DQN_API Dqn_OSExecResult Dqn_OS_ExecWait(Dqn_OSExecAsyncHandle handle)
{
    Dqn_OSExecResult result = {};
    if (!handle.process || handle.os_error_code) {
        result.os_error_code = handle.os_error_code;
        return result;
    }

    #if defined(DQN_OS_WIN32)
    DWORD exec_result = WaitForSingleObject(handle.process, INFINITE);
    if (exec_result == WAIT_FAILED) {
        result.os_error_code = GetLastError();
        return result;
    }

    DWORD exit_status;
    if (!GetExitCodeProcess(handle.process, &exit_status)) {
        result.os_error_code = GetLastError();
        return result;
    }

    result.exit_code = exit_status;
    CloseHandle(handle.process);
    #elif defined(DQN_PLATFORM_EMSCRIPTEN)
    DQN_ASSERTF(false, "Unsupported operation");
    #else
    for (;;) {
        int status = 0;
        if (waitpid(DQN_CAST(pid_t)handle.process, &status, 0) < 0) {
            result.os_error_code = errno;
            break;
        }

        if (WIFEXITED(status)) {
            result.exit_code = WEXITSTATUS(status);
            break;
        }

        if (WIFSIGNALLED(status)) {
            result.os_error_code = WTERMSIG(status);
            break;
        }
    }
    #endif
    return result;
}

DQN_API Dqn_OSExecAsyncHandle Dqn_OS_ExecAsync(Dqn_Str8 cmd, Dqn_Str8 working_dir)
{
    Dqn_OSExecAsyncHandle result = {};
    if (cmd.size == 0)
        return result;

    #if defined(DQN_OS_WIN32)
    Dqn_ThreadScratch scratch       = Dqn_Thread_GetScratch(nullptr);
    Dqn_Str16         cmd16         = Dqn_Win_Str8ToStr16(scratch.arena, cmd);
    Dqn_Str16         working_dir16 = Dqn_Win_Str8ToStr16(scratch.arena, working_dir);

    PROCESS_INFORMATION proc_info = {};
    STARTUPINFOW startup_info     = {};
    startup_info.cb               = sizeof(STARTUPINFOW);
    startup_info.hStdError        = GetStdHandle(STD_ERROR_HANDLE);
    startup_info.hStdOutput       = GetStdHandle(STD_OUTPUT_HANDLE);
    startup_info.hStdInput        = GetStdHandle(STD_INPUT_HANDLE);
    startup_info.dwFlags         |= STARTF_USESTDHANDLES;
    BOOL create_result            = CreateProcessW(nullptr, cmd16.data, nullptr, nullptr, true, 0, nullptr, working_dir16.data, &startup_info, &proc_info);
    if (!create_result) {
        result.os_error_code = GetLastError();
        return result;
    }

    CloseHandle(proc_info.hThread);
    result.process = proc_info.hProcess;
    #else
    DQN_ASSERTF(false, "Unsupported operation");
    // TODO: This API will need to switch to an array of strings for unix
    #if 0
    pid_t child_pid = fork();
    if (child_pid < 0) {
        result.os_error_code = errno;
        return result;
    }

    if (child_pid == 0) {
        if (working_dir.size) {
            if (chdir(working_dir.data) == -1) {
                result.os_error_code = errno;
                return result;
            }
        }

        if (execvp(cmd.items[0], (char * const*) cmd_null.items) < 0) {
            result.os_error_code = errno;
            return result;
        }
        DQN_INVALID_CODE_PATH;
    }

    result.process = DQN_CAST(void *)child_pid;
    #endif
    #endif
    return result;
}

DQN_API Dqn_OSExecResult Dqn_OS_Exec(Dqn_Str8 cmd, Dqn_Str8 working_dir)
{
    Dqn_OSExecAsyncHandle async_handle = Dqn_OS_ExecAsync(cmd, working_dir);
    Dqn_OSExecResult result            = Dqn_OS_ExecWait(async_handle);
    return result;
}

DQN_API void Dqn_OS_ExecOrAbort(Dqn_Str8 cmd, Dqn_Str8 working_dir)
{
    Dqn_OSExecResult result = Dqn_OS_Exec(cmd, working_dir);
    if (result.os_error_code || result.exit_code) {
        if (result.os_error_code) {
            Dqn_Log_ErrorF("OS failed to execute the requested command returning the error code %u. The command was\n\n%.*s", result.os_error_code, DQN_STR_FMT(cmd));
            Dqn_OS_Exit(result.os_error_code);
        }

        if (result.exit_code) {
            Dqn_Log_ErrorF("OS executed command and returned a non-zero status: %u. The command was\n\n%.*s", result.exit_code, DQN_STR_FMT(cmd));
            Dqn_OS_Exit(result.exit_code);
        }
    }
}
