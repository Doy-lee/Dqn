// =================================================================================================
// [$PRIN] Dqn_Print            |                             | Console printing
// =================================================================================================
enum Dqn_PrintStd
{
    Dqn_PrintStd_Out,
    Dqn_PrintStd_Err,
};

enum Dqn_PrintBold
{
    Dqn_PrintBold_No,
    Dqn_PrintBold_Yes,
};

struct Dqn_PrintStyle
{
    Dqn_PrintBold bold;
    bool          colour;
    uint8_t       r, g, b;
};

enum Dqn_PrintESCColour
{
    Dqn_PrintESCColour_Fg,
    Dqn_PrintESCColour_Bg,
};

// NOTE: Print Style ===============================================================================
DQN_API Dqn_PrintStyle Dqn_Print_StyleColour       (uint8_t r, uint8_t g, uint8_t b, Dqn_PrintBold bold);
DQN_API Dqn_PrintStyle Dqn_Print_StyleColourU32    (uint32_t rgb, Dqn_PrintBold bold);
DQN_API Dqn_PrintStyle Dqn_Print_StyleBold         ();

// NOTE: Print Standard Out ========================================================================
#define                Dqn_Print(string)                        Dqn_Print_Std(Dqn_PrintStd_Out, string)
#define                Dqn_Print_F(fmt, ...)                    Dqn_Print_StdF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)
#define                Dqn_Print_FV(fmt, args)                  Dqn_Print_StdFV(Dqn_PrintStd_Out, fmt, args)

#define                Dqn_Print_Style(style, string)           Dqn_Print_StdStyle(Dqn_PrintStd_Out, style, string)
#define                Dqn_Print_FStyle(style, fmt, ...)        Dqn_Print_StdFStyle(Dqn_PrintStd_Out, style, fmt, ## __VA_ARGS__)
#define                Dqn_Print_FVStyle(style, fmt, args, ...) Dqn_Print_StdFVStyle(Dqn_PrintStd_Out, style, fmt, args)

#define                Dqn_Print_Ln(string)                     Dqn_Print_StdLn(Dqn_PrintStd_Out, string)
#define                Dqn_Print_LnF(fmt, ...)                  Dqn_Print_StdLnF(Dqn_PrintStd_Out, fmt, ## __VA_ARGS__)
#define                Dqn_Print_LnFV(fmt, args)                Dqn_Print_StdLnFV(Dqn_PrintStd_Out, fmt, args)

#define                Dqn_Print_LnStyle(style, string)         Dqn_Print_StdLnStyle(Dqn_PrintStd_Out, style, string);
#define                Dqn_Print_LnFStyle(style, fmt, ...)      Dqn_Print_StdLnFStyle(Dqn_PrintStd_Out, style, fmt, ## __VA_ARGS__);
#define                Dqn_Print_LnFVStyle(style, fmt, args)    Dqn_Print_StdLnFVStyle(Dqn_PrintStd_Out, style, fmt, args);

// NOTE: Print =====================================================================================
DQN_API void           Dqn_Print_Std               (Dqn_PrintStd std_handle, Dqn_String8 string);
DQN_API void           Dqn_Print_StdF              (Dqn_PrintStd std_handle, char const *fmt, ...);
DQN_API void           Dqn_Print_StdFV             (Dqn_PrintStd std_handle, char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdStyle          (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string);
DQN_API void           Dqn_Print_StdFStyle         (Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, ...);
DQN_API void           Dqn_Print_StdFVStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLn             (Dqn_PrintStd std_handle, Dqn_String8 string);
DQN_API void           Dqn_Print_StdLnF            (Dqn_PrintStd std_handle, char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFV           (Dqn_PrintStd std_handle, char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLnStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string);
DQN_API void           Dqn_Print_StdLnFStyle       (Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFVStyle      (Dqn_PrintStd std_handle, Dqn_PrintStyle style, char const *fmt, va_list args);

// NOTE: ANSI Formatting Codes =====================================================================
Dqn_String8            Dqn_Print_ESCColourString   (Dqn_PrintESCColour colour, uint8_t r, uint8_t g, uint8_t b);
Dqn_String8            Dqn_Print_ESCColourU32String(Dqn_PrintESCColour colour, uint32_t value);

#define                Dqn_Print_ESCColourFgString(r, g, b)  Dqn_Print_ESCColourString(Dqn_PrintESCColour_Fg, r, g, b)
#define                Dqn_Print_ESCColourBgString(r, g, b)  Dqn_Print_ESCColourString(Dqn_PrintESCColour_Bg, r, g, b)
#define                Dqn_Print_ESCColourFg(r, g, b)        Dqn_Print_ESCColourString(Dqn_PrintESCColour_Fg, r, g, b).data
#define                Dqn_Print_ESCColourBg(r, g, b)        Dqn_Print_ESCColourString(Dqn_PrintESCColour_Bg, r, g, b).data

#define                Dqn_Print_ESCColourFgU32String(value) Dqn_Print_ESCColourU32String(Dqn_PrintESCColour_Fg, value)
#define                Dqn_Print_ESCColourBgU32String(value) Dqn_Print_ESCColourU32String(Dqn_PrintESCColour_Bg, value)
#define                Dqn_Print_ESCColourFgU32(value)       Dqn_Print_ESCColourU32String(Dqn_PrintESCColour_Fg, value).data
#define                Dqn_Print_ESCColourBgU32(value)       Dqn_Print_ESCColourU32String(Dqn_PrintESCColour_Bg, value).data

#define                Dqn_Print_ESCReset                    "\x1b[0m"
#define                Dqn_Print_ESCBold                     "\x1b[1m"
#define                Dqn_Print_ESCResetString              DQN_STRING8(Dqn_Print_ESCReset)
#define                Dqn_Print_ESCBoldString               DQN_STRING8(Dqn_Print_ESCBold)

// =================================================================================================
// [$LLOG] Dqn_Log              |                             | Library logging
// =================================================================================================
enum Dqn_LogType
{
    Dqn_LogType_Debug,
    Dqn_LogType_Info,
    Dqn_LogType_Warning,
    Dqn_LogType_Error,
    Dqn_LogType_Count,
};

/// RGBA
#define Dqn_LogTypeColourU32_Info    0x00'87'ff'ff // Blue
#define Dqn_LogTypeColourU32_Warning 0xff'ff'00'ff // Yellow
#define Dqn_LogTypeColourU32_Error   0xff'00'00'ff // Red

/// The logging procedure of the library. Users can override the default logging
/// function by setting the logging function pointer in Dqn_Library. This
/// function will be invoked every time a log is recorded using the following
/// functions.
///
/// @param[in] log_type This value is one of the Dqn_LogType values if the log
/// was generated from one of the default categories. -1 if the log is not from
/// one of the default categories.
typedef void Dqn_LogProc(Dqn_String8 type, int log_type, void *user_data, Dqn_CallSite call_site, char const *fmt, va_list va);

#define Dqn_Log_DebugF(fmt, ...)        Dqn_Log_TypeFCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_InfoF(fmt, ...)         Dqn_Log_TypeFCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_WarningF(fmt, ...)      Dqn_Log_TypeFCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, ## __VA_ARGS__)
#define Dqn_Log_ErrorF(fmt, ...)        Dqn_Log_TypeFCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

#define Dqn_Log_DebugFV(fmt, args)      Dqn_Log_TypeFVCallSite(Dqn_LogType_Debug, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_InfoFV(fmt, args)       Dqn_Log_TypeFVCallSite(Dqn_LogType_Info, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_WarningFV(fmt, args)    Dqn_Log_TypeFVCallSite(Dqn_LogType_Warning, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_ErrorFV(fmt, args)      Dqn_Log_TypeFVCallSite(Dqn_LogType_Error, DQN_CALL_SITE, fmt, args)

#define Dqn_Log_TypeFV(type, fmt, args) Dqn_Log_TypeFVCallSite(type, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_TypeF(type, fmt, ...)   Dqn_Log_TypeFCallSite(type, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

#define Dqn_Log_FV(type, fmt, args)     Dqn_Log_FVCallSite(type, DQN_CALL_SITE, fmt, args)
#define Dqn_Log_F(type, fmt, ...)       Dqn_Log_FCallSite(type, DQN_CALL_SITE, fmt, ## __VA_ARGS__)

DQN_API Dqn_String8 Dqn_Log_MakeString    (Dqn_Allocator allocator, bool colour, Dqn_String8 type, int log_type, Dqn_CallSite call_site, char const *fmt, va_list args);
DQN_API void        Dqn_Log_TypeFVCallSite(Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void        Dqn_Log_TypeFCallSite (Dqn_LogType type, Dqn_CallSite call_site, char const *fmt, ...);
DQN_API void        Dqn_Log_FVCallSite    (Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, va_list va);
DQN_API void        Dqn_Log_FCallSite     (Dqn_String8 type, Dqn_CallSite call_site, char const *fmt, ...);

