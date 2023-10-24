#if !defined(DQN_CPP_BUILD_H)
#define DQN_CPP_BUILD_H

struct Dqn_CPPBuildCompileFile
{
    Dqn_Slice<Dqn_Str8> flags;
    Dqn_Str8            input_file_path;
    Dqn_Str8            output_file_path;
};

Dqn_Str8 const DQN_CPP_BUILD_OBJ_SUFFIX_OBJ = DQN_STR8(".obj");
Dqn_Str8 const DQN_CPP_BUILD_OBJ_SUFFIX_O   = DQN_STR8(".o");

enum Dqn_CPPBuildCompiler
{
    Dqn_CPPBuildCompiler_MSVC,
    Dqn_CPPBuildCompiler_GCC,
};

struct Dqn_CPPBuildContext
{
    Dqn_CPPBuildCompiler               compiler;
    Dqn_Str8                           compile_file_obj_suffix;
    Dqn_Slice<Dqn_CPPBuildCompileFile> compile_files;
    Dqn_Slice<Dqn_Str8>                compile_flags;
    Dqn_Slice<Dqn_Str8>                include_dirs;
    Dqn_Slice<Dqn_Str8>                link_flags;
    Dqn_Str8                           build_dir;
};

enum Dqn_CPPBuildStatus
{
    Dqn_CPPBuildStatus_Ok,
    Dqn_CPPBuildStatus_BuildDirectoryFailedToBeMade,
};

struct Dqn_CPPBuildAsyncResult
{
    Dqn_CPPBuildStatus    status;
    Dqn_OSExecAsyncHandle async_handle;
};

enum Dqn_CPPBuildMode
{
    Dqn_CPPBuildMode_AlwaysRebuild,
    Dqn_CPPBuildMode_CacheBuild,
};

DQN_API Dqn_Str8                Dqn_CPPBuild_ToCommandLine(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Allocator allocator);
DQN_API Dqn_CPPBuildAsyncResult Dqn_CPPBuild_Async        (Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode);
DQN_API void                    Dqn_CPPBuild_ExecOrAbort  (Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode);
#endif // DQN_CPP_BUILD_H

#if defined(DQN_CPP_BUILD_IMPLEMENTATION)
DQN_API Dqn_Str8 Dqn_CPPBuild_ToCommandLine(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Allocator allocator)
{
    // NOTE: Check if object files are newer than the source files =================================
    Dqn_ThreadScratch scratch                                = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_Str8 result                                       = {};

    Dqn_Slice<Dqn_CPPBuildCompileFile> dirtied_compile_files = build_context.compile_files;
    if (mode == Dqn_CPPBuildMode_CacheBuild) {
        dirtied_compile_files      = Dqn_Slice_Alloc<Dqn_CPPBuildCompileFile>(scratch.arena, build_context.compile_files.size, Dqn_ZeroMem_Yes);
        dirtied_compile_files.size = 0;

        DQN_FOR_UINDEX (index, build_context.compile_files.size) {
            Dqn_CPPBuildCompileFile file = build_context.compile_files.data[index];

            Dqn_Str8 obj_file_name = {};
            if (file.output_file_path.size) {
                obj_file_name = file.output_file_path;
            } else {
                // NOTE: Determine the object file suffix
                Dqn_Str8 compile_file_obj_suffix = build_context.compile_file_obj_suffix;
                if (compile_file_obj_suffix.size == 0)
                    compile_file_obj_suffix = DQN_CPP_BUILD_OBJ_SUFFIX_OBJ;

                // NOTE: Create the object file path
                Dqn_Str8 file_stem = Dqn_Str8_FileNameNoExtension(file.input_file_path);
                obj_file_name      = Dqn_Str8_InitF(scratch.allocator, "%.*s%.*s", DQN_STR_FMT(file_stem), DQN_STR_FMT(compile_file_obj_suffix));
            }

            Dqn_Str8 obj_file_path = obj_file_name;
            if (build_context.build_dir.size)
                obj_file_path = Dqn_FsPath_ConvertF(scratch.arena, "%.*s/%.*s", DQN_STR_FMT(build_context.build_dir), DQN_STR_FMT(obj_file_name));

            Dqn_FsInfo file_info     = Dqn_Fs_GetInfo(file.input_file_path);
            Dqn_FsInfo obj_file_info = Dqn_Fs_GetInfo(obj_file_path);
            if (obj_file_info.last_write_time_in_s >= file_info.last_write_time_in_s)
                continue;

            dirtied_compile_files.data[dirtied_compile_files.size++] = file;
        }

        if (dirtied_compile_files.size <= 0)
            return result;
    }

    // NOTE: Build the command line invocation =====================================================
    Dqn_Str8Builder builder = {};
    builder.allocator          = allocator;
    DQN_FOR_UINDEX (index, build_context.compile_flags.size) {
        Dqn_Str8 flag = build_context.compile_flags.data[index];
        if (index)
            Dqn_Str8Builder_AppendF(&builder, " ");
        Dqn_Str8Builder_AppendRef(&builder, flag);
    }

    DQN_FOR_UINDEX (index, build_context.include_dirs.size) {
        Dqn_Str8 include_dir = build_context.include_dirs.data[index];
        if (builder.count)
            Dqn_Str8Builder_AppendF(&builder, " ");
        Dqn_Str8Builder_AppendF(&builder, "/I %.*s", DQN_STR_FMT(include_dir));
    }

    DQN_FOR_UINDEX (index, dirtied_compile_files.size) {
        Dqn_CPPBuildCompileFile file = dirtied_compile_files.data[index];
        Dqn_Str8 obj_file            = {};
        if (builder.count)
            Dqn_Str8Builder_AppendF(&builder, " ");

        if (file.output_file_path.size) {
            switch (build_context.compiler) {
                case Dqn_CPPBuildCompiler_MSVC: {
                    Dqn_Str8Builder_AppendF(&builder, "/Fo%.*s ", DQN_STR_FMT(file.output_file_path));
                } break;

                case Dqn_CPPBuildCompiler_GCC: {
                    Dqn_Str8Builder_AppendF(&builder, "-o %.*s ", DQN_STR_FMT(file.output_file_path));
                } break;
            }
        }

        DQN_FOR_UINDEX (flag_index, file.flags.size) {
            Dqn_Str8 flag = file.flags.data[flag_index];
            Dqn_Str8Builder_AppendF(&builder, "%s%.*s", flag_index ? " " : "", DQN_STR_FMT(flag));
        }

        if (file.flags.size)
            Dqn_Str8Builder_AppendF(&builder, " ");
        Dqn_Str8Builder_AppendRef(&builder, file.input_file_path);
    }

    DQN_FOR_UINDEX (index, build_context.link_flags.size) {
        Dqn_Str8 file = build_context.link_flags.data[index];
        if (builder.count)
            Dqn_Str8Builder_AppendF(&builder, " ");
        Dqn_Str8Builder_AppendRef(&builder, file);
    }

    result = Dqn_Str8Builder_Build(&builder, allocator);
    return result;
}


DQN_API Dqn_CPPBuildAsyncResult Dqn_CPPBuild_Async(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode)
{
    Dqn_ThreadScratch       scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_Str8                cmd     = Dqn_CPPBuild_ToCommandLine(build_context, mode, scratch.allocator);
    Dqn_CPPBuildAsyncResult result  = {};
    if (!cmd.size)
        return result;

    if (!Dqn_Fs_MakeDir(build_context.build_dir)) {
        result.status = Dqn_CPPBuildStatus_BuildDirectoryFailedToBeMade;
        return result;
    }

    result.async_handle = Dqn_OS_ExecAsync(cmd, build_context.build_dir);
    return result;
}

void Dqn_CPPBuild_ExecOrAbort(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode)
{
    if (!Dqn_Fs_MakeDir(build_context.build_dir)) {
        Dqn_Log_ErrorF("Failed to make build dir '%.*s'", DQN_STR_FMT(build_context.build_dir));
        exit(-1);
    }
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(nullptr);
    Dqn_Str8 cmd              = Dqn_CPPBuild_ToCommandLine(build_context, mode, scratch.allocator);
    Dqn_OS_ExecOrAbort(cmd, build_context.build_dir);
}
#endif // DQN_CPP_BUILD_IMPLEMENTATION
