#if defined(__linux__)
    #define HANDMADE_MATH_NO_SSE
#endif

#if defined(__GNUC__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wfree-nonheap-object"
#endif

#if defined(_WIN32)
#define WIN32_MEAN_AND_LEAN
#include <Winsock2.h>
#include <ws2tcpip.h>
#include <Windows.h>
#endif

#define DQN_PLATFORM_HEADER
#define DQN_PLATFORM_IMPLEMENTATION
#define DQN_IMPLEMENTATION
#include "dqn.h"

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_CPP_MODE
#include "tests/HandmadeMath.h"

#include <limits.h>
#include <stdio.h>

// TODO(doyle): Replace DQN_ASSERT with a non-halting assert that can connect to
// some sort of testing framework to track successes and failures.

#define LOG_HEADER() LogHeader(__func__)
FILE_SCOPE i32  global_indent;
FILE_SCOPE bool global_new_line;

#define RED   "\x1B[31m"
#define GRN   "\x1B[32m"
#define YEL   "\x1B[33m"
#define BLU   "\x1B[34m"
#define MAG   "\x1B[35m"
#define CYN   "\x1B[36m"
#define WHT   "\x1B[37m"
#define RESET "\x1B[0m"

enum class Status
{
    None,
    Ok,
    Error
};
void Log(Status status, char const *fmt, va_list va)
{
    DQN_ASSERT(global_indent >= 0);
    LOCAL_PERSIST i32 line_len = 0;

    char buf[1024] = {};
    i32 buf_len     = 0;
    {
        buf_len = Dqn_vsprintf(buf, fmt, va);
        DQN_ASSERT(buf_len < (i32)DQN_ARRAY_COUNT(buf));
        line_len += buf_len;
    }

    char indent_str[] = "    ";
    i32 indent_len    = DQN_CHAR_COUNT(indent_str);
    {
        line_len += (indent_len * global_indent);
        for (auto i = 0; i < global_indent; i++)
            printf("%s", indent_str);

        printf("%s", &(buf[0]));
    }

    if (status == Status::Ok || status == Status::Error)
    {
        char ok_status[] = "OK";
        char err_status[] = "ERROR";
        
        char *statusStr;
        i32 status_str_len;
        if (status == Status::Ok)
        {
            statusStr = ok_status;
            status_str_len = DQN_CHAR_COUNT(ok_status);
        }
        else
        {
            statusStr = err_status;
            status_str_len = DQN_CHAR_COUNT(err_status);
        }
        line_len += status_str_len;

        i32 target_len = 90;
        i32 remaining = target_len - line_len;
        remaining     = DQN_MAX(remaining, 0);

        for (auto i = 0; i < remaining; i++)
            putchar('.');

        if (status == Status::Ok)
        {
            printf(GRN "%s" RESET, statusStr);
        }
        else
        {
            printf(RED "%s" RESET, statusStr);
        }
    }

    if (global_new_line)
    {
        line_len = 0;
        printf("\n");
    }
}

void Log(Status status, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Log(status, fmt, va);
    va_end(va);
}

void Log(char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Log(Status::None, fmt, va);
    va_end(va);
}

void LogHeader(char const *func_name)
{
    global_indent--;
    Log("\n[%s]", func_name);
    global_indent++;
}

#include "DqnFixedString.cpp"
#include "DqnOS.cpp"
#include "DqnJson.cpp"
#include "DqnVHashTable.cpp"
#include "DqnMemStack.cpp"

void HandmadeMathVerifyMat4(DqnMat4 dqn_mat, hmm_mat4 hmm_mat)
{
    f32 *hmm_matf = (f32 *)&hmm_mat;
    f32 *dqn_matf = (f32 *)&dqn_mat;

    const u32 EXPECTED_SIZE = 16;
    u32 totalSize           = DQN_ARRAY_COUNT(dqn_mat.e) * DQN_ARRAY_COUNT(dqn_mat.e[0]);
    DQN_ASSERT(totalSize == EXPECTED_SIZE);
    DQN_ASSERT(totalSize ==
               (DQN_ARRAY_COUNT(hmm_mat.Elements) * DQN_ARRAY_COUNT(hmm_mat.Elements[0])));

    for (u32 i = 0; i < EXPECTED_SIZE; i++)
    {
        const f32 EPSILON = 0.001f;
        f32 diff          = hmm_matf[i] - dqn_matf[i];
        diff              = DQN_ABS(diff);
        DQN_ASSERTM(diff < EPSILON, "hmm_matf[%d]: %f, dqn_matf[%d]: %f\n", i, hmm_matf[i], i,
                       dqn_matf[i]);
    }
}

void HandmadeMathTestInternal()
{
    LOG_HEADER();
    // Test Perspective/Projection matrix values
}

void Dqn_Test()
{
    LOG_HEADER();

    // const u64 LARGEST_NUM  = (u64)-1;
    const i64 SMALLEST_NUM = LLONG_MIN;
    // StrToI64
    if (1)
    {
        const char *const a = "123";
        DQN_ASSERT(Dqn_StrToI64(a, DqnStr_Len(a)) == 123);

        const char *const b = "-123";
        DQN_ASSERT(Dqn_StrToI64(b, DqnStr_Len(b)) == -123);
        DQN_ASSERT(Dqn_StrToI64(b, 1) == 0);

        const char *const c = "-0";
        DQN_ASSERT(Dqn_StrToI64(c, DqnStr_Len(c)) == 0);

        const char *const d = "+123";
        DQN_ASSERT(Dqn_StrToI64(d, DqnStr_Len(d)) == 123);

// TODO(doyle): Unsigned conversion
#if 0
            char *e = "18446744073709551615";
            DQN_ASSERT((u64)(Dqn_StrToI64(e, DqnStr_Len(e))) == LARGEST_NUM);
#endif

        const char *const f = "-9223372036854775808";
        DQN_ASSERT(Dqn_StrToI64(f, DqnStr_Len(f)) == SMALLEST_NUM);

        Log("Dqn_StrToI64()");
    }

    // i64 to str
    if (1)
    {
        char a[DQN_I64_MAX_STR_SIZE] = {};
        Dqn_I64ToStr(+100, a, DQN_ARRAY_COUNT(a));
        DQN_ASSERT(DqnStr_Cmp(a, "100") == 0);

        char b[DQN_I64_MAX_STR_SIZE] = {};
        Dqn_I64ToStr(-100, b, DQN_ARRAY_COUNT(b));
        DQN_ASSERT(DqnStr_Cmp(b, "-100") == 0);

        char c[DQN_I64_MAX_STR_SIZE] = {};
        Dqn_I64ToStr(0, c, DQN_ARRAY_COUNT(c));
        DQN_ASSERT(DqnStr_Cmp(c, "0") == 0);

#if 0
            char d[DQN_I64_MAX_STR_SIZE] = {};
            Dqn_I64ToStr(LARGEST_NUM, d, DQN_ARRAY_COUNT(d));
            DQN_ASSERT(DqnStr_Cmp(d, "18446744073709551615") == 0);
#endif

        if (sizeof(size_t) == sizeof(u64))
        {
            char e[DQN_I64_MAX_STR_SIZE] = {};
            Dqn_I64ToStr(SMALLEST_NUM, e, DQN_ARRAY_COUNT(e));
            DQN_ASSERTM(DqnStr_Cmp(e, "-9223372036854775808") == 0, "e: %s", e);
        }

        Log("Dqn_I64ToStr()");
    }

    // StrToF32
    if (1)
    {
        const f32 EPSILON = 0.001f;
        const char a[]    = "-0.66248";
        f32 vA            = Dqn_StrToF32(a, DQN_ARRAY_COUNT(a));
        DQN_ASSERT(DQN_ABS(vA) - DQN_ABS(-0.66248f) < EPSILON);

        const char b[] = "-0.632053";
        f32 vB         = Dqn_StrToF32(b, DQN_ARRAY_COUNT(b));
        DQN_ASSERT(DQN_ABS(vB) - DQN_ABS(-0.632053f) < EPSILON);

        const char c[] = "-0.244271";
        f32 vC         = Dqn_StrToF32(c, DQN_ARRAY_COUNT(c));
        DQN_ASSERT(DQN_ABS(vC) - DQN_ABS(-0.244271f) < EPSILON);

        const char d[] = "-0.511812";
        f32 vD         = Dqn_StrToF32(d, DQN_ARRAY_COUNT(d));
        DQN_ASSERT(DQN_ABS(vD) - DQN_ABS(-0.511812f) < EPSILON);

        const char e[] = "-0.845392";
        f32 vE         = Dqn_StrToF32(e, DQN_ARRAY_COUNT(e));
        DQN_ASSERT(DQN_ABS(vE) - DQN_ABS(-0.845392f) < EPSILON);

        const char f[] = "0.127809";
        f32 vF         = Dqn_StrToF32(f, DQN_ARRAY_COUNT(f));
        DQN_ASSERT(DQN_ABS(vF) - DQN_ABS(-0.127809f) < EPSILON);

        const char g[] = "0.532";
        f32 vG         = Dqn_StrToF32(g, DQN_ARRAY_COUNT(g));
        DQN_ASSERT(DQN_ABS(vG) - DQN_ABS(-0.532f) < EPSILON);

        const char h[] = "0.923";
        f32 vH         = Dqn_StrToF32(h, DQN_ARRAY_COUNT(h));
        DQN_ASSERT(DQN_ABS(vH) - DQN_ABS(-0.923f) < EPSILON);

        const char i[] = "0.000";
        f32 vI         = Dqn_StrToF32(i, DQN_ARRAY_COUNT(i));
        DQN_ASSERT(DQN_ABS(vI) - DQN_ABS(-0.000f) < EPSILON);

        const char j[] = "0.000283538";
        f32 vJ         = Dqn_StrToF32(j, DQN_ARRAY_COUNT(j));
        DQN_ASSERT(DQN_ABS(vJ) - DQN_ABS(-0.000283538f) < EPSILON);

        const char k[] = "-1.25";
        f32 vK         = Dqn_StrToF32(k, DQN_ARRAY_COUNT(k));
        DQN_ASSERT(DQN_ABS(vK) - DQN_ABS(-1.25f) < EPSILON);

        const char l[] = "0.286843";
        f32 vL         = Dqn_StrToF32(l, DQN_ARRAY_COUNT(l));
        DQN_ASSERT(DQN_ABS(vL) - DQN_ABS(-0.286843f) < EPSILON);

        const char m[] = "-0.406";
        f32 vM         = Dqn_StrToF32(m, DQN_ARRAY_COUNT(m));
        DQN_ASSERT(DQN_ABS(vM) - DQN_ABS(-0.406f) < EPSILON);

        const char n[] = "-0.892";
        f32 vN         = Dqn_StrToF32(n, DQN_ARRAY_COUNT(n));
        DQN_ASSERT(DQN_ABS(vN) - DQN_ABS(-0.892f) < EPSILON);

        const char o[] = "0.201";
        f32 vO         = Dqn_StrToF32(o, DQN_ARRAY_COUNT(o));
        DQN_ASSERT(DQN_ABS(vO) - DQN_ABS(-0.201f) < EPSILON);

        const char p[] = "1.25";
        f32 vP         = Dqn_StrToF32(p, DQN_ARRAY_COUNT(p));
        DQN_ASSERT(DQN_ABS(vP) - DQN_ABS(1.25f) < EPSILON);

        const char q[] = "9.64635e-05";
        f32 vQ         = Dqn_StrToF32(q, DQN_ARRAY_COUNT(q));
        DQN_ASSERT(DQN_ABS(vQ) - DQN_ABS(9.64635e-05) < EPSILON);

        const char r[] = "9.64635e+05";
        f32 vR         = Dqn_StrToF32(r, DQN_ARRAY_COUNT(r));
        DQN_ASSERT(DQN_ABS(vR) - DQN_ABS(9.64635e+05) < EPSILON);

        Log("Dqn_StrToF32()");
    }

    // UCS <-> UTF8 Checks
    if (1)
    {
        // Test ascii characters
        if (1)
        {
            u32 codepoint = '@';
            u32 string[1] = {};

            u32 bytes_used = Dqn_UCSToUTF8(&string[0], codepoint);
            DQN_ASSERT(bytes_used == 1);
            DQN_ASSERT(string[0] == '@');

            bytes_used = Dqn_UTF8ToUCS(&string[0], codepoint);
            DQN_ASSERT(string[0] >= 0 && string[0] < 0x80);
            DQN_ASSERT(bytes_used == 1);

            Log("Dqn_UTF8ToUCS(): Test ascii characters");
        }

        // Test 2 byte characters
        if (1)
        {
            u32 codepoint = 0x278;
            u32 string[1] = {};

            u32 bytes_used = Dqn_UCSToUTF8(&string[0], codepoint);
            DQN_ASSERT(bytes_used == 2);
            DQN_ASSERT(string[0] == 0xC9B8);

            bytes_used = Dqn_UTF8ToUCS(&string[0], string[0]);
            DQN_ASSERT(string[0] == codepoint);
            DQN_ASSERT(bytes_used == 2);

            Log("Dqn_UTF8ToUCS(): Test 2 byte characters");
        }

        // Test 3 byte characters
        if (1)
        {
            u32 codepoint = 0x0A0A;
            u32 string[1] = {};

            u32 bytes_used = Dqn_UCSToUTF8(&string[0], codepoint);
            DQN_ASSERT(bytes_used == 3);
            DQN_ASSERT(string[0] == 0xE0A88A);

            bytes_used = Dqn_UTF8ToUCS(&string[0], string[0]);
            DQN_ASSERT(string[0] == codepoint);
            DQN_ASSERT(bytes_used == 3);

            Log("Dqn_UTF8ToUCS(): Test 3 byte characters");
        }

        // Test 4 byte characters
        if (1)
        {
            u32 codepoint = 0x10912;
            u32 string[1] = {};
            u32 bytes_used = Dqn_UCSToUTF8(&string[0], codepoint);

            DQN_ASSERT(bytes_used == 4);
            DQN_ASSERT(string[0] == 0xF090A492);

            bytes_used = Dqn_UTF8ToUCS(&string[0], string[0]);
            DQN_ASSERT(string[0] == codepoint);
            DQN_ASSERT(bytes_used == 4);

            Log("Dqn_UTF8ToUCS(): Test 4 byte characters");
        }

        if (1)
        {
            u32 codepoint = 0x10912;
            u32 bytes_used = Dqn_UCSToUTF8(NULL, codepoint);
            DQN_ASSERT(bytes_used == 0);

            bytes_used = Dqn_UTF8ToUCS(NULL, codepoint);
            DQN_ASSERT(bytes_used == 0);

            Log("Dqn_UTF8ToUCS(): Test return result on on NULL output param");
        }
    }

}

void DqnStr_Test()
{
    // String Checks
    if (1)
    {
        LOG_HEADER();

        // strcmp
        if (1)
        {
            const char *const a = "str_a";

            // Check simple compares
            if (1)
            {
                DQN_ASSERT(DqnStr_Cmp(a, "str_a") == +0);
                DQN_ASSERT(DqnStr_Cmp(a, "str_b") == -1);
                DQN_ASSERT(DqnStr_Cmp("str_b", a) == +1);
                DQN_ASSERT(DqnStr_Cmp(a, "") == +1);
                DQN_ASSERT(DqnStr_Cmp("", "") == 0);

                // NOTE: Check that the string has not been trashed.
                DQN_ASSERT(DqnStr_Cmp(a, "str_a") == +0);
                Log("DqnStr_Cmp(): Check simple compares");
            }

            // Check ops against null
            if (1)
            {
                DQN_ASSERT(DqnStr_Cmp(NULL, NULL) != +0);
                DQN_ASSERT(DqnStr_Cmp(a, NULL) != +0);
                DQN_ASSERT(DqnStr_Cmp(NULL, a) != +0);
                Log("DqnStr_Cmp(): Check ops against null");
            }
        }

        // strlen
        if (1)
        {
            const char *const a = "str_a";
            DQN_ASSERT(DqnStr_Len(a) == 5);
            DQN_ASSERT(DqnStr_Len("") == 0);
            DQN_ASSERT(DqnStr_Len("   a  ") == 6);
            DQN_ASSERT(DqnStr_Len("a\n") == 2);

            // NOTE: Check that the string has not been trashed.
            DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);
            DQN_ASSERT(DqnStr_Len(NULL) == 0);

            Log("DqnStr_Len()");
        }

        // StrReverse
        if (1)
        {
            // Basic reverse operations
            if (1)
            {
                char a[] = "aba";
                DqnStr_Reverse(a, DqnStr_Len(a));
                DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);

                DqnStr_Reverse(a, 2);
                DQN_ASSERT(DqnStr_Cmp(a, "baa") == 0);

                DqnStr_Reverse(a, DqnStr_Len(a));
                DQN_ASSERT(DqnStr_Cmp(a, "aab") == 0);

                DqnStr_Reverse(&a[1], 2);
                DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);

                DqnStr_Reverse(a, 0);
                DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);
                Log("DqnStr_Reverse(): Basic reverse operations");
            }

            // Try reverse empty string
            if (1)
            {
                char a[] = "";
                DqnStr_Reverse(a, DqnStr_Len(a));
                DQN_ASSERT(DqnStr_Cmp(a, "") == 0);
                Log("DqnStr_Reverse(): Reverse empty string");
            }

            // Try reverse single char string
            if (1)
            {
                char a[] = "a";
                DqnStr_Reverse(a, DqnStr_Len(a));
                DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);

                DqnStr_Reverse(a, 0);
                DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);
                Log("DqnStr_Reverse(): Reverse single char string");
            }
        }

        if (1)
        {
            const char *const a = "Microsoft";
            const char *const b = "icro";
            i32 lenA            = DqnStr_Len(a);
            i32 lenB            = DqnStr_Len(b);
            DQN_ASSERT(DqnStr_HasSubstring(a, lenA, b, lenB) == true);
            DQN_ASSERT(DqnStr_HasSubstring(a, lenA, "iro", DqnStr_Len("iro")) == false);
            DQN_ASSERT(DqnStr_HasSubstring(b, lenB, a, lenA) == false);
            DQN_ASSERT(DqnStr_HasSubstring("iro", DqnStr_Len("iro"), a, lenA) == false);
            DQN_ASSERT(DqnStr_HasSubstring("", 0, "iro", 4) == false);
            DQN_ASSERT(DqnStr_HasSubstring("", 0, "", 0) == false);
            DQN_ASSERT(DqnStr_HasSubstring(NULL, 0, NULL, 0) == false);
            Log("DqnStr_HasSubstring(): Check string with matching substring");
        }

        if (1)
        {
            const char *const a = "Micro";
            const char *const b = "irob";
            i32 lenA            = DqnStr_Len(a);
            i32 lenB            = DqnStr_Len(b);
            DQN_ASSERT(DqnStr_HasSubstring(a, lenA, b, lenB) == false);
            DQN_ASSERT(DqnStr_HasSubstring(b, lenB, a, lenA) == false);
            Log("DqnStr_HasSubstring(): Check string with non-matching substring");
        }

    }
}

void DqnChar_Test()
{
    LOG_HEADER();

    // Char Checks
    if (1)
    {
        DQN_ASSERT(DqnChar_IsAlpha('a') == true);
        DQN_ASSERT(DqnChar_IsAlpha('A') == true);
        DQN_ASSERT(DqnChar_IsAlpha('0') == false);
        DQN_ASSERT(DqnChar_IsAlpha('@') == false);
        DQN_ASSERT(DqnChar_IsAlpha(' ') == false);
        DQN_ASSERT(DqnChar_IsAlpha('\n') == false);
        Log(Status::Ok, "IsAlpha");

        DQN_ASSERT(DqnChar_IsDigit('1') == true);
        DQN_ASSERT(DqnChar_IsDigit('n') == false);
        DQN_ASSERT(DqnChar_IsDigit('N') == false);
        DQN_ASSERT(DqnChar_IsDigit('*') == false);
        DQN_ASSERT(DqnChar_IsDigit(' ') == false);
        DQN_ASSERT(DqnChar_IsDigit('\n') == false);
        Log(Status::Ok, "IsDigit");

        DQN_ASSERT(DqnChar_IsAlphaNum('1') == true);
        DQN_ASSERT(DqnChar_IsAlphaNum('a') == true);
        DQN_ASSERT(DqnChar_IsAlphaNum('A') == true);
        DQN_ASSERT(DqnChar_IsAlphaNum('*') == false);
        DQN_ASSERT(DqnChar_IsAlphaNum(' ') == false);
        DQN_ASSERT(DqnChar_IsAlphaNum('\n') == false);
        Log(Status::Ok, "IsAlphaNum");

        DQN_ASSERT(DqnChar_ToLower(L'A') == L'a');
        DQN_ASSERT(DqnChar_ToLower(L'a') == L'a');
        DQN_ASSERT(DqnChar_ToLower(L' ') == L' ');
        Log(Status::Ok, "ToLower");

        DQN_ASSERT(DqnChar_ToUpper(L'A') == L'A');
        DQN_ASSERT(DqnChar_ToUpper(L'a') == L'A');
        DQN_ASSERT(DqnChar_ToUpper(L' ') == L' ');
        Log(Status::Ok, "ToUpper");

        DQN_ASSERT(DqnChar_IsWhitespace(' '));
        DQN_ASSERT(DqnChar_IsWhitespace('\r'));
        DQN_ASSERT(DqnChar_IsWhitespace('\n'));
        DQN_ASSERT(DqnChar_IsWhitespace('\t'));
        Log(Status::Ok, "IsWhiteSpace");
    }

    // Trim white space test
    if (1)
    {
        if (1)
        {
            char a[]     = "";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 0);
            DQN_ASSERT(result == nullptr);
        }

        if (1)
        {
            char a[]     = "a";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 1);
            DQN_ASSERT(result == a);
        }

        if (1)
        {
            char a[]     = " abc";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 3);
            DQN_ASSERT(result == (a + 1));
        }

        if (1)
        {
            char a[]     = "abc ";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 3);
            DQN_ASSERT(result == a);
        }

        if (1)
        {
            char a[]     = "   abc ";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 3);
            DQN_ASSERT(result == a + 3);
        }

        if (1)
        {
            char a[]     = "         ";
            i32 newLen   = 0;
            auto *result = DqnChar_TrimWhitespaceAround(a, DQN_CHAR_COUNT(a), &newLen);

            DQN_ASSERT(newLen == 0);
            DQN_ASSERT(result == nullptr);
        }

        Log(Status::Ok, "TrimWhitespaceAround");
    }
}

void DqnString_Test()
{
    LOG_HEADER();

    // Try expanding string
    if (1)
    {
        DqnString str = "hello world";
        DQN_DEFER { str.Free(); };
        str = "hello world2";
        str.Append(", hello again");
        str.Append(", and hello again");

        DQN_ASSERT(str.str[str.len] == 0);
        DQN_ASSERT(str.len <= str.max);
        DQN_ASSERTM(DqnStr_Cmp("hello world2, hello again, and hello again", str.str) == 0, "str: %s", str.str);

        str.Free();
        Log(Status::Ok, "Check expand on append");
    }

    {
        DqnString str  = DQN_BUFFER_STR_LIT("hello world");
        DQN_DEFER { str.Free(); };
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello world") == 0);

        Log(Status::Ok, "Copy constructor DqnSlice<char>");
    }

    {
        DqnString zero = {};
        DqnString str  = DQN_BUFFER_STR_LIT("hello world");
        str.Free();
        str = zero;

        DqnBuffer<char const> helloSlice = DQN_BUFFER_STR_LIT("hello");
        str = helloSlice;
        DQN_DEFER { str.Free(); };
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello") == 0);

        Log(Status::Ok, "Copy constructor (DqnFixedString<>)");
    }

    {
        DqnString str = DQN_BUFFER_STR_LIT("hello world");
        DQN_DEFER { str.Free(); };
        DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
        DQN_ASSERTM(DqnStr_Cmp(str.str, "hello sailor") == 0, "Result: %s", str.str);

        Log(Status::Ok, "Sprintf");
    }

    {
        {
            DqnString str = DQN_BUFFER_STR_LIT("hello world");
            DQN_DEFER { str.Free(); };
            DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
            str += DQN_BUFFER_STR_LIT(".end");
            DQN_ASSERTM(DqnStr_Cmp(str.str, "hello sailor.end") == 0, "Result: %s", str.str);
        }

        {
            DqnString str = DQN_BUFFER_STR_LIT("hello world");
            DQN_DEFER { str.Free(); };
            DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
            DQN_ASSERT(str.SprintfAppend(" %d, %d", 100, 200));
            DQN_ASSERT(DqnStr_Cmp(str.str, "hello sailor 100, 200") == 0);
        }

        Log(Status::Ok, "Concatenation, operator +=, SprintfAppend");
    }

    {
        DqnString str;
        str = "hello big world";
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello big world") == 0);
        str.Free();

        str = DqnString("goodbye", DQN_CHAR_COUNT("goodbye"));
        DQN_ASSERT(DqnStr_Cmp(str.str, "goodbye") == 0);
        Log(Status::Ok, "Copy constructor (char const *str, int len)");
    }

    {
        DqnString str = DQN_BUFFER_STR_LIT("hello world");
        DQN_DEFER { str.Free(); };
        DQN_ASSERT(str.Sprintf("hello %s", "sailor"));
        str = str + " end" + DQN_BUFFER_STR_LIT(" of");
        DQN_ASSERT(DqnStr_Cmp(str.str, "hello sailor end of") == 0);

        Log(Status::Ok, "Operator +");
    }

    {
        DqnString str = "localhost";
        DQN_DEFER { str.Free(); };
        str.SprintfAppend(":%d", 16832);
        str += "/json_rpc";
        DQN_ASSERT(str.len == 24 && DqnStr_Cmp("localhost:16832/json_rpc", str.str) == 0);

        Log(Status::Ok, "Copy constructor, sprintf, operator +=");
    }
}

void DqnRnd_Test()
{
    LOG_HEADER();

    auto pcg = DqnRndPCG();
    for (i32 i = 0; i < 1000000; i++)
    {
        i32 min    = -100;
        i32 max    = 1000000000;
        i32 result = pcg.Range(min, max);
        DQN_ASSERT(result >= min && result <= max);

        f32 rand_f32 = pcg.Nextf();
        DQN_ASSERT(rand_f32 >= 0.0f && rand_f32 <= 1.0f);
    }
    Log(Status::Ok, "DqnRndPCG");
}

void DqnMath_Test()
{
    LOG_HEADER();

    // Lerp
    if (1)
    {
        if (1)
        {
            f32 start = 10;
            f32 t     = 0.5f;
            f32 end   = 20;
            DQN_ASSERT(DqnMath_Lerp(start, t, end) == 15);
        }

        if (1)
        {
            f32 start = 10;
            f32 t     = 2.0f;
            f32 end   = 20;
            DQN_ASSERT(DqnMath_Lerp(start, t, end) == 30);
        }

        Log(Status::Ok, "Lerp");
    }

     // Sqrtf
    if (1)
    {
        DQN_ASSERT(DqnMath_Sqrtf(4.0f) == 2.0f);
        Log(Status::Ok, "Sqrtf");
    }

    // Handmade Math Test
    if (1)
    {
        if (1)
        {
            f32 aspect_ratio         = 1;
            DqnMat4 dqn_perspective  = DqnMat4_Perspective(90, aspect_ratio, 100, 1000);
            hmm_mat4 hmm_perspective = HMM_Perspective(90, aspect_ratio, 100, 1000);
            HandmadeMathVerifyMat4(dqn_perspective, hmm_perspective);

            Log(Status::Ok, "HandmadeMathTest: Perspective");
        }

        // Test Mat4 translate * scale
        if (1)
        {
            hmm_vec3 hmm_vec       = HMM_Vec3i(1, 2, 3);
            DqnV3 dqn_vec          = DqnV3(1, 2, 3);
            DqnMat4 dqn_translate  = DqnMat4_Translate3f(dqn_vec.x, dqn_vec.y, dqn_vec.z);
            hmm_mat4 hmm_translate = HMM_Translate(hmm_vec);
            HandmadeMathVerifyMat4(dqn_translate, hmm_translate);

            hmm_vec3 hmm_axis      = HMM_Vec3(0.5f, 0.2f, 0.7f);
            DqnV3 dqn_axis         = DqnV3(0.5f, 0.2f, 0.7f);
            f32 rotation_in_degrees = 80.0f;

            DqnMat4 dqn_rotate = DqnMat4_Rotate(DQN_DEGREES_TO_RADIANS(rotation_in_degrees), dqn_axis.x,
                                               dqn_axis.y, dqn_axis.z);
            hmm_mat4 hmm_rotate = HMM_Rotate(rotation_in_degrees, hmm_axis);
            HandmadeMathVerifyMat4(dqn_rotate, hmm_rotate);

            dqn_vec *= 2;
            hmm_vec *= 2;
            DqnMat4 dqn_scale  = DqnMat4_Scale(dqn_vec.x, dqn_vec.y, dqn_vec.z);
            hmm_mat4 hmm_scale = HMM_Scale(hmm_vec);
            HandmadeMathVerifyMat4(dqn_scale, hmm_scale);

            DqnMat4 dqn_ts_matrix  = DqnMat4_Mul(dqn_translate, dqn_scale);
            hmm_mat4 hmm_ts_matrix = HMM_MultiplyMat4(hmm_translate, hmm_scale);
            HandmadeMathVerifyMat4(dqn_ts_matrix, hmm_ts_matrix);

            // Test Mat4 * MulV4
            if (1)
            {
                DqnV4 dqn_v4    = DqnV4(1, 2, 3, 4);
                hmm_vec4 hmm_v4 = HMM_Vec4(1, 2, 3, 4);

                DqnV4 dqn_result    = DqnMat4_MulV4(dqn_ts_matrix, dqn_v4);
                hmm_vec4 hmmResult = HMM_MultiplyMat4ByVec4(hmm_ts_matrix, hmm_v4);

                DQN_ASSERT(dqn_result.x == hmmResult.X);
                DQN_ASSERT(dqn_result.y == hmmResult.Y);
                DQN_ASSERT(dqn_result.z == hmmResult.Z);
                DQN_ASSERT(dqn_result.w == hmmResult.W);

                Log(Status::Ok, "HandmadeMathTest: Mat4 * MulV4");
            }
        }
    }
}

void DqnVX_Test()
{
    LOG_HEADER();

     // V2
    if (1)
    {
        // Ctor
        if (1)
        {
            // Ctor with floats
            if (1)
            {
                DqnV2 vec = DqnV2(5.5f, 5.0f);
                DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f);
                DQN_ASSERT(vec.w == 5.5f && vec.h == 5.0f);
            }

            // Ctor with 2 integers
            if (1)
            {
                DqnV2 vec = DqnV2(3, 5);
                DQN_ASSERT(vec.x == 3 && vec.y == 5.0f);
                DQN_ASSERT(vec.w == 3 && vec.h == 5.0f);
            }
            Log(Status::Ok, "DqnV2: Ctor");
        }

        // V2 Arithmetic
        if (1)
        {
            DqnV2 vec_a = DqnV2(5, 10);
            DqnV2 vec_b = DqnV2(2, 3);
            DQN_ASSERT(DqnV2_Equals(vec_a, vec_b) == false);
            DQN_ASSERT(DqnV2_Equals(vec_a, DqnV2(5, 10)) == true);
            DQN_ASSERT(DqnV2_Equals(vec_b, DqnV2(2, 3)) == true);

            DqnV2 result = DqnV2_Add(vec_a,  DqnV2(5, 10));
            DQN_ASSERT(DqnV2_Equals(result, DqnV2(10, 20)) == true);

            result = DqnV2_Sub(result, DqnV2(5, 10));
            DQN_ASSERT(DqnV2_Equals(result, DqnV2(5, 10)) == true);

            result = DqnV2_Scalef(result, 5);
            DQN_ASSERT(DqnV2_Equals(result, DqnV2(25, 50)) == true);

            result = DqnV2_Hadamard(result, DqnV2(10.0f, 0.5f));
            DQN_ASSERT(DqnV2_Equals(result, DqnV2(250, 25)) == true);

            f32 dot_result = DqnV2_Dot(DqnV2(5, 10), DqnV2(3, 4));
            DQN_ASSERT(dot_result == 55);
            Log(Status::Ok, "DqnV2: Arithmetic");
        }

        // Test operator overloading
        if (1)
        {
            DqnV2 vec_a = DqnV2(5, 10);
            DqnV2 vec_b = DqnV2(2, 3);
            DQN_ASSERT((vec_a == vec_b) == false);
            DQN_ASSERT((vec_a == DqnV2(5, 10)) == true);
            DQN_ASSERT((vec_b == DqnV2(2, 3)) == true);

            DqnV2 result = vec_a + DqnV2(5, 10);
            DQN_ASSERT((result == DqnV2(10, 20)) == true);

            result -= DqnV2(5, 10);
            DQN_ASSERT((result == DqnV2(5, 10)) == true);

            result *= 5;
            DQN_ASSERT((result == DqnV2(25, 50)) == true);

            result = result * DqnV2(10.0f, 0.5f);
            DQN_ASSERT((result == DqnV2(250, 25)) == true);

            result += DqnV2(1, 1);
            DQN_ASSERT((result == DqnV2(251, 26)) == true);

            result = result - DqnV2(1, 1);
            DQN_ASSERT((result == DqnV2(250, 25)) == true);
            Log(Status::Ok, "DqnV2: Operator Overloading");
        }

        // V2 Properties
        if (1)
        {
            const f32 EPSILON = 0.001f;
            DqnV2 a           = DqnV2(0, 0);
            DqnV2 b           = DqnV2(3, 4);

            f32 len_sq = DqnV2_LengthSquared(a, b);
            DQN_ASSERT(len_sq == 25);

            f32 length = DqnV2_Length(a, b);
            DQN_ASSERT(length == 5);

            DqnV2 normalised = DqnV2_Normalise(b);
            f32 norm_x        = b.x / 5.0f;
            f32 norm_y        = b.y / 5.0f;
            f32 diff_norm_x    = normalised.x - norm_x;
            f32 diff_norm_y    = normalised.y - norm_y;
            DQN_ASSERTM(diff_norm_x < EPSILON, "normalised.x: %f, norm_x: %f\n", normalised.x, norm_x);
            DQN_ASSERTM(diff_norm_y < EPSILON, "normalised.y: %f, norm_y: %f\n", normalised.y, norm_y);

            DqnV2 c = DqnV2(3.5f, 8.0f);
            DQN_ASSERT(DqnV2_Overlaps(b, c) == true);
            DQN_ASSERT(DqnV2_Overlaps(b, a) == false);

            DqnV2 d = DqnV2_Perpendicular(c);
            DQN_ASSERT(DqnV2_Dot(c, d) == 0);

            Log(Status::Ok, "DqnV2: LengthSquared, Length, Normalize, Overlaps, Perp");
        }

        // ConstrainToRatio
        if (1)
        {
            DqnV2 ratio  = DqnV2(16, 9);
            DqnV2 dim    = DqnV2(2000, 1080);
            DqnV2 result = DqnV2_ConstrainToRatio(dim, ratio);
            DQN_ASSERT(result.w == 1920 && result.h == 1080);
            Log(Status::Ok, "DqnV2: ConstrainToRatio");
        }
    }

    // V3
    if (1)
    {
        // Ctor
        if (1)
        {
            // Floats
            if (1)
            {
                DqnV3 vec = DqnV3(5.5f, 5.0f, 5.875f);
                DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f);
                DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f);
            }

            // Integers
            if (1)
            {
                DqnV3 vec = DqnV3(3, 4, 5);
                DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5);
                DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5);
            }
            Log(Status::Ok, "DqnV3: Ctor");
        }

        if (1)
        {
            // Arithmetic
            if (1)
            {
                DqnV3 vec_a = DqnV3(5, 10, 15);
                DqnV3 vec_b = DqnV3(2, 3, 6);
                DQN_ASSERT(DqnV3_Equals(vec_a, vec_b) == false);
                DQN_ASSERT(DqnV3_Equals(vec_a, DqnV3(5, 10, 15)) == true);
                DQN_ASSERT(DqnV3_Equals(vec_b, DqnV3(2, 3, 6)) == true);

                DqnV3 result = DqnV3_Add(vec_a, DqnV3(5, 10, 15));
                DQN_ASSERT(DqnV3_Equals(result, DqnV3(10, 20, 30)) == true);

                result = DqnV3_Sub(result, DqnV3(5, 10, 15));
                DQN_ASSERT(DqnV3_Equals(result, DqnV3(5, 10, 15)) == true);

                result = DqnV3_Scalef(result, 5);
                DQN_ASSERT(DqnV3_Equals(result, DqnV3(25, 50, 75)) == true);

                result = DqnV3_Hadamard(result, DqnV3(10.0f, 0.5f, 10.0f));
                DQN_ASSERT(DqnV3_Equals(result, DqnV3(250, 25, 750)) == true);

                f32 dot_result = DqnV3_Dot(DqnV3(5, 10, 2), DqnV3(3, 4, 6));
                DQN_ASSERT(dot_result == 67);

                DqnV3 cross = DqnV3_Cross(vec_a, vec_b);
                DQN_ASSERT(DqnV3_Equals(cross, DqnV3(15, 0, -5)) == true);
            }

            // Operator overloading
            if (1)
            {
                DqnV3 vec_a = DqnV3(5, 10, 15);
                DqnV3 vec_b = DqnV3(2, 3, 6);
                DQN_ASSERT((vec_a == vec_b) == false);
                DQN_ASSERT((vec_a == DqnV3(5, 10, 15)) == true);
                DQN_ASSERT((vec_b == DqnV3(2, 3, 6)) == true);

                DqnV3 result = vec_a + DqnV3(5, 10, 15);
                DQN_ASSERT((result == DqnV3(10, 20, 30)) == true);

                result -= DqnV3(5, 10, 15);
                DQN_ASSERT((result == DqnV3(5, 10, 15)) == true);

                result = result * 5;
                DQN_ASSERT((result == DqnV3(25, 50, 75)) == true);

                result *= DqnV3(10.0f, 0.5f, 10.0f);
                DQN_ASSERT((result == DqnV3(250, 25, 750)) == true);

                result = result - DqnV3(1, 1, 1);
                DQN_ASSERT((result == DqnV3(249, 24, 749)) == true);

                result += DqnV3(1, 1, 1);
                DQN_ASSERT((result == DqnV3(250, 25, 750)) == true);
            }
            Log(Status::Ok, "DqnV3: Arithmetic");
        }
    }

     // V4
    if (1)
    {

        // Ctor
        if (1)
        {
            // Floats
            if (1)
            {
                DqnV4 vec = DqnV4(5.5f, 5.0f, 5.875f, 5.928f);
                DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f && vec.w == 5.928f);
                DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f && vec.a == 5.928f);
            }

            // Integers
            if (1)
            {
                DqnV4 vec = DqnV4(3, 4, 5, 6);
                DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5 && vec.w == 6);
                DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5 && vec.a == 6);
            }
            Log(Status::Ok, "DqnV4: Ctor");
        }
        // V4 Arithmetic
        if (1)
        {
            // Arithmetic
            {
                DqnV4 vec_a = DqnV4(5, 10, 15, 20);
                DqnV4 vec_b = DqnV4(2, 3, 6, 8);
                DQN_ASSERT(DqnV4_Equals(vec_a, vec_b) == false);
                DQN_ASSERT(DqnV4_Equals(vec_a, DqnV4(5, 10, 15, 20)) == true);
                DQN_ASSERT(DqnV4_Equals(vec_b, DqnV4(2, 3, 6, 8)) == true);

                DqnV4 result = DqnV4_Add(vec_a, DqnV4(5, 10, 15, 20));
                DQN_ASSERT(DqnV4_Equals(result, DqnV4(10, 20, 30, 40)) == true);

                result = DqnV4_Sub(result, DqnV4(5, 10, 15, 20));
                DQN_ASSERT(DqnV4_Equals(result, DqnV4(5, 10, 15, 20)) == true);

                result = DqnV4_Scalef(result, 5);
                DQN_ASSERT(DqnV4_Equals(result, DqnV4(25, 50, 75, 100)) == true);

                result = DqnV4_Hadamard(result, DqnV4(10.0f, 0.5f, 10.0f, 0.25f));
                DQN_ASSERT(DqnV4_Equals(result, DqnV4(250, 25, 750, 25)) == true);

                f32 dot_result = DqnV4_Dot(DqnV4(5, 10, 2, 8), DqnV4(3, 4, 6, 5));
                DQN_ASSERT(dot_result == 107);
            }

            // Operator Overloading
            if (1)
            {
                DqnV4 vec_a = DqnV4(5, 10, 15, 20);
                DqnV4 vec_b = DqnV4(2, 3, 6, 8);
                DQN_ASSERT((vec_a == vec_b) == false);
                DQN_ASSERT((vec_a == DqnV4(5, 10, 15, 20)) == true);
                DQN_ASSERT((vec_b == DqnV4(2, 3, 6, 8)) == true);

                DqnV4 result = vec_a + DqnV4(5, 10, 15, 20);
                DQN_ASSERT((result == DqnV4(10, 20, 30, 40)) == true);

                result = result - DqnV4(5, 10, 15, 20);
                DQN_ASSERT((result == DqnV4(5, 10, 15, 20)) == true);

                result = result * 5;
                DQN_ASSERT((result == DqnV4(25, 50, 75, 100)) == true);

                result *= DqnV4(10.0f, 0.5f, 10.0f, 0.25f);
                DQN_ASSERT((result == DqnV4(250, 25, 750, 25)) == true);

                result += DqnV4(1, 1, 1, 1);
                DQN_ASSERT((result == DqnV4(251, 26, 751, 26)) == true);

                result -= DqnV4(1, 1, 1, 1);
                DQN_ASSERT((result == DqnV4(250, 25, 750, 25)) == true);
            }
            Log(Status::Ok, "DqnV4: Arithmetic");
        }
    }
}

void DqnRect_Test()
{
    LOG_HEADER();
    // Rect
    if (1)
    {
        // Test rect init functions
        if (1)
        {
            DqnRect rect4f = DqnRect(1.1f, 2.2f, 3.3f, 4.4f);
            DqnRect rect4i = DqnRect(1, 2, 3, 4);

            DQN_ASSERT(rect4i.min.x == 1 && rect4i.min.y == 2);
            DQN_ASSERT(rect4i.max.x == 4 && rect4i.max.y == 6);

            const f32 EPSILON = 0.001f;
            f32 diff_max_x      = rect4f.max.x - 4.4f;
            f32 diff_max_y      = rect4f.max.y - 6.6f;
            DQN_ASSERT(rect4f.min.x == 1.1f && rect4f.min.y == 2.2f);
            DQN_ASSERT(DQN_ABS(diff_max_x) < EPSILON && DQN_ABS(diff_max_y) < EPSILON);

            DqnRect rect = DqnRect(-10, -10, 20, 20);
            DQN_ASSERT(DqnV2_Equals(rect.min, DqnV2(-10, -10)));
            DQN_ASSERT(DqnV2_Equals(rect.max, DqnV2(10, 10)));
            Log(Status::Ok, "Ctor");
        }

        // Test rect get size function
        if (1)
        {
            // Test float rect
            if (1)
            {
                DqnRect rect = DqnRect(DqnV2(-10, -10), DqnV2(20, 20));

                f32 width, height;
                rect.GetSize(&width, &height);
                DQN_ASSERT(width == 20);
                DQN_ASSERT(height == 20);

                DqnV2 dim = rect.GetSize();
                DQN_ASSERT(DqnV2_Equals(dim, DqnV2(20, 20)));
                Log(Status::Ok, "GetSize");
            }
        }

        // Test rect get centre
        DqnRect rect     = DqnRect(DqnV2(-10, -10), DqnV2(20, 20));
        DqnV2 rect_centre = rect.GetCenter();
        DQN_ASSERT(DqnV2_Equals(rect_centre, DqnV2(0, 0)));
        Log(Status::Ok, "GetCentre");

        // Test clipping rect get centre
        DqnRect clip_rect   = DqnRect(DqnV2(-15, -15), DqnV2(10, 10) + DqnV2(15));
        DqnRect clip_result = rect.ClipRect(clip_rect);
        DQN_ASSERT(clip_result.min.x == -10 && clip_result.min.y == -10);
        DQN_ASSERT(clip_result.max.x == 10 && clip_result.max.y == 10);
        Log(Status::Ok, "ClipRect");

        // Test shifting rect
        if (1)
        {
            DqnRect shifted_rect = rect.Move(DqnV2(10, 0));
            DQN_ASSERT(DqnV2_Equals(shifted_rect.min, DqnV2(0, -10)));
            DQN_ASSERT(DqnV2_Equals(shifted_rect.max, DqnV2(20, 10)));

            // Ensure dimensions have remained the same
            if (1)
            {
                f32 width, height;
                shifted_rect.GetSize(&width, &height);
                DQN_ASSERT(width == 20);
                DQN_ASSERT(height == 20);

                DqnV2 dim = shifted_rect.GetSize();
                DQN_ASSERT(DqnV2_Equals(dim, DqnV2(20, 20)));
            }

            // Test rect contains p
            if (1)
            {
                DqnV2 inP  = DqnV2(5, 5);
                DqnV2 outP = DqnV2(100, 100);
                DQN_ASSERT(shifted_rect.ContainsP(inP));
                DQN_ASSERT(!shifted_rect.ContainsP(outP));
            }

            Log(Status::Ok, "Move");
        }
    }
}

void DqnArray_TestRealDataInternal(DqnArray<char> *array)
{
    (void)array;
#ifdef DQN_PLATFORM_HEADER
    size_t buf_size = 0;
    u8 *buf        = DqnFile_ReadAll("tests/google-10000-english.txt", &buf_size);
    DQN_ASSERT(buf);

    for (usize i = 0; i < buf_size; i++)
        array->Push(buf[i]);

    DQN_ASSERT((size_t)array->len == buf_size);
    for (auto i = 0; i < array->len; i++)
        DQN_ASSERT(array->data[i] == buf[i]);

    array->Free();
    free(buf);

    Log(Status::Ok, "Testing real data");
#endif
}

void DqnArray_Test()
{
    LOG_HEADER();
    if (1)
    {
        if (1)
        {
            DqnArray<char> array1 = {};
            array1.Reserve(3);
            DQN_ASSERT(array1.len == 0);
            DQN_ASSERT(array1.max == 3);
            array1.Free();

            array1.Reserve(0);
            DQN_ASSERT(array1.len == 0);
            DQN_ASSERT(array1.max == 0);

            array1.Push('c');
            DQN_ASSERT(array1.len == 1);
            array1.Free();

            Log(Status::Ok, "Testing faux-array constructors DqnArray_()");
        }

        if (1)
        {
            DqnArray<char> array = {};
            array.Reserve(1);
            DqnArray_TestRealDataInternal(&array);
        }

        if (1)
        {
            auto stack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
            DQN_DEFER { stack.Free(); };
#if 0
            if (1)
            {
                auto memGuard0 = stack.TempRegionGuard();
                DqnArray<char> array(&stack.myHeadAPI);
                array.Reserve(1);
                DqnArray_TestRealDataInternal(&array);
            }

            // Test reallocing strategies for memory stacks
            if (1)
            {
                auto memGuard0 = stack.TempRegionGuard();
                DqnArray<char> array(&stack.myHeadAPI);
                array.Reserve(128);
                stack.Push(1024);
                DqnArray_TestRealDataInternal(&array);
            }
#endif
        }
    }
}

#ifdef DQN_PLATFORM_HEADER
void DqnFile_Test()
{
    LOG_HEADER();
    // File i/o
    if (1)
    {

        // Test file open
        if (1)
        {
            const char *const FILE_TO_OPEN = ".clang-format";
            u32 expectedSize               = 0;
#if defined(DQN__IS_UNIX)
            {
                struct stat fileStat = {0};
                DQN_ASSERT(stat(FILE_TO_OPEN, &fileStat) == 0);
                expectedSize = fileStat.st_size;
            }

            if (1)
            {
                // NOTE: cpuinfo is generated when queried, so a normal 'stat'
                // should give us zero, but we fall back to manual byte checking
                // which should give us the proper size.
                size_t size = 0;
                DQN_ASSERT(DqnFile_Size("/proc/cpuinfo", &size));
                DQN_ASSERT(size > 0);
            }

#else
            {
                HANDLE handle = CreateFile(FILE_TO_OPEN, GENERIC_READ, 0, NULL, OPEN_EXISTING,
                                           FILE_ATTRIBUTE_NORMAL, NULL);
                if (handle == INVALID_HANDLE_VALUE)
                {
                    DqnWin32_DisplayLastError("CreateFile() failed");
                }
                DQN_ASSERT(handle != INVALID_HANDLE_VALUE);

                LARGE_INTEGER size;
                DQN_ASSERT(GetFileSizeEx(handle, &size));

                CloseHandle(handle);
                expectedSize = size.LowPart;
            }
#endif

            if (1)
            {
                size_t size = 0;
                DQN_ASSERT(DqnFile_Size(FILE_TO_OPEN, &size));
                DQN_ASSERT(size == expectedSize);
            }

            DqnFile file = {};
            DQN_ASSERT(file.Open(
                ".clang-format", DqnFile::Flag::FileReadWrite, DqnFile::Action::OpenOnly));

            DQN_ASSERTM(file.size == expectedSize,
                           "DqnFileOpen() failed: file.size: %d, expected:%d\n", file.size,
                           expectedSize);

            u8 *buffer = (u8 *)calloc(1, (size_t)file.size * sizeof(u8));
            DQN_ASSERT(file.Read(buffer, (u32)file.size) == file.size);
            free(buffer);

            file.Close();
            DQN_ASSERT(!file.handle && file.size == 0 && file.flags == 0);

            Log(Status::Ok, "General test");
        }

        // Test invalid file
        if (1)
        {
            DqnFile file = {};
            DQN_ASSERT(!file.Open(
                "asdljasdnel;kajdf", DqnFile::Flag::FileReadWrite, DqnFile::Action::OpenOnly));
            DQN_ASSERT(file.size == 0);
            DQN_ASSERT(file.flags == 0);
            DQN_ASSERT(!file.handle);
            Log(Status::Ok, "Invalid file test");
        }
    }

    // Write Test
    if (1)
    {
        const char *file_names[]                   = {"dqn_1", "dqn_2", "dqn_3", "dqn_4", "dqn_5"};
        const char *writeData[]                   = {"1234", "2468", "36912", "481216", "5101520"};
        DqnFile files[DQN_ARRAY_COUNT(file_names)] = {};

        // Write data out to some files
        for (u32 i = 0; i < DQN_ARRAY_COUNT(file_names); i++)
        {
            u32 permissions = DqnFile::Flag::FileReadWrite;
            DqnFile *file   = files + i;
            if (!file->Open(file_names[i], permissions, DqnFile::Action::ClearIfExist))
            {
                bool result =
                    file->Open(file_names[i], permissions, DqnFile::Action::CreateIfNotExist);
                DQN_ASSERT(result);
            }

            size_t bytesToWrite = DqnStr_Len(writeData[i]);
            u8 *dataToWrite     = (u8 *)(writeData[i]);
            size_t bytesWritten = file->Write(dataToWrite, bytesToWrite);
            DQN_ASSERT(bytesWritten == bytesToWrite);
            file->Close();
        }

        auto memstack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);
        // Read data back in
        for (u32 i = 0; i < DQN_ARRAY_COUNT(file_names); i++)
        {
            // Manual read the file contents
            {
                u32 permissions = DqnFile::Flag::FileRead;
                DqnFile *file = files + i;
                bool result = file->Open(file_names[i], permissions, DqnFile::Action::OpenOnly);
                DQN_ASSERT(result);

                u8 *buffer = (u8 *)memstack.Push(file->size);
                DQN_ASSERT(buffer);

                size_t bytesRead = file->Read(buffer, file->size);
                DQN_ASSERT(bytesRead == file->size);

                // Verify the data is the same as we wrote out
                DQN_ASSERT(DqnStr_Cmp((char *)buffer, (writeData[i]), (i32)bytesRead) == 0);

                // Delete when we're done with it
                memstack.Pop(buffer);
                file->Close();
            }

            // Read using the ReadEntireFile api which doesn't need a file handle as an argument
            {
                size_t reqSize = 0;
                DQN_ASSERT(DqnFile_Size(file_names[i], &reqSize));

                u8 *buffer = (u8 *)memstack.Push(reqSize);
                DQN_ASSERT(buffer);

                DQN_ASSERT(DqnFile_ReadAll(file_names[i], buffer, reqSize));

                // Verify the data is the same as we wrote out
                DQN_ASSERT(DqnStr_Cmp((char *)buffer, (writeData[i]), (i32)reqSize) == 0);
                memstack.Pop(buffer);
            }

            DQN_ASSERT(DqnFile_Delete(file_names[i]));
        }

        // Then check delete actually worked, files should not exist.
        for (u32 i = 0; i < DQN_ARRAY_COUNT(file_names); i++)
        {
            DqnFile dummy   = {};
            u32 permissions = DqnFile::Flag::FileRead;
            bool fileExists = dummy.Open(file_names[i], permissions, DqnFile::Action::OpenOnly);
            DQN_ASSERT(!fileExists);
        }
        memstack.Free();

        Log(Status::Ok, "Write file");
    }

    // Test directory listing
    if (1)
    {
        i32 num_files;
#if defined(DQN___IS_UNIX)
        char **file_list = DqnFile_ListDir(".", &num_files);
#else
        char **file_list = DqnFile_ListDir("*", &num_files);
#endif

        Log("Test directory listing");
        global_indent++;
        for (auto i = 0; i < num_files; i++)
            Log("%02d: %s", i, file_list[i]);

        DqnFile_ListDirFree(file_list, num_files);
        global_indent--;
        Log(Status::Ok, "List directory files");
    }

}

void PlatformSleep(int milliseconds)
{
#if defined(DQN__IS_UNIX)
    usleep(milliseconds * 1000);
#else
    Sleep(milliseconds);
#endif
}

void DqnTimer_Test()
{
    LOG_HEADER();

    if (1)
    {

        int sleepTimeInMs = 250;
        f64 start_in_ms = DqnTimer_NowInMs();
        PlatformSleep(sleepTimeInMs);
        f64 end_in_ms = DqnTimer_NowInMs();

        DQN_ASSERT((start_in_ms + sleepTimeInMs) <= end_in_ms);
        Log("start: %f, end: %f", start_in_ms, end_in_ms);

        Log(Status::Ok, "Timer advanced in time over 1 second");
        global_indent++;
        Log("Start: %f, End: %f", start_in_ms, end_in_ms);
        global_indent--;
    }
}


FILE_SCOPE u32 volatile global_debug_counter;
FILE_SCOPE DqnLock global_job_queue_lock;
const u32 QUEUE_SIZE = 256;
FILE_SCOPE void JobQueueDebugCallbackIncrementCounter(DqnJobQueue *const queue, void *const user_data)
{
    (void)user_data;
    DQN_ASSERT(queue->size == QUEUE_SIZE);
    {
        auto guard = global_job_queue_lock.Guard();
        global_debug_counter++;

        // u32 number = global_debug_counter;
#if defined(DQN__IS_WIN32)
        // Log("JobQueueDebugCallbackIncrementCounter(): Thread %d: Incrementing Number: %d", GetCurrentThreadId(), number);
#else
        // Log("JobQueueDebugCallbackIncrementCounter(): Thread unix: Incrementing Number: %d", number);
#endif
    }

}

FILE_SCOPE void DqnJobQueue_Test()
{
    LOG_HEADER();
    global_debug_counter = 0;

    auto memstack = DqnMemStack(DQN_MEGABYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);

    u32 num_threads, num_cores;
    DqnOS_GetThreadsAndCores(&num_cores, &num_threads);
    DQN_ASSERT(num_threads > 0 && num_cores > 0);

    u32 total_threads = (num_cores - 1) * num_threads;
    if (total_threads == 0) total_threads = 1;

    DqnJobQueue job_queue = {};
    DqnJob *job_list      = (DqnJob *)memstack.Push(sizeof(*job_queue.job_list) * QUEUE_SIZE);
    DQN_ASSERT(DqnJobQueue_Init(&job_queue, job_list, QUEUE_SIZE, total_threads));

    const u32 WORK_ENTRIES = 2048;
    DQN_ASSERT(global_job_queue_lock.Init());
    for (u32 i = 0; i < WORK_ENTRIES; i++)
    {
        DqnJob job   = {};
        job.callback = JobQueueDebugCallbackIncrementCounter;
        while (!DqnJobQueue_AddJob(&job_queue, job))
        {
            DqnJobQueue_TryExecuteNextJob(&job_queue);
        }
    }

    DqnJobQueue_BlockAndCompleteAllJobs(&job_queue);
    DQN_ASSERT(global_debug_counter == WORK_ENTRIES);
    global_job_queue_lock.Delete();

    Log("Final incremented value: %d\n", global_debug_counter);
}

#else
f64 DqnTimer_NowInMs() { return 0; }
f64 DqnTimer_NowInS()  { return 0; }
#endif // DQN_PLATFORM_HEADER

#include <algorithm>
void DqnQuickSort_Test()
{
    LOG_HEADER();
    auto state = DqnRndPCG();
    if (1)
    {
        auto stack = DqnMemStack(DQN_KILOBYTE(1), Dqn::ZeroMem::Yes, DqnMemStack::Flag::BoundsGuard);

        // Create array of ints
        u32 num_ints      = 1000000;
        u32 size_in_bytes  = sizeof(u32) * num_ints;
        u32 *dqn_cpp_array = (u32 *)stack.Push(size_in_bytes);
        u32 *std_array    = (u32 *)stack.Push(size_in_bytes);
        DQN_ASSERT(dqn_cpp_array && std_array);

        f64 dqn_cpp_timings[2]                           = {};
        f64 std_timings[DQN_ARRAY_COUNT(dqn_cpp_timings)] = {};

        f64 dqn_cpp_avg = 0;
        f64 std_avg    = 0;

        Log("Timings"); global_indent++;
        for (u32 timingsIndex = 0; timingsIndex < DQN_ARRAY_COUNT(dqn_cpp_timings); timingsIndex++)
        {
            // Populate with random numbers
            for (u32 i = 0; i < num_ints; i++)
            {
                dqn_cpp_array[i] = state.Next();
                std_array[i]    = dqn_cpp_array[i];
            }

            global_new_line = false;
            Log("%02d: ", timingsIndex);
            global_indent -= 2;
            // Time Dqn_QuickSort
            {
                f64 start = DqnTimer_NowInS();
                DqnQuickSort(dqn_cpp_array, num_ints);

                f64 duration = DqnTimer_NowInS() - start;
                dqn_cpp_timings[timingsIndex] = duration;
                dqn_cpp_avg += duration;
                Log("Dqn_QuickSort: %f vs ", dqn_cpp_timings[timingsIndex]);
            }

            // Time std::sort
            global_new_line = true;
            {
                f64 start = DqnTimer_NowInS();
                std::sort(std_array, std_array + num_ints);
                f64 duration = DqnTimer_NowInS() - start;

                std_timings[timingsIndex] = duration;
                std_avg += duration;

                Log("std::sort: %f", std_timings[timingsIndex]);
            }
            global_indent += 2;

            // Validate algorithm is correct
            for (u32 i = 0; i < num_ints; i++)
            {
                DQN_ASSERTM(dqn_cpp_array[i] == std_array[i], "DqnArray[%d]: %d, std_array[%d]: %d", i,
                            dqn_cpp_array[i], std_array[i], i);
            }
        }
        global_indent--;

        // Print averages
        if (1)
        {
            dqn_cpp_avg /= (f64)DQN_ARRAY_COUNT(dqn_cpp_timings);
            std_avg    /= (f64)DQN_ARRAY_COUNT(std_timings);
            Log("Average Timings");
            global_indent++;
            Log("Dqn_QuickSort: %f vs std::sort: %f\n", dqn_cpp_avg, std_avg);
            global_indent--;
        }
        stack.Free();
        Log(Status::Ok, "QuickSort");
    }
}

void DqnBSearch_Test()
{
    LOG_HEADER();
    if (1)
    {
        u32 array[] = {1, 2, 3};
        i64 result = DqnBSearch<u32>(array, DQN_ARRAY_COUNT(array), 1);
        DQN_ASSERT(result == 0);

        result = DqnBSearch<u32>(array, DQN_ARRAY_COUNT(array), 2);
        DQN_ASSERT(result == 1);

        result = DqnBSearch<u32>(array, DQN_ARRAY_COUNT(array), 3);
        DQN_ASSERT(result == 2);

        result = DqnBSearch<u32>(array, DQN_ARRAY_COUNT(array), 4);
        DQN_ASSERT(result == -1);
        Log(Status::Ok, "With odd sized array and custom compare");
    }

    if (1)
    {
        i64 array[] = {1, 2, 3, 4};
        i64 result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 1);
        DQN_ASSERT(result == 0);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 2);
        DQN_ASSERT(result == 1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 3);
        DQN_ASSERT(result == 2);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 4);
        DQN_ASSERT(result == 3);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 5);
        DQN_ASSERT(result == -1);
        Log(Status::Ok, "With even sized array");
    }

    if (1)
    {
        i64 array[] = {1, 2, 3};
        i64 result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 0, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 1, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 2, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 0);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 3, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 4, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 2);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 5, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 2);
        Log(Status::Ok, "Lower bound with odd sized array");
    }

    if (1)
    {
        i64 array[] = {1, 2, 3, 4};

        i64 result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 0, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 1, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 2, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 0);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 3, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 4, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 2);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 5, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 3);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 6, DqnBSearchType::MinusOne);
        DQN_ASSERT(result == 3);
        Log(Status::Ok, "Lower bound with even sized array");
    }

    if (1)
    {
        i64 array[] = {1, 2, 3};
        i64 result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 0, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 0);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 1, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 2, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 2);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 3, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 4, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 5, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);
        Log(Status::Ok, "Higher bound with odd sized array");
    }

    if (1)
    {
        i64 array[] = {1, 2, 3, 4};

        i64 result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 0, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 0);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 1, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 2, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 2);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 3, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == 3);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 4, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 5, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);

        result = DqnBSearch(array, DQN_ARRAY_COUNT(array), 6, DqnBSearchType::PlusOne);
        DQN_ASSERT(result == -1);
        Log(Status::Ok, "Higher bound with even sized array");
    }
}

void DqnMemSet_Test()
{
    LOG_HEADER();
    auto rnd = DqnRndPCG();

    const int NUM_TIMINGS = 5;
    f64 timings[2][NUM_TIMINGS] = {};
    f64 avg_timings[DQN_ARRAY_COUNT(timings)] = {};
    void *buffers[DQN_ARRAY_COUNT(timings)]  = {};

    const i32 NUM_ITERATIONS = DQN_ARRAY_COUNT(timings[0]);
    Log("Timings");
    for (auto i = 0; i < NUM_ITERATIONS; i++)
    {
        i32 size = rnd.Range(DQN_MEGABYTE(16), DQN_MEGABYTE(32));
        u8 value = (u8)rnd.Range(0, 255);

        global_indent++;
        global_new_line = false;
        Log("%02d: ", i);
        global_indent--;
        global_indent--;

        i32 timingsIndex = 0;
        // DqnMem_Set
        {
            buffers[timingsIndex] = malloc(size); DQN_ASSERT(buffers[timingsIndex]);

            f64 start = DqnTimer_NowInMs();
            DqnMem_Set(buffers[timingsIndex], value, size);
            f64 duration = DqnTimer_NowInMs() - start;

            timings[timingsIndex++][i] = duration;
            Log("DqnMem_Set: %5.3f vs ", duration);
        }


        // crt memset
        {
            buffers[timingsIndex] = malloc(size); DQN_ASSERT(buffers[timingsIndex]);

            f64 start = DqnTimer_NowInMs();
            memset(buffers[timingsIndex], value, size);
            f64 duration = DqnTimer_NowInMs() - start;

            timings[timingsIndex++][i] = duration;
            Log("memset: %5.3f\n", duration);
        }
        global_indent++;
        global_new_line = true;

        for (auto test_index = 0; test_index < size; test_index++)
        {
            DQN_ASSERT(((u8 *)buffers[0])[test_index] == ((u8 *)buffers[1])[test_index]);
        }

        for (usize bufferIndex = 0; bufferIndex < DQN_ARRAY_COUNT(buffers); bufferIndex++)
        {
            free(buffers[bufferIndex]);
        }
    }

    for (usize timingsIndex = 0; timingsIndex < DQN_ARRAY_COUNT(timings); timingsIndex++)
    {
        f64 total_time = 0;
        for (auto iteration_index = 0; iteration_index < NUM_ITERATIONS; iteration_index++)
        {
            total_time += timings[timingsIndex][iteration_index];
        }
        avg_timings[timingsIndex] = total_time / (f64)NUM_ITERATIONS;
    }

    Log("Average Timings");
    global_indent++;
    Log("DqnMem_Set: %f vs memset: %f\n", avg_timings[0], avg_timings[1]);
    global_indent--;

    Log(Status::Ok, "MemSet");
}

struct RawBuf
{
    char *buffer;
    int   len;
};

DQN_CATALOG_LOAD_PROC(CatalogRawLoad, RawBuf)
{
    usize buf_size;
    u8 *buf = DqnFile_ReadAll(file.str, &buf_size);

    if (!buf)
        return false;

    data->buffer = reinterpret_cast<char *>(buf);
    data->len    = static_cast<int>(buf_size);
    return true;
}

void DqnCatalog_Test()
{
    LOG_HEADER();

    DqnCatalog<RawBuf, CatalogRawLoad> catalog = {};
    catalog.PollAssets();

    DqnCatalogPath path = "DqnCatalog_TrackFile";
    DqnFile_Delete(path.str);

    // Initially write the file and check the catalog is able to open it up
    {
        char const write_buf[] = "aaaa";
        DqnFile_WriteAll(path.str, reinterpret_cast<u8 const *>(write_buf), DQN_CHAR_COUNT(write_buf));
        RawBuf *buf = catalog.GetIfUpdated(path);
        DQN_ASSERT(DqnMem_Cmp(buf->buffer, write_buf, DQN_CHAR_COUNT(write_buf)) == 0);
        Log(Status::Ok, "Catalog finds and loads on demand new file");
    }

    // Update the file and check that the GetIfUpdated returns a non-nullptr (because the entry is updated)
    {
        PlatformSleep(1000);
        char const write_buf[] = "xxxx";
        DqnFile_WriteAll(path.str, reinterpret_cast<u8 const *>(write_buf), DQN_CHAR_COUNT(write_buf));
        RawBuf *buf = catalog.GetIfUpdated(path);
        DQN_ASSERT(DqnMem_Cmp(buf->buffer, write_buf, DQN_CHAR_COUNT(write_buf)) == 0);
        Log(Status::Ok, "Catalog finds updated file after subsequent write");
    }

    // Update the file and get the catalog to poll the entries and check it has been updated
    {
        PlatformSleep(1000);
        char const write_buf[] = "abcd";
        DqnFile_WriteAll(path.str, reinterpret_cast<u8 const *>(write_buf), DQN_CHAR_COUNT(write_buf));
        catalog.PollAssets();

        RawBuf *buf = catalog.GetIfUpdated(path);
        DQN_ASSERT(DqnMem_Cmp(buf->buffer, write_buf, DQN_CHAR_COUNT(write_buf)) == 0);
        Log(Status::Ok, "Catalog finds updated file using the poll asset interface");
    }

    // Update the file and get the catalog to poll the entries and check it has been updated
    {
        catalog.Erase(path.str);
        RawBuf *buf = catalog.Get(path);
        DQN_ASSERT(buf == nullptr);
        Log(Status::Ok, "Catalog erase removes file from catalog");
    }

    DqnFile_Delete(path.str);
}

int main(void)
{
    global_indent  = 1;
    global_new_line = true;
    DqnString_Test();
    DqnMemStack_Test();
    DqnChar_Test();
    DqnRnd_Test();
    DqnMath_Test();
    DqnVX_Test();
    DqnRect_Test();
    DqnArray_Test();
    DqnQuickSort_Test();
    DqnBSearch_Test();
    DqnMemSet_Test();
    DqnFixedString_Test();
    DqnJson_Test();

#ifdef DQN_PLATFORM_HEADER
    DqnVHashTable_Test();
    DqnOS_Test();
    DqnFile_Test();
    DqnCatalog_Test();
    DqnTimer_Test();
    DqnJobQueue_Test();
#endif

    // Log("\nPress 'Enter' Key to Exit\n");
    // getchar();

    return 0;
}

#if defined(__GNUC__)
    #pragma GCC diagnostic pop
#endif

