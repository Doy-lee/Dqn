// NOTE: [$EXEC] Dqn_OSExec ========================================================================
struct Dqn_OSExecAsyncHandle
{
    uint32_t  os_error_code;
    void     *process;
};

struct Dqn_OSExecResult
{
    uint32_t os_error_code;
    uint32_t exit_code;
};

DQN_API void                  Dqn_OS_Exit       (uint32_t exit_code);
DQN_API Dqn_OSExecResult      Dqn_OS_ExecWait   (Dqn_OSExecAsyncHandle handle);
DQN_API Dqn_OSExecAsyncHandle Dqn_OS_ExecAsync  (Dqn_Str8 cmd, Dqn_Str8 working_dir);
DQN_API Dqn_OSExecResult      Dqn_OS_Exec       (Dqn_Str8 cmd, Dqn_Str8 working_dir);
DQN_API void                  Dqn_OS_ExecOrAbort(Dqn_Str8 cmd, Dqn_Str8 working_dir);
