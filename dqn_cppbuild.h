#if !defined(DQN_CPP_BUILD_H)
#define DQN_CPP_BUILD_H

////////////////////////////////////////////////////////////////////////////////////////////////////
//
//    $$$$$$\  $$$$$$$\  $$$$$$$\        $$$$$$$\  $$\   $$\ $$$$$$\ $$\       $$$$$$$\
//   $$  __$$\ $$  __$$\ $$  __$$\       $$  __$$\ $$ |  $$ |\_$$  _|$$ |      $$  __$$\
//   $$ /  \__|$$ |  $$ |$$ |  $$ |      $$ |  $$ |$$ |  $$ |  $$ |  $$ |      $$ |  $$ |
//   $$ |      $$$$$$$  |$$$$$$$  |      $$$$$$$\ |$$ |  $$ |  $$ |  $$ |      $$ |  $$ |
//   $$ |      $$  ____/ $$  ____/       $$  __$$\ $$ |  $$ |  $$ |  $$ |      $$ |  $$ |
//   $$ |  $$\ $$ |      $$ |            $$ |  $$ |$$ |  $$ |  $$ |  $$ |      $$ |  $$ |
//   \$$$$$$  |$$ |      $$ |            $$$$$$$  |\$$$$$$  |$$$$$$\ $$$$$$$$\ $$$$$$$  |
//    \______/ \__|      \__|            \_______/  \______/ \______|\________|\_______/
//
//   dqn_cppbuild.h -- Helper functions to make build scripts in C++
//
////////////////////////////////////////////////////////////////////////////////////////////////////

#if !defined(DQN_H)
#error 'dqn.h' must be included before 'dqn_cppbuild.h'
#endif

struct Dqn_CPPBuildCompileFile
{
    Dqn_Slice<Dqn_Str8> prefix_flags;
    Dqn_Slice<Dqn_Str8> suffix_flags;
    Dqn_Str8            input_file_path;
    Dqn_Str8            output_file_path;
};

Dqn_Str8 const DQN_CPP_BUILD_OBJ_SUFFIX_OBJ = DQN_STR8(".obj");
Dqn_Str8 const DQN_CPP_BUILD_OBJ_SUFFIX_O   = DQN_STR8(".o");

enum Dqn_CPPBuildFlagsStyle
{
    Dqn_CPPBuildFlagsStyle_MSVC,
    Dqn_CPPBuildFlagsStyle_GCC,
    Dqn_CPPBuildFlagsStyle_CLANG,
};

enum Dqn_CPPBuildAppendCompilerToCommand
{
    Dqn_CPPBuildAppendCompilerToCommand_No,
    Dqn_CPPBuildAppendCompilerToCommand_Yes,
};

struct Dqn_CPPBuildContext
{
    // Dictates the type of compiler flags the functions may append to the
    // build command line
    Dqn_CPPBuildFlagsStyle             flags_style;

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

DQN_API Dqn_Slice<Dqn_Str8>     Dqn_CPPBuild_ToCommandLine    (Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Arena *arena);
DQN_API Dqn_Str8                Dqn_CPPBuild_ToCommandLineStr8(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Arena *arena);
DQN_API Dqn_CPPBuildAsyncResult Dqn_CPPBuild_Async            (Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode);
DQN_API void                    Dqn_CPPBuild_ExecOrAbort      (Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode);
#endif // DQN_CPP_BUILD_H

#if defined(DQN_CPP_BUILD_IMPLEMENTATION)
DQN_API Dqn_Slice<Dqn_Str8> Dqn_CPPBuild_ToCommandLine(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Arena *arena)
{
    // NOTE: Check if object files are newer than the source files /////////////////////////////////
    Dqn_Scratch scratch  = Dqn_Scratch_Get(arena);
    Dqn_Slice<Dqn_Str8> result = {};

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
                obj_file_name      = Dqn_Str8_InitF(scratch.arena, "%.*s%.*s", DQN_STR_FMT(file_stem), DQN_STR_FMT(compile_file_obj_suffix));
            }

            Dqn_Str8 obj_file_path = obj_file_name;
            if (build_context.build_dir.size)
                obj_file_path = Dqn_OS_PathConvertF(scratch.arena, "%.*s/%.*s", DQN_STR_FMT(build_context.build_dir), DQN_STR_FMT(obj_file_name));

            Dqn_OSPathInfo file_info     = Dqn_OS_PathInfo(file.input_file_path);
            Dqn_OSPathInfo obj_file_info = Dqn_OS_PathInfo(obj_file_path);
            if (obj_file_info.last_write_time_in_s >= file_info.last_write_time_in_s)
                continue;

            dirtied_compile_files.data[dirtied_compile_files.size++] = file;
        }

        if (dirtied_compile_files.size <= 0)
            return result;
    }

    // NOTE: Build the command line invocation /////////////////////////////////////////////////////
    Dqn_Str8Builder builder = {};
    builder.arena       = scratch.arena;
    Dqn_Str8Builder_AppendRefArray(&builder, build_context.compile_flags);

    DQN_FOR_UINDEX(index, build_context.include_dirs.size) {
        Dqn_Str8 include_dir = build_context.include_dirs.data[index];
        Dqn_Str8Builder_AppendRef(&builder, DQN_STR8("-I"));
        Dqn_Str8Builder_AppendRef(&builder, include_dir);
    }

    DQN_FOR_UINDEX(index, dirtied_compile_files.size) {
        Dqn_CPPBuildCompileFile file = dirtied_compile_files.data[index];
        if (Dqn_Str8_HasData(file.output_file_path)) {
            switch (build_context.flags_style) {
                case Dqn_CPPBuildFlagsStyle_MSVC: {
                    Dqn_Str8Builder_AppendF(&builder, "-Fo%.*s", DQN_STR_FMT(file.output_file_path));
                } break;

                case Dqn_CPPBuildFlagsStyle_GCC: /*FALLTHRU*/
                case Dqn_CPPBuildFlagsStyle_CLANG: {
                    Dqn_Str8Builder_AppendF  (&builder, "-o");
                    Dqn_Str8Builder_AppendRef(&builder, file.output_file_path);
                } break;
            }
        }

        // TODO(doyle): Check if the file exists, error if it doesn't

        Dqn_Str8Builder_AppendRefArray(&builder, file.prefix_flags);
        Dqn_Str8Builder_AppendRef(&builder, file.input_file_path);
        Dqn_Str8Builder_AppendRefArray(&builder, file.suffix_flags);
    }

    Dqn_Str8Builder_AppendRefArray(&builder, build_context.link_flags);
    result = Dqn_Str8Builder_BuildSlice(&builder, arena);
    return result;
}

DQN_API Dqn_Str8 Dqn_CPPBuild_ToCommandLineStr8(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_Arena *arena)
{
    Dqn_Slice<Dqn_Str8> cmd_line = Dqn_CPPBuild_ToCommandLine(build_context, mode, arena);
    Dqn_Str8 result              = Dqn_Slice_Str8Render(arena, cmd_line, DQN_STR8(" ") /*separator*/);
    return result;
}

DQN_API Dqn_CPPBuildAsyncResult Dqn_CPPBuild_Async(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode, Dqn_ErrorSink *error)
{
    Dqn_Scratch             scratch  = Dqn_Scratch_Get(nullptr);
    Dqn_Slice<Dqn_Str8>     cmd_line = Dqn_CPPBuild_ToCommandLine(build_context, mode, scratch.arena);
    Dqn_CPPBuildAsyncResult result   = {};
    if (!cmd_line.size)
        return result;

    if (!Dqn_OS_MakeDir(build_context.build_dir)) {
        result.status = Dqn_CPPBuildStatus_BuildDirectoryFailedToBeMade;
        Dqn_ErrorSink_MakeF(error, result.status, "Failed to make build directory '%.*s'", DQN_STR_FMT(build_context.build_dir));
        return result;
    }

    result.async_handle = Dqn_OS_ExecAsync(cmd_line, build_context.build_dir, Dqn_OSExecFlag_Nil, error);
    return result;
}

void Dqn_CPPBuild_ExecOrAbort(Dqn_CPPBuildContext build_context, Dqn_CPPBuildMode mode)
{
    if (!Dqn_OS_MakeDir(build_context.build_dir)) {
        Dqn_Log_ErrorF("Failed to make build dir '%.*s'", DQN_STR_FMT(build_context.build_dir));
        exit(-1);
    }
    Dqn_Scratch         scratch  = Dqn_Scratch_Get(nullptr);
    Dqn_Slice<Dqn_Str8> cmd_line = Dqn_CPPBuild_ToCommandLine(build_context, mode, scratch.arena);
    Dqn_OS_ExecOrAbort(cmd_line, build_context.build_dir, Dqn_OSExecFlag_Nil, scratch.arena);
}
#endif // DQN_CPP_BUILD_IMPLEMENTATION
