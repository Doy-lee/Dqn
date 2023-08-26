// NOTE: [$PRIN] Dqn_Print =========================================================================
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
DQN_API void           Dqn_Print_StdF              (Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API void           Dqn_Print_StdFV             (Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdStyle          (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string);
DQN_API void           Dqn_Print_StdFStyle         (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API void           Dqn_Print_StdFVStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLn             (Dqn_PrintStd std_handle, Dqn_String8 string);
DQN_API void           Dqn_Print_StdLnF            (Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFV           (Dqn_PrintStd std_handle, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args);

DQN_API void           Dqn_Print_StdLnStyle        (Dqn_PrintStd std_handle, Dqn_PrintStyle style, Dqn_String8 string);
DQN_API void           Dqn_Print_StdLnFStyle       (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, ...);
DQN_API void           Dqn_Print_StdLnFVStyle      (Dqn_PrintStd std_handle, Dqn_PrintStyle style, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args);

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

