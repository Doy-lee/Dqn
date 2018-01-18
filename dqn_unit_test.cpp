#if (defined(_WIN32) || defined(_WIN64))
	#define DQN_WIN32_IMPLEMENTATION
	#include <Windows.h>
#endif

#if defined(__linux__)
	#define DQN_UNIX_IMPLEMENTATION
	#define HANDMADE_MATH_NO_SSE
#endif

#if defined(__GNUC__)
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wfree-nonheap-object"
#endif

#define DQN_PLATFORM_HEADER
#define DQN_IMPLEMENTATION
#include "dqn.h"

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_CPP_MODE
#include "tests/HandmadeMath.h"

#include <limits.h>
#include <stdio.h>

FILE_SCOPE void LogHeader(const char *const header)
{
	DQN_ASSERT_HARD(header);

	char buf[1024] = {};
	DQN_ASSERT(Dqn_sprintf(buf, "// %s", header) < (i32)DQN_ARRAY_COUNT(buf));
	printf("\n//////////////////////////////////////////////////////////////////\n");
	printf("%s\n", buf);
	printf("//////////////////////////////////////////////////////////////////\n");
}

FILE_SCOPE void LogSuccess(const char *const functionName)
{
	DQN_ASSERT_HARD(functionName);

	char buf[1024] = {};
	DQN_ASSERT(Dqn_sprintf(buf, "%s", functionName) < (i32)DQN_ARRAY_COUNT(buf));
	printf("%s: Completed successfully\n", buf);
}

void HandmadeMathVerifyMat4(DqnMat4 dqnMat, hmm_mat4 hmmMat)
{
	f32 *hmmMatf = (f32 *)&hmmMat;
	f32 *dqnMatf = (f32 *)&dqnMat;

	const u32 EXPECTED_SIZE = 16;
	u32 totalSize           = DQN_ARRAY_COUNT(dqnMat.e) * DQN_ARRAY_COUNT(dqnMat.e[0]);
	DQN_ASSERT(totalSize == EXPECTED_SIZE);
	DQN_ASSERT(totalSize ==
	           (DQN_ARRAY_COUNT(hmmMat.Elements) * DQN_ARRAY_COUNT(hmmMat.Elements[0])));

	for (u32 i = 0; i < EXPECTED_SIZE; i++)
	{
		const f32 EPSILON = 0.001f;
		f32 diff          = hmmMatf[i] - dqnMatf[i];
		diff              = DQN_ABS(diff);
		DQN_ASSERT_MSG(diff < EPSILON, "hmmMatf[%d]: %f, dqnMatf[%d]: %f\n", i, hmmMatf[i], i,
		               dqnMatf[i]);
	}
}

void HandmadeMathTestInternal()
{
	LogHeader("DqnMath vs HandmadeMath Test");
	// Test Perspective/Projection matrix values
	if (1)
	{
		f32 aspectRatio         = 1;
		DqnMat4 dqnPerspective  = DqnMat4_Perspective(90, aspectRatio, 100, 1000);
		hmm_mat4 hmmPerspective = HMM_Perspective(90, aspectRatio, 100, 1000);
		HandmadeMathVerifyMat4(dqnPerspective, hmmPerspective);

		LogSuccess("HandmadeMathTest(): Perspective");
	}

	// Test Mat4 translate * scale
	if (1)
	{
		hmm_vec3 hmmVec       = HMM_Vec3i(1, 2, 3);
		DqnV3 dqnVec          = DqnV3_(1, 2, 3);
		DqnMat4 dqnTranslate  = DqnMat4_Translate3f(dqnVec.x, dqnVec.y, dqnVec.z);
		hmm_mat4 hmmTranslate = HMM_Translate(hmmVec);
		HandmadeMathVerifyMat4(dqnTranslate, hmmTranslate);

		hmm_vec3 hmmAxis      = HMM_Vec3(0.5f, 0.2f, 0.7f);
		DqnV3 dqnAxis         = DqnV3_(0.5f, 0.2f, 0.7f);
		f32 rotationInDegrees = 80.0f;

		DqnMat4 dqnRotate = DqnMat4_Rotate(DQN_DEGREES_TO_RADIANS(rotationInDegrees), dqnAxis.x,
		                                   dqnAxis.y, dqnAxis.z);
		hmm_mat4 hmmRotate = HMM_Rotate(rotationInDegrees, hmmAxis);
		HandmadeMathVerifyMat4(dqnRotate, hmmRotate);

		dqnVec *= 2;
		hmmVec *= 2;
		DqnMat4 dqnScale  = DqnMat4_Scale(dqnVec.x, dqnVec.y, dqnVec.z);
		hmm_mat4 hmmScale = HMM_Scale(hmmVec);
		HandmadeMathVerifyMat4(dqnScale, hmmScale);

		DqnMat4 dqnTSMatrix  = DqnMat4_Mul(dqnTranslate, dqnScale);
		hmm_mat4 hmmTSMatrix = HMM_MultiplyMat4(hmmTranslate, hmmScale);
		HandmadeMathVerifyMat4(dqnTSMatrix, hmmTSMatrix);

		// Test Mat4 * MulV4
		if (1)
		{
			DqnV4 dqnV4    = DqnV4_(1, 2, 3, 4);
			hmm_vec4 hmmV4 = HMM_Vec4(1, 2, 3, 4);

			DqnV4 dqnResult    = DqnMat4_MulV4(dqnTSMatrix, dqnV4);
			hmm_vec4 hmmResult = HMM_MultiplyMat4ByVec4(hmmTSMatrix, hmmV4);

			DQN_ASSERT(dqnResult.x == hmmResult.X);
			DQN_ASSERT(dqnResult.y == hmmResult.Y);
			DQN_ASSERT(dqnResult.z == hmmResult.Z);
			DQN_ASSERT(dqnResult.w == hmmResult.W);

			LogSuccess("HandmadeMathTest(): Mat4 * MulV4");
		}

		LogSuccess("HandmadeMathTest(): Translate/Scale/Rotate Mat4_Mul");
	}
}

void Dqn_Test()
{
	LogHeader("Dqn_Test");

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

		LogSuccess("Dqn_StrToI64()");
	}

	// i64 to str
	if (1)
	{
		char a[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
		Dqn_I64ToStr(+100, a, DQN_ARRAY_COUNT(a));
		DQN_ASSERT(DqnStr_Cmp(a, "100") == 0);

		char b[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
		Dqn_I64ToStr(-100, b, DQN_ARRAY_COUNT(b));
		DQN_ASSERT(DqnStr_Cmp(b, "-100") == 0);

		char c[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
		Dqn_I64ToStr(0, c, DQN_ARRAY_COUNT(c));
		DQN_ASSERT(DqnStr_Cmp(c, "0") == 0);

#if 0
			char d[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
			Dqn_I64ToStr(LARGEST_NUM, d, DQN_ARRAY_COUNT(d));
			DQN_ASSERT(DqnStr_Cmp(d, "18446744073709551615") == 0);
#endif

		if (sizeof(size_t) == sizeof(u64))
		{
			char e[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
			Dqn_I64ToStr(SMALLEST_NUM, e, DQN_ARRAY_COUNT(e));
			DQN_ASSERT_MSG(DqnStr_Cmp(e, "-9223372036854775808") == 0, "e: %s", e);
		}

		LogSuccess("Dqn_I64ToStr()");
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

		LogSuccess("Dqn_StrToF32()");
	}

	// UCS <-> UTF8 Checks
	if (1)
	{
		// Test ascii characters
		if (1)
		{
			u32 codepoint = '@';
			u32 string[1] = {};

			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
			DQN_ASSERT(bytesUsed == 1);
			DQN_ASSERT(string[0] == '@');

			bytesUsed = Dqn_UTF8ToUCS(&string[0], codepoint);
			DQN_ASSERT(string[0] >= 0 && string[0] < 0x80);
			DQN_ASSERT(bytesUsed == 1);

			LogSuccess("Dqn_UTF8ToUCS(): Test ascii characters");
		}

		// Test 2 byte characters
		if (1)
		{
			u32 codepoint = 0x278;
			u32 string[1] = {};

			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
			DQN_ASSERT(bytesUsed == 2);
			DQN_ASSERT(string[0] == 0xC9B8);

			bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
			DQN_ASSERT(string[0] == codepoint);
			DQN_ASSERT(bytesUsed == 2);

			LogSuccess("Dqn_UTF8ToUCS(): Test 2 byte characters");
		}

		// Test 3 byte characters
		if (1)
		{
			u32 codepoint = 0x0A0A;
			u32 string[1] = {};

			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
			DQN_ASSERT(bytesUsed == 3);
			DQN_ASSERT(string[0] == 0xE0A88A);

			bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
			DQN_ASSERT(string[0] == codepoint);
			DQN_ASSERT(bytesUsed == 3);

			LogSuccess("Dqn_UTF8ToUCS(): Test 3 byte characters");
		}

		// Test 4 byte characters
		if (1)
		{
			u32 codepoint = 0x10912;
			u32 string[1] = {};
			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);

			DQN_ASSERT(bytesUsed == 4);
			DQN_ASSERT(string[0] == 0xF090A492);

			bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
			DQN_ASSERT(string[0] == codepoint);
			DQN_ASSERT(bytesUsed == 4);

			LogSuccess("Dqn_UTF8ToUCS(): Test 4 byte characters");
		}

		if (1)
		{
			u32 codepoint = 0x10912;
			u32 bytesUsed = Dqn_UCSToUTF8(NULL, codepoint);
			DQN_ASSERT(bytesUsed == 0);

			bytesUsed = Dqn_UTF8ToUCS(NULL, codepoint);
			DQN_ASSERT(bytesUsed == 0);

			LogSuccess("Dqn_UTF8ToUCS(): Test return result on on NULL output param");
		}
	}

}

void DqnStr_Test()
{
	// String Checks
	if (1)
	{
		LogHeader("DqnStr_Test");

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
				LogSuccess("DqnStr_Cmp(): Check simple compares");
			}

			// Check ops against null
			if (1)
			{
				DQN_ASSERT(DqnStr_Cmp(NULL, NULL) != +0);
				DQN_ASSERT(DqnStr_Cmp(a, NULL) != +0);
				DQN_ASSERT(DqnStr_Cmp(NULL, a) != +0);
				LogSuccess("DqnStr_Cmp(): Check ops against null");
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

			LogSuccess("DqnStr_Len()");
		}

		// strncpy
		if (1)
		{
			if (1)
			{
				const char *const a = "str_a";
				char b[10]          = {};
				// Check copy into empty array
				if (1)
				{
					char *result = DqnStr_Copy(b, a, DqnStr_Len(a));
					DQN_ASSERT(DqnStr_Cmp(b, "str_a") == 0);
					DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);
					DQN_ASSERT(DqnStr_Cmp(result, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(result) == 5);
					LogSuccess("DqnStr_Copy(): Check copy into empty array");
				}

				// Check copy into array offset, overlap with old results
				if (1)
				{
					char *newResult = DqnStr_Copy(&b[1], a, DqnStr_Len(a));
					DQN_ASSERT(DqnStr_Cmp(newResult, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(newResult) == 5);

					DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(a) == 5);

					DQN_ASSERT(DqnStr_Cmp(b, "sstr_a") == 0);
					DQN_ASSERT(DqnStr_Len(b) == 6);
					LogSuccess("DqnStr_Copy(): Check copy into array offset, overlap with old results");
				}
			}
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
				LogSuccess("DqnStr_Reverse(): Basic reverse operations");
			}

			// Try reverse empty string
			if (1)
			{
				char a[] = "";
				DqnStr_Reverse(a, DqnStr_Len(a));
				DQN_ASSERT(DqnStr_Cmp(a, "") == 0);
				LogSuccess("DqnStr_Reverse(): Reverse empty string");
			}

			// Try reverse single char string
			if (1)
			{
				char a[] = "a";
				DqnStr_Reverse(a, DqnStr_Len(a));
				DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);

				DqnStr_Reverse(a, 0);
				DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);
				LogSuccess("DqnStr_Reverse(): Reverse single char string");
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
			LogSuccess("DqnStr_HasSubstring(): Check string with matching substring");
		}

		if (1)
		{
			const char *const a = "Micro";
			const char *const b = "irob";
			i32 lenA            = DqnStr_Len(a);
			i32 lenB            = DqnStr_Len(b);
			DQN_ASSERT(DqnStr_HasSubstring(a, lenA, b, lenB) == false);
			DQN_ASSERT(DqnStr_HasSubstring(b, lenB, a, lenA) == false);
			LogSuccess("DqnStr_HasSubstring(): Check string with non-matching substring");
		}

	}
}

void DqnChar_Test()
{
	LogHeader("DqnChar_Test");

	// Char Checks
	if (1)
	{
		DQN_ASSERT(DqnChar_IsAlpha('a') == true);
		DQN_ASSERT(DqnChar_IsAlpha('A') == true);
		DQN_ASSERT(DqnChar_IsAlpha('0') == false);
		DQN_ASSERT(DqnChar_IsAlpha('@') == false);
		DQN_ASSERT(DqnChar_IsAlpha(' ') == false);
		DQN_ASSERT(DqnChar_IsAlpha('\n') == false);
		LogSuccess("DqnChar_IsAlpha()");

		DQN_ASSERT(DqnChar_IsDigit('1') == true);
		DQN_ASSERT(DqnChar_IsDigit('n') == false);
		DQN_ASSERT(DqnChar_IsDigit('N') == false);
		DQN_ASSERT(DqnChar_IsDigit('*') == false);
		DQN_ASSERT(DqnChar_IsDigit(' ') == false);
		DQN_ASSERT(DqnChar_IsDigit('\n') == false);
		LogSuccess("DqnChar_IsDigit()");

		DQN_ASSERT(DqnChar_IsAlphaNum('1') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('a') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('A') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('*') == false);
		DQN_ASSERT(DqnChar_IsAlphaNum(' ') == false);
		DQN_ASSERT(DqnChar_IsAlphaNum('\n') == false);
		LogSuccess("DqnChar_IsAlphaNum()");

		DQN_ASSERT(DqnChar_ToLower(L'A') == L'a');
		DQN_ASSERT(DqnChar_ToLower(L'a') == L'a');
		DQN_ASSERT(DqnChar_ToLower(L' ') == L' ');
		LogSuccess("DqnChar_ToLower()");

		DQN_ASSERT(DqnChar_ToUpper(L'A') == L'A');
		DQN_ASSERT(DqnChar_ToUpper(L'a') == L'A');
		DQN_ASSERT(DqnChar_ToUpper(L' ') == L' ');
		LogSuccess("DqnChar_ToUpper()");
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

		LogSuccess("DqnChar_TrimAroundWhitespace()");
	}
}

void DqnString_Test()
{
	LogHeader("DqnString");
	// Check fixed mem string doesn't allow string to expand and fail if try to append
	if (1)
	{
		char space[4] = {};
		DqnString str = {};
		DQN_ASSERT(str.InitFixedMem(space, DQN_ARRAY_COUNT(space)));

		DQN_ASSERT(str.Append("test_doesnt_fit") == false);
		DQN_ASSERT(str.Append("tooo") == false);
		DQN_ASSERT(str.Append("fit") == true);
		DQN_ASSERT(str.Append("test_doesnt_fit") == false);
		DQN_ASSERT(str.Append("1") == false);

		DQN_ASSERT(str.str[str.len] == 0);
		DQN_ASSERT(str.len <= str.max);
		LogSuccess("DqnString->Append(): Check fixed mem string doesn't expand and fails.");
	}

	// Try expanding string
	if (1)
	{
		DqnString str = {};
		DQN_ASSERT(str.InitLiteral("hello world"));
		DQN_ASSERT(str.Append(", hello again"));
		DQN_ASSERT(str.Append(", and hello again"));

		DQN_ASSERT(str.str[str.len] == 0);
		DQN_ASSERT(str.len <= str.max);

		str.Free();
		LogSuccess("DqnString(): Check storage expansion on append");
	}

	// Try init literal no alloc
	if (1)
	{
		char *literal = "this is a literal string";
		DqnString str = {};
		DQN_ASSERT(str.InitLiteralNoAlloc(literal));
		DQN_ASSERT(str.Append(", hello again") == false);
		str.Free();
		LogSuccess("DqnString(): Try init literl no alloc, no further expansion");
	}
}

void DqnTimer_Test()
{
	LogHeader("DqnTimer");

	if (1)
	{
#if defined(DQN_UNIX_PLATFORM)
		f64 startInS     = DqnTimer_NowInS();
		u32 sleepTimeInS = 1;
		sleep(sleepTimeInS);
		f64 endInS = DqnTimer_NowInS();
		printf("start: %f, end: %f\n", startInS, endInS);
		DQN_ASSERT((startInS + sleepTimeInS) <= endInS);

#elif defined(DQN_WIN32_PLATFORM)
		f64 startInMs     = DqnTimer_NowInMs();
		u32 sleepTimeInMs = 1000;
		Sleep(sleepTimeInMs);
		f64 endInMs = DqnTimer_NowInMs();

		printf("start: %f, end: %f\n", startInMs, endInMs);
		DQN_ASSERT((startInMs + sleepTimeInMs) <= endInMs);
#endif
		LogSuccess("DqnTimer(): Timer advanced in time over 1second");
	}
}

void DqnRnd_Test()
{
	LogHeader("Random Number Generator Test");

	DqnRndPCG pcg; pcg.Init();
	for (i32 i = 0; i < 1000000; i++)
	{
		i32 min    = -100;
		i32 max    = 1000000000;
		i32 result = pcg.Range(min, max);
		DQN_ASSERT(result >= min && result <= max);

		f32 randF32 = pcg.Nextf();
		DQN_ASSERT(randF32 >= 0.0f && randF32 <= 1.0f);
	}

	printf("RandomTest(): RndPCG: Completed successfully\n");
	printf("RandomTest(): Completed successfully\n");
}

void DqnMath_Test()
{
	LogHeader("DqnMath");

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

		LogSuccess("DqnMath_Lerp()");
	}

	 // sqrtf
	if (1)
	{
		DQN_ASSERT(DqnMath_Sqrtf(4.0f) == 2.0f);
		LogSuccess("DqnMath_Sqrt()");
	}

	HandmadeMathTestInternal();
}

void DqnVX_Test()
{
	LogHeader("Math Vector Test");
	if (1)
	{ // V2

		// V2 Creating
		if (1)
		{
			DqnV2 vec = DqnV2_(5.5f, 5.0f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 5.5f && vec.h == 5.0f);
			LogSuccess("DqnV2_(): Creating");
		}

		// V2 with 2 integers
		if (1)
		{
			DqnV2 vec = DqnV2_(3, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 3 && vec.h == 5.0f);
			LogSuccess("DqnV2_(): with 2 integers");
		}

		// V2 Arithmetic
		if (1)
		{
			DqnV2 vecA = DqnV2_(5, 10);
			DqnV2 vecB = DqnV2_(2, 3);
			DQN_ASSERT(DqnV2_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV2_Equals(vecA, DqnV2_(5, 10)) == true);
			DQN_ASSERT(DqnV2_Equals(vecB, DqnV2_(2, 3)) == true);

			DqnV2 result = DqnV2_Add(vecA,  DqnV2_(5, 10));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_(10, 20)) == true);

			result = DqnV2_Sub(result, DqnV2_(5, 10));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_(5, 10)) == true);

			result = DqnV2_Scalef(result, 5);
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_(25, 50)) == true);

			result = DqnV2_Hadamard(result, DqnV2_(10.0f, 0.5f));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_(250, 25)) == true);

			f32 dotResult = DqnV2_Dot(DqnV2_(5, 10), DqnV2_(3, 4));
			DQN_ASSERT(dotResult == 55);
			LogSuccess("DqnV2_(): Arithmetic");
		}

		// Test operator overloading
		if (1)
		{
			DqnV2 vecA = DqnV2_(5, 10);
			DqnV2 vecB = DqnV2_(2, 3);
			DQN_ASSERT((vecA == vecB) == false);
			DQN_ASSERT((vecA == DqnV2_(5, 10)) == true);
			DQN_ASSERT((vecB == DqnV2_(2, 3)) == true);

			DqnV2 result = vecA + DqnV2_(5, 10);
			DQN_ASSERT((result == DqnV2_(10, 20)) == true);

			result -= DqnV2_(5, 10);
			DQN_ASSERT((result == DqnV2_(5, 10)) == true);

			result *= 5;
			DQN_ASSERT((result == DqnV2_(25, 50)) == true);

			result = result * DqnV2_(10.0f, 0.5f);
			DQN_ASSERT((result == DqnV2_(250, 25)) == true);

			result += DqnV2_(1, 1);
			DQN_ASSERT((result == DqnV2_(251, 26)) == true);

			result = result - DqnV2_(1, 1);
			DQN_ASSERT((result == DqnV2_(250, 25)) == true);
			LogSuccess("DqnV2_(): operator overloading");
		}

		// V2 Properties
		if (1)
		{
			const f32 EPSILON = 0.001f;
			DqnV2 a           = DqnV2_(0, 0);
			DqnV2 b           = DqnV2_(3, 4);

			f32 lengthSq = DqnV2_LengthSquared(a, b);
			DQN_ASSERT(lengthSq == 25);

			f32 length = DqnV2_Length(a, b);
			DQN_ASSERT(length == 5);

			DqnV2 normalised = DqnV2_Normalise(b);
			f32 normX        = b.x / 5.0f;
			f32 normY        = b.y / 5.0f;
			f32 diffNormX    = normalised.x - normX;
			f32 diffNormY    = normalised.y - normY;
			DQN_ASSERT_MSG(diffNormX < EPSILON, "normalised.x: %f, normX: %f\n", normalised.x,
			               normX);
			DQN_ASSERT_MSG(diffNormY < EPSILON, "normalised.y: %f, normY: %f\n", normalised.y,
			               normY);

			DqnV2 c = DqnV2_(3.5f, 8.0f);
			DQN_ASSERT(DqnV2_Overlaps(b, c) == true);
			DQN_ASSERT(DqnV2_Overlaps(b, a) == false);

			DqnV2 d = DqnV2_Perpendicular(c);
			DQN_ASSERT(DqnV2_Dot(c, d) == 0);

			LogSuccess("DqnV2_(): LengthSquared, Length, Normalize, Overlaps, Perp");
		}

		// V2 ConstrainToRatio
		if (1)
		{
			DqnV2 ratio  = DqnV2_(16, 9);
			DqnV2 dim    = DqnV2_(2000, 1080);
			DqnV2 result = DqnV2_ConstrainToRatio(dim, ratio);
			DQN_ASSERT(result.w == 1920 && result.h == 1080);
			LogSuccess("DqnV2->ConstrainToRatio()");
		}
	}

	// V3
	if (1)
	{
		// V3i Creating
		if (1)
		{
			DqnV3 vec = DqnV3_(5.5f, 5.0f, 5.875f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f);
			LogSuccess("DqnV3_(): Creating");
		}

		// V3i Creating
		if (1)
		{
			DqnV3 vec = DqnV3_(3, 4, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5);
			DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5);
			LogSuccess("DqnV3_(): Creating");
		}

		// V3 Arithmetic
		if (1)
		{
			DqnV3 vecA = DqnV3_(5, 10, 15);
			DqnV3 vecB = DqnV3_(2, 3, 6);
			DQN_ASSERT(DqnV3_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV3_Equals(vecA, DqnV3_(5, 10, 15)) == true);
			DQN_ASSERT(DqnV3_Equals(vecB, DqnV3_(2, 3, 6)) == true);

			DqnV3 result = DqnV3_Add(vecA, DqnV3_(5, 10, 15));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_(10, 20, 30)) == true);

			result = DqnV3_Sub(result, DqnV3_(5, 10, 15));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_(5, 10, 15)) == true);

			result = DqnV3_Scalef(result, 5);
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_(25, 50, 75)) == true);

			result = DqnV3_Hadamard(result, DqnV3_(10.0f, 0.5f, 10.0f));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_(250, 25, 750)) == true);

			f32 dotResult = DqnV3_Dot(DqnV3_(5, 10, 2), DqnV3_(3, 4, 6));
			DQN_ASSERT(dotResult == 67);

			DqnV3 cross = DqnV3_Cross(vecA, vecB);
			DQN_ASSERT(DqnV3_Equals(cross, DqnV3_(15, 0, -5)) == true);
			LogSuccess("DqnV3_(): Arithmetic");
		}

		// V3 More Arithmetic
		if (1)
		{
			DqnV3 vecA = DqnV3_(5, 10, 15);
			DqnV3 vecB = DqnV3_(2, 3, 6);
			DQN_ASSERT((vecA == vecB) == false);
			DQN_ASSERT((vecA == DqnV3_(5, 10, 15)) == true);
			DQN_ASSERT((vecB == DqnV3_(2, 3, 6)) == true);

			DqnV3 result = vecA + DqnV3_(5, 10, 15);
			DQN_ASSERT((result == DqnV3_(10, 20, 30)) == true);

			result -= DqnV3_(5, 10, 15);
			DQN_ASSERT((result == DqnV3_(5, 10, 15)) == true);

			result = result * 5;
			DQN_ASSERT((result == DqnV3_(25, 50, 75)) == true);

			result *= DqnV3_(10.0f, 0.5f, 10.0f);
			DQN_ASSERT((result == DqnV3_(250, 25, 750)) == true);

			result = result - DqnV3_(1, 1, 1);
			DQN_ASSERT((result == DqnV3_(249, 24, 749)) == true);

			result += DqnV3_(1, 1, 1);
			DQN_ASSERT((result == DqnV3_(250, 25, 750)) == true);
			LogSuccess("DqnV3_(): More Arithmetic");
		}
	}

	 // V4
	if (1)
	{

		// V4 Creating
		if (1)
		{
			DqnV4 vec = DqnV4_(5.5f, 5.0f, 5.875f, 5.928f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f && vec.w == 5.928f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f && vec.a == 5.928f);
			LogSuccess("DqnV4_(): Creating");
		}

		// V4i Creating
		if (1)
		{
			DqnV4 vec = DqnV4_(3, 4, 5, 6);
			DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5 && vec.w == 6);
			DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5 && vec.a == 6);
			LogSuccess("DqnV4_(): Integer ctor creating");
		}

		// V4 Arithmetic
		if (1)
		{
			DqnV4 vecA = DqnV4_(5, 10, 15, 20);
			DqnV4 vecB = DqnV4_(2, 3, 6, 8);
			DQN_ASSERT(DqnV4_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV4_Equals(vecA, DqnV4_(5, 10, 15, 20)) == true);
			DQN_ASSERT(DqnV4_Equals(vecB, DqnV4_(2, 3, 6, 8)) == true);

			DqnV4 result = DqnV4_Add(vecA, DqnV4_(5, 10, 15, 20));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_(10, 20, 30, 40)) == true);

			result = DqnV4_Sub(result, DqnV4_(5, 10, 15, 20));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_(5, 10, 15, 20)) == true);

			result = DqnV4_Scalef(result, 5);
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_(25, 50, 75, 100)) == true);

			result = DqnV4_Hadamard(result, DqnV4_(10.0f, 0.5f, 10.0f, 0.25f));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_(250, 25, 750, 25)) == true);

			f32 dotResult = DqnV4_Dot(DqnV4_(5, 10, 2, 8), DqnV4_(3, 4, 6, 5));
			DQN_ASSERT(dotResult == 107);
			LogSuccess("DqnV4_(): Arithmetic");
		}

		// V4 More Arthmetic
		if (1)
		{
			DqnV4 vecA = DqnV4_(5, 10, 15, 20);
			DqnV4 vecB = DqnV4_(2, 3, 6, 8);
			DQN_ASSERT((vecA == vecB) == false);
			DQN_ASSERT((vecA == DqnV4_(5, 10, 15, 20)) == true);
			DQN_ASSERT((vecB == DqnV4_(2, 3, 6, 8)) == true);

			DqnV4 result = vecA + DqnV4_(5, 10, 15, 20);
			DQN_ASSERT((result == DqnV4_(10, 20, 30, 40)) == true);

			result = result - DqnV4_(5, 10, 15, 20);
			DQN_ASSERT((result == DqnV4_(5, 10, 15, 20)) == true);

			result = result * 5;
			DQN_ASSERT((result == DqnV4_(25, 50, 75, 100)) == true);

			result *= DqnV4_(10.0f, 0.5f, 10.0f, 0.25f);
			DQN_ASSERT((result == DqnV4_(250, 25, 750, 25)) == true);

			result += DqnV4_(1, 1, 1, 1);
			DQN_ASSERT((result == DqnV4_(251, 26, 751, 26)) == true);

			result -= DqnV4_(1, 1, 1, 1);
			DQN_ASSERT((result == DqnV4_(250, 25, 750, 25)) == true);
			LogSuccess("DqnV4_(): More Arthmetic");
		}
	}
}

void DqnRect_Test()
{
	// Rect
	if (1)
	{
		// Test rect init functions
		if (1)
		{
			DqnRect rect4f = DqnRect_(1.1f, 2.2f, 3.3f, 4.4f);
			DqnRect rect4i = DqnRect_(1, 2, 3, 4);

			DQN_ASSERT(rect4i.min.x == 1 && rect4i.min.y == 2);
			DQN_ASSERT(rect4i.max.x == 4 && rect4i.max.y == 6);

			const f32 EPSILON = 0.001f;
			f32 diffMaxX      = rect4f.max.x - 4.4f;
			f32 diffMaxY      = rect4f.max.y - 6.6f;
			DQN_ASSERT(rect4f.min.x == 1.1f && rect4f.min.y == 2.2f);
			DQN_ASSERT(DQN_ABS(diffMaxX) < EPSILON && DQN_ABS(diffMaxY) < EPSILON);

			DqnRect rect = DqnRect_(-10, -10, 20, 20);
			DQN_ASSERT(DqnV2_Equals(rect.min, DqnV2_(-10, -10)));
			DQN_ASSERT(DqnV2_Equals(rect.max, DqnV2_(10, 10)));
			LogSuccess("DqnRect_(): Test rect init functions");
		}

		// Test rect get size function
		if (1)
		{
			// Test float rect
			if (1)
			{
				DqnRect rect = DqnRect_(DqnV2_(-10, -10), DqnV2_(20, 20));

				f32 width, height;
				rect.GetSize(&width, &height);
				DQN_ASSERT(width == 20);
				DQN_ASSERT(height == 20);

				DqnV2 dim = rect.GetSize();
				DQN_ASSERT(DqnV2_Equals(dim, DqnV2_(20, 20)));
				LogSuccess("DqnRect->GetSize(): Test float rect");
			}
		}

		// Test rect get centre
		DqnRect rect     = DqnRect_(DqnV2_(-10, -10), DqnV2_(20, 20));
		DqnV2 rectCenter = rect.GetCenter();
		DQN_ASSERT(DqnV2_Equals(rectCenter, DqnV2_(0, 0)));
		LogSuccess("DqnRect->GetCentre()");

		// Test clipping rect get centre
		DqnRect clipRect   = DqnRect_(DqnV2_(-15, -15), DqnV2_(10, 10) + DqnV2_(15));
		DqnRect clipResult = rect.ClipRect(clipRect);
		DQN_ASSERT(clipResult.min.x == -10 && clipResult.min.y == -10);
		DQN_ASSERT(clipResult.max.x == 10 && clipResult.max.y == 10);
		LogSuccess("DqnRect->ClipRect()");

		// Test shifting rect
		if (1)
		{
			DqnRect shiftedRect = rect.Move(DqnV2_(10, 0));
			DQN_ASSERT(DqnV2_Equals(shiftedRect.min, DqnV2_(0, -10)));
			DQN_ASSERT(DqnV2_Equals(shiftedRect.max, DqnV2_(20, 10)));

			// Ensure dimensions have remained the same
			if (1)
			{
				f32 width, height;
				shiftedRect.GetSize(&width, &height);
				DQN_ASSERT(width == 20);
				DQN_ASSERT(height == 20);

				DqnV2 dim = shiftedRect.GetSize();
				DQN_ASSERT(DqnV2_Equals(dim, DqnV2_(20, 20)));
			}

			// Test rect contains p
			if (1)
			{
				DqnV2 inP  = DqnV2_(5, 5);
				DqnV2 outP = DqnV2_(100, 100);
				DQN_ASSERT(shiftedRect.ContainsP(inP));
				DQN_ASSERT(!shiftedRect.ContainsP(outP));
			}

			LogSuccess("DqnRect->Move()");
		}
	}
}

void DqnArray_TestInternal(const DqnMemAPI memAPI)
{
	if (1)
	{
		DqnArray<DqnV2> array = {};
		if (1)
		{
			DQN_ASSERT(array.Init(1, memAPI));
			DQN_ASSERT(array.max >= 1);
			DQN_ASSERT(array.count == 0);

			// Test basic insert
			if (1)
			{
				DqnV2 va = DqnV2_(5, 10);
				DQN_ASSERT(array.Push(va));

				DqnV2 vb = array.data[0];
				DQN_ASSERT(DqnV2_Equals(va, vb));

				DQN_ASSERT(array.max >= 1);
				DQN_ASSERT(array.count == 1);
				LogSuccess("DqnArray(): Test basic insert");
			}

			// Test array resizing and freeing
			if (1)
			{
				DqnV2 va = DqnV2_(10, 15);
				DQN_ASSERT(array.Push(va));

				DqnV2 vb = array.data[0];
				DQN_ASSERT(DqnV2_Equals(va, vb) == false);

				vb = array.data[1];
				DQN_ASSERT(DqnV2_Equals(va, vb) == true);

				DQN_ASSERT(array.max >= 2);
				DQN_ASSERT(array.count == 2);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 3);
				DQN_ASSERT(array.count == 3);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 4);
				DQN_ASSERT(array.count == 4);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 5);
				DQN_ASSERT(array.count == 5);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 6);
				DQN_ASSERT(array.count == 6);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 7);
				DQN_ASSERT(array.count == 7);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 8);
				DQN_ASSERT(array.count == 8);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 9);
				DQN_ASSERT(array.count == 9);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 10);
				DQN_ASSERT(array.count == 10);

				DQN_ASSERT(array.Push(va));
				DQN_ASSERT(array.max >= 11);
				DQN_ASSERT(array.count == 11);

				DqnV2 vc = DqnV2_(90, 100);
				DQN_ASSERT(array.Push(vc));
				DQN_ASSERT(array.max >= 12);
				DQN_ASSERT(array.count == 12);
				DQN_ASSERT(DqnV2_Equals(vc, array.data[11]));

				LogSuccess("DqnArray(): Test resizing and free");
			}
		}
		DQN_ASSERT(array.Free());

		if (1)
		{
			DQN_ASSERT(array.Init(1, memAPI));
			DQN_ASSERT(array.max >= 1);
			DQN_ASSERT(array.count == 0);
			LogSuccess("DqnArray(): Empty array");
		}
		DQN_ASSERT(array.Free());

		if (1)
		{
			DqnV2 a = DqnV2_(1, 2);
			DqnV2 b = DqnV2_(3, 4);
			DqnV2 c = DqnV2_(5, 6);
			DqnV2 d = DqnV2_(7, 8);

			DQN_ASSERT(array.Init(16, memAPI));
			DQN_ASSERT(array.Remove(0) == false);
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 0);

			array.Clear();
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 0);

			DQN_ASSERT(array.Push(a));
			DQN_ASSERT(array.Push(b));
			DQN_ASSERT(array.Push(c));
			DQN_ASSERT(array.Push(d));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 4);

			DQN_ASSERT(array.Remove(0));
			DQN_ASSERT(DqnV2_Equals(array.data[0], d));
			DQN_ASSERT(DqnV2_Equals(array.data[1], b));
			DQN_ASSERT(DqnV2_Equals(array.data[2], c));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 3);

			DQN_ASSERT(array.Remove(2));
			DQN_ASSERT(DqnV2_Equals(array.data[0], d));
			DQN_ASSERT(DqnV2_Equals(array.data[1], b));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 2);

			DQN_ASSERT(array.Remove(100) == false);
			DQN_ASSERT(DqnV2_Equals(array.data[0], d));
			DQN_ASSERT(DqnV2_Equals(array.data[1], b));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 2);

			array.Clear();
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 0);
			LogSuccess("DqnArray(): Test removal");
		}
		DQN_ASSERT(array.Free());

		if (1)
		{
			DqnV2 a = DqnV2_(1, 2);
			DqnV2 b = DqnV2_(3, 4);
			DqnV2 c = DqnV2_(5, 6);
			DqnV2 d = DqnV2_(7, 8);

			DQN_ASSERT(array.Init(16, memAPI));

			DQN_ASSERT(array.Push(a));
			DQN_ASSERT(array.Push(b));
			DQN_ASSERT(array.Push(c));
			DQN_ASSERT(array.Push(d));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 4);

			array.RemoveStable(0);
			DQN_ASSERT(DqnV2_Equals(array.data[0], b));
			DQN_ASSERT(DqnV2_Equals(array.data[1], c));
			DQN_ASSERT(DqnV2_Equals(array.data[2], d));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 3);

			array.RemoveStable(1);
			DQN_ASSERT(DqnV2_Equals(array.data[0], b));
			DQN_ASSERT(DqnV2_Equals(array.data[1], d));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 2);

			array.RemoveStable(1);
			DQN_ASSERT(DqnV2_Equals(array.data[0], b));
			DQN_ASSERT(array.max >= 16);
			DQN_ASSERT(array.count == 1);

			LogSuccess("DqnArray(): Test stable removal");
		}
		DQN_ASSERT(array.Free());
	}

	if (1)
	{
		// Test normal remove list scenario
		if (1)
		{
			i64 indexesToFree[] = {3, 2, 1, 0};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 0);
			array.Free();
		}

		// Test all indexes invalid
		if (1)
		{
			i64 indexesToFree[] = {100, 200, 300, 400};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 4);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 32);
			DQN_ASSERT(array.data[2] == 29);
			DQN_ASSERT(array.data[3] == 31);
			array.Free();
		}

		// Test remove singular index
		if (1)
		{
			i64 indexesToFree[] = {1};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 3);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 29);
			DQN_ASSERT(array.data[2] == 31);
			array.Free();
		}

		// Test remove singular invalid index
		if (1)
		{
			i64 indexesToFree[] = {100};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 4);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 32);
			DQN_ASSERT(array.data[2] == 29);
			DQN_ASSERT(array.data[3] == 31);
			array.Free();
		}

		// Test remove second last index
		if (1)
		{
			i64 indexesToFree[] = {2};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 3);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 32);
			DQN_ASSERT(array.data[2] == 31);
			array.Free();
		}

		// Test remove last 2 indexes
		if (1)
		{
			i64 indexesToFree[] = {2, 3};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));
			DQN_ASSERT(array.count == 2);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 32);
			array.Free();
		}

		// Test invalid free index doesn't delete out of bounds
		if (1)
		{
			i64 indexesToFree[] = {30, 1, 3};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));

			DQN_ASSERT(array.count == 2);
			DQN_ASSERT(array.data[0] == 128);
			DQN_ASSERT(array.data[1] == 29);
			array.Free();
		}

		// Test a free list including the first index
		if (1)
		{
			i64 indexesToFree[] = {0, 1, 2};
			i32 intList[]       = {128, 32, 29, 31};

			DqnArray<i32> array;
			array.Init(DQN_ARRAY_COUNT(intList), memAPI);
			array.Push(intList, DQN_ARRAY_COUNT(intList));
			array.RemoveStable(indexesToFree, DQN_ARRAY_COUNT(indexesToFree));

			DQN_ASSERT(array.count == 1);
			DQN_ASSERT(array.data[0] == 31);
			array.Free();
		}


		LogSuccess("DqnArray(): Test stable removal with list of indexes");
	}
}

void DqnArray_TestRealDataInternal(DqnArray<char> *array)
{
#ifdef DQN_XPLATFORM_LAYER
	size_t bufSize = 0;
	u8 *buf        = DqnFile::ReadEntireFileSimple("tests/google-10000-english.txt", &bufSize);
	DQN_ASSERT(buf);

	for (auto i = 0; i < bufSize; i++)
		array->Push(buf[i]);

	DQN_ASSERT((size_t)array->count == bufSize);
	for (auto i = 0; i < array->count; i++)
		DQN_ASSERT(array->data[i] == buf[i]);

	DQN_ASSERT(array->Free());
	free(buf);

	LogSuccess("DqnArray(): Testing real data");
#endif
}

void DqnArray_Test()
{
	LogHeader("DqnArray_Test");
	if (1)
	{
		DqnArray_TestInternal(DqnMemAPI_HeapAllocator());
	}

	if (1)
	{
		if (1)
		{
			DqnArray<char> array1 = DqnArray_<char>(3);
			DQN_ASSERT(array1.count == 0);
			DQN_ASSERT(array1.max == 3);
			array1.Free();

			array1 = DqnArray_<char>();
			DQN_ASSERT(array1.count == 0);
			DQN_ASSERT(array1.max == 0);

			array1.Push('c');
			DQN_ASSERT(array1.count == 1);
			DQN_ASSERT(array1.max == 1);
			array1.Free();

			LogSuccess("DqnArray(): Testing faux-array constructors DqnArray_()");
		}

		if (1)
		{
			DqnArray<char> array = {};
			DQN_ASSERT(array.Init(1));
			DqnArray_TestRealDataInternal(&array);
		}

		if (1)
		{
			DqnMemStack stack = {}; stack.Init(DQN_MEGABYTE(1), true, 4);
			DqnMemAPI memAPI  = DqnMemAPI_StackAllocator(&stack);

			if (1)
			{
				auto memGuard0 = stack.TempRegionGuard();
				DqnArray<char> array = {};
				DQN_ASSERT(array.Init(1, memAPI));
				DqnArray_TestRealDataInternal(&array);
			}

			// Test reallocing strategies for memory stacks
			if (1)
			{
				auto memGuard0 = stack.TempRegionGuard();
				DqnArray<char> array = {};
				DQN_ASSERT(array.Init(128, memAPI));
				stack.Push(1024);
				DqnArray_TestRealDataInternal(&array);
			}
			stack.Free();
		}
	}
}

void DqnMemStack_Test()
{
	LogHeader("DqnMemStack_Test");

	// Test over allocation, alignments, temp regions
	if (1)
	{
		size_t allocSize    = DQN_KILOBYTE(1);
		DqnMemStack stack   = {};
		const u32 ALIGNMENT = 4;
		stack.Init(allocSize, false, ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Alocate A
		size_t sizeA  = (size_t)(allocSize * 0.5f);
		void *resultA = stack.Push(sizeA);
		DQN_ASSERT(((intptr_t)resultA % ALIGNMENT) == 0);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used >= sizeA + 0 && stack.block->used <= sizeA + 3);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(resultA);
		u8 *ptrA = (u8 *)resultA;
		for (u32 i  = 0; i < sizeA; i++)
			ptrA[i] = 1;

		DqnMemStack::Block *blockA = stack.block;
		// Alocate B
		size_t sizeB  = (size_t)(allocSize * 2.0f);
		void *resultB = stack.Push(sizeB);
		DQN_ASSERT(((intptr_t)resultB % ALIGNMENT) == 0);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == DQN_KILOBYTE(2));

		// Since we alignment the pointers we return they can be within 0-3
		// bytes of what we expect and since this is in a new block as well used
		// will reflect just this allocation.
		DQN_ASSERT(stack.block->used >= sizeB + 0 && stack.block->used <= sizeB + 3);
		DQN_ASSERT(resultB);
		u8 *ptrB = (u8 *)resultB;
		for (u32 i  = 0; i < sizeB; i++)
			ptrB[i] = 2;

		// Check that a new block was created since there wasn't enough space
		DQN_ASSERT(stack.block->prevBlock == blockA);
		DQN_ASSERT(stack.block != blockA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(blockA->used == sizeA);
		DqnMemStack::Block *blockB = stack.block;

		// Check temp regions work
		DqnMemStackTempRegion tempBuffer = stack.TempRegionBegin();

		size_t sizeC  = 1024 + 1;
		void *resultC = stack.Push(sizeC);
		DQN_ASSERT(((intptr_t)resultC % ALIGNMENT) == 0);
		DQN_ASSERT(stack.block != blockB && stack.block != blockA);
		DQN_ASSERT(stack.block->used >= sizeC + 0 && stack.block->used <= sizeC + 3);
		DQN_ASSERT(stack.tempRegionCount == 1);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// NOTE: Allocation should be aligned to 4 byte boundary
		DQN_ASSERT(tempBuffer.stack->block->size == 2048);
		u8 *ptrC = (u8 *)resultC;
		for (u32 i  = 0; i < sizeC; i++)
			ptrC[i] = 3;

		// Check that a new block was created since there wasn't enough space
		DQN_ASSERT(stack.block->prevBlock == blockB);
		DQN_ASSERT(stack.block != blockB);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		for (u32 i = 0; i < sizeA; i++)
			DQN_ASSERT(ptrA[i] == 1);
		for (u32 i = 0; i < sizeB; i++)
			DQN_ASSERT(ptrB[i] == 2);
		for (u32 i = 0; i < sizeC; i++)
			DQN_ASSERT(ptrC[i] == 3);

		// End temp region which should revert back to 2 linked stacks, A and B
		stack.TempRegionEnd(tempBuffer);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == sizeB);
		DQN_ASSERT(stack.block->used >= sizeB + 0 && stack.block->used <= sizeB + 3);
		DQN_ASSERT(stack.tempRegionCount == 0);
		DQN_ASSERT(resultB);

		DQN_ASSERT(stack.block->prevBlock == blockA);
		DQN_ASSERT(stack.block != blockA);
		DQN_ASSERT(blockA->used == sizeA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Release the last linked stack from the push stack
		stack.FreeLastBlock();

		// Which should return back to the 1st allocation
		DQN_ASSERT(stack.block == blockA);
		DQN_ASSERT(stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == sizeA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(!stack.block->prevBlock);

		// Free once more to release stack A memory
		stack.FreeLastBlock();
		DQN_ASSERT(!stack.block);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(stack.tempRegionCount == 0);
		LogSuccess("DqnMemStack(): Test over allocation, alignments, temp regions");
	}

	// Test stack with fixed memory does not allocate more
	if (1)
	{
		u8 memory[DQN_KILOBYTE(1)] = {};
		DqnMemStack stack          = {};
		const u32 ALIGNMENT        = 4;
		stack.InitWithFixedMem(memory, DQN_ARRAY_COUNT(memory), ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == DQN_ARRAY_COUNT(memory) - sizeof(DqnMemStack::Block));
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Allocation larger than stack mem size should fail
		DQN_ASSERT(!stack.Push(DQN_ARRAY_COUNT(memory) * 2));

		// Check free does nothing
		stack.Free();
		stack.FreeLastBlock();
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == DQN_ARRAY_COUNT(memory) - sizeof(DqnMemStack::Block));
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		LogSuccess("DqnMemStack(): Test stack with fixed memory does not allocate more");
	}

	// Test stack with fixed size, allocates once from platform but does not
	// grow further
	if (1)
	{
		size_t allocSize    = DQN_KILOBYTE(1);
		DqnMemStack stack   = {};
		const u32 ALIGNMENT = 4;
		stack.InitWithFixedSize(allocSize, false, ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		void *result = stack.Push((size_t)(0.5f * allocSize));
		DQN_ASSERT(result);

		// Allocating more should fail
		DQN_ASSERT(!stack.Push(allocSize));

		// Freeing should work
		stack.Free();
		DQN_ASSERT(!stack.block);
		LogSuccess(
		    "DqnMemStack(): Test stack with fixed size allocates one from platform but does not "
		    "grow further");
	}

	// Test freeing/clear block and alignment
	if (1)
	{
		size_t firstBlockSize = DQN_KILOBYTE(1);
		DqnMemStack stack     = {};
		const u32 ALIGNMENT   = 16;
		stack.Init(firstBlockSize, false, ALIGNMENT);

		DqnMemStack::Block *firstBlock = stack.block;
		u8 *first                      = NULL;
		{
			u32 allocate40Bytes = 40;
			u8 *data            = (u8 *)stack.Push(allocate40Bytes);

			// Test that the allocation got aligned to 16 byte boundary
			DQN_ASSERT(data);
			DQN_ASSERT(stack.block->size == firstBlockSize);
			DQN_ASSERT((size_t)data % ALIGNMENT == 0);

			for (u32 i  = 0; i < allocate40Bytes; i++)
				data[i] = 'a';

			// Clear the block, but don't zero it out
			stack.ClearCurrBlock(false);
			for (u32 i = 0; i < allocate40Bytes; i++)
				DQN_ASSERT(data[i] == 'a');

			// Test clear reverted the use pointer
			DQN_ASSERT(stack.block->used == 0);
			DQN_ASSERT(stack.block->size == firstBlockSize);

			// Reallocate the data
			data = (u8 *)stack.Push(firstBlockSize);
			DQN_ASSERT(stack.block->size == firstBlockSize);
			DQN_ASSERT((size_t)data % ALIGNMENT == 0);

			// Fill with 'b's
			for (u32 i  = 0; i < firstBlockSize; i++)
				data[i] = 'b';

			// Clear block and zero it out
			stack.ClearCurrBlock(true);
			for (u32 i = 0; i < firstBlockSize; i++)
				DQN_ASSERT(data[i] == 0);

			// General Check stack struct contains the values we expect from
			// initialisation
			DQN_ASSERT(stack.flags == 0);
			DQN_ASSERT(stack.tempRegionCount == 0);
			DQN_ASSERT(stack.byteAlign == ALIGNMENT);
			DQN_ASSERT(stack.block->size == firstBlockSize);

			// Write out data to current block
			data = (u8 *)stack.Push(firstBlockSize);
			for (u32 i  = 0; i < firstBlockSize; i++)
				data[i] = 'c';

			first = data;
		}

		// Force it to allocate three new blocks and write out data to each
		size_t secondBlockSize          = DQN_KILOBYTE(2);
		u8 *second                      = (u8 *)stack.Push(secondBlockSize);
		DqnMemStack::Block *secondBlock = stack.block;
		for (u32 i    = 0; i < secondBlockSize; i++)
			second[i] = 'd';

		size_t thirdBlockSize          = DQN_KILOBYTE(3);
		u8 *third                      = (u8 *)stack.Push(thirdBlockSize);
		DqnMemStack::Block *thirdBlock = stack.block;
		for (u32 i   = 0; i < thirdBlockSize; i++)
			third[i] = 'e';

		size_t fourthBlockSize          = DQN_KILOBYTE(4);
		u8 *fourth                      = (u8 *)stack.Push(fourthBlockSize);
		DqnMemStack::Block *fourthBlock = stack.block;
		for (u32 i    = 0; i < fourthBlockSize; i++)
			fourth[i] = 'f';

		DQN_ASSERT((firstBlock != secondBlock) && (secondBlock != thirdBlock) &&
		           (thirdBlock != fourthBlock));
		DQN_ASSERT(firstBlock->prevBlock == NULL);
		DQN_ASSERT(secondBlock->prevBlock == firstBlock);
		DQN_ASSERT(thirdBlock->prevBlock == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		// NOTE: Making blocks manually is not really recommended ..
		// Try and free an invalid block by mocking a fake block
		u8 fakeBlockMem[DQN_KILOBYTE(3)] = {};
		DqnMemStack::Block fakeBlock     = {};
		fakeBlock.memory                 = fakeBlockMem;
		fakeBlock.size                   = DQN_ARRAY_COUNT(fakeBlockMem);
		fakeBlock.used                   = 0;
		DQN_ASSERT(!stack.FreeMemBlock(&fakeBlock));

		// Ensure that the actual blocks are still valid and freeing did nothing
		DQN_ASSERT(firstBlock->size == firstBlockSize);
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(thirdBlock->size == thirdBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(firstBlock->used == firstBlockSize);
		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(thirdBlock->used == thirdBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT((firstBlock != secondBlock) && (secondBlock != thirdBlock) &&
		           (thirdBlock != fourthBlock));
		DQN_ASSERT(firstBlock->prevBlock == NULL);
		DQN_ASSERT(secondBlock->prevBlock == firstBlock);
		DQN_ASSERT(thirdBlock->prevBlock == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		for (u32 i = 0; i < firstBlockSize; i++)
			DQN_ASSERT(first[i] == 'c');

		for (u32 i = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');

		for (u32 i = 0; i < thirdBlockSize; i++)
			DQN_ASSERT(third[i] == 'e');

		for (u32 i = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the first block
		stack.FreeMemBlock(firstBlock);

		// Revalidate state
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(thirdBlock->size == thirdBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(thirdBlock->used == thirdBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT((secondBlock != thirdBlock) && (thirdBlock != fourthBlock));
		DQN_ASSERT(secondBlock->prevBlock == NULL);
		DQN_ASSERT(thirdBlock->prevBlock == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		for (u32 i = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');

		for (u32 i = 0; i < thirdBlockSize; i++)
			DQN_ASSERT(third[i] == 'e');

		for (u32 i = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the third block
		stack.FreeMemBlock(thirdBlock);

		// Revalidate state
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT(secondBlock != fourthBlock);
		DQN_ASSERT(secondBlock->prevBlock == NULL);
		DQN_ASSERT(fourthBlock->prevBlock == secondBlock);

		for (u32 i = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');

		for (u32 i = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the second block
		stack.FreeMemBlock(secondBlock);

		// Revalidate state
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);
		DQN_ASSERT(fourthBlock->prevBlock == NULL);

		for (u32 i = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the stack
		stack.Free();
		DQN_ASSERT(!stack.block);
		LogSuccess("DqnMemStack(): Test freeing/clear block and alignment");
	}

	// Test pop
	if (1)
	{
		// Test aligned pop
		if (1)
		{
			DqnMemStack stack = {};
			stack.Init(DQN_KILOBYTE(1), true);

			size_t allocSize = 512;
			void *alloc      = stack.Push(allocSize);
			DQN_ASSERT(stack.block->used == allocSize);

			DQN_ASSERT(stack.Pop(alloc, allocSize));
			DQN_ASSERT(stack.block->used == 0);
			stack.Free();
			LogSuccess("DqnMemStack(): Test aligned pop");
		}

		// Test pop on a non-byte aligned allocation. This checks to see if
		// Pop() doesn't naiively forget to re-byte align the passed in size.
		if (1)
		{
			DqnMemStack stack = {};
			stack.Init(DQN_KILOBYTE(1), true);

			size_t allocSize = 1;
			void *alloc      = stack.Push(allocSize);
			DQN_ASSERT(stack.block->used == DQN_ALIGN_POW_N(allocSize, stack.byteAlign));

			DQN_ASSERT(stack.Pop(alloc, allocSize));
			DQN_ASSERT(stack.block->used == 0);
			stack.Free();
			LogSuccess("DqnMemStack(): Test pop on non-aligned allocation");
		}
	}
}

#ifdef DQN_XPLATFORM_LAYER
void DqnFile_Test()
{
	LogHeader("DqnFile");
	// File i/o
	if (1)
	{

		// Test file open
		if (1)
		{
			const char *const FILE_TO_OPEN = ".clang-format";
			u32 expectedSize               = 0;
#if defined(DQN_UNIX_IMPLEMENTATION)
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
				DQN_ASSERT(DqnFile_GetFileSize("/proc/cpuinfo", &size));
				DQN_ASSERT(size > 0);
			}

#elif defined(DQN_WIN32_IMPLEMENTATION)
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
				DQN_ASSERT_HARD(DqnFile::GetFileSize(FILE_TO_OPEN, size));
				DQN_ASSERT_HARD(size == expectedSize);
			}

			DqnFile file = {};
			DQN_ASSERT(file.Open(".clang-format",
			                        (DqnFile::PermissionFlag::FileWrite | DqnFile::PermissionFlag::FileRead),
			                        DqnFile::Action::OpenOnly));

			DQN_ASSERT_MSG(file.size == expectedSize,
			               "DqnFileOpen() failed: file.size: %d, expected:%d\n", file.size,
			               expectedSize);

			u8 *buffer = (u8 *)calloc(1, (size_t)file.size * sizeof(u8));
			DQN_ASSERT(file.Read(buffer, (u32)file.size) == file.size);
			free(buffer);

			file.Close();
			DQN_ASSERT(!file.handle && file.size == 0 && file.flags == 0);

			if (1)
			{
				DqnFile raiiFile = DqnFile(true);
				if (raiiFile.Open(FILE_TO_OPEN,
				                  DqnFile::PermissionFlag::FileWrite | DqnFile::PermissionFlag::FileRead,
				                  DqnFile::Action::OpenOnly))
				{
					i32 breakHereToTestRaii = 0;
					(void)breakHereToTestRaii;
				}
			}

			LogSuccess("DqnFile(): General test");
		}

		// Test invalid file
		if (1)
		{
			DqnFile file = {};
			DQN_ASSERT(!file.Open("asdljasdnel;kajdf", (DqnFile::PermissionFlag::FileWrite |
			                                            DqnFile::PermissionFlag::FileRead),
			                      DqnFile::Action::OpenOnly));
			DQN_ASSERT(file.size == 0);
			DQN_ASSERT(file.flags == 0);
			DQN_ASSERT(!file.handle);
			LogSuccess("DqnFile(): Invalid file test");
		}
	}

	////////////////////////////////////////////////////////////////////////////
	// Write Test
	////////////////////////////////////////////////////////////////////////////
	if (1)
	{
		const char *fileNames[]                   = {"dqn_1", "dqn_2", "dqn_3", "dqn_4", "dqn_5"};
		const char *writeData[]                   = {"1234", "2468", "36912", "481216", "5101520"};
		DqnFile files[DQN_ARRAY_COUNT(fileNames)] = {};

		// Write data out to some files
		for (u32 i = 0; i < DQN_ARRAY_COUNT(fileNames); i++)
		{
			u32 permissions = DqnFile::PermissionFlag::FileRead | DqnFile::PermissionFlag::FileWrite;
			DqnFile *file = files + i;
			if (!file->Open(fileNames[i], permissions, DqnFile::Action::ClearIfExist))
			{
				bool result =
				    file->Open(fileNames[i], permissions, DqnFile::Action::CreateIfNotExist);
				DQN_ASSERT(result);
			}

			size_t bytesToWrite = DqnStr_Len(writeData[i]);
			u8 *dataToWrite     = (u8 *)(writeData[i]);
			size_t bytesWritten = file->Write(dataToWrite, bytesToWrite, 0);
			DQN_ASSERT(bytesWritten == bytesToWrite);
			file->Close();
		}

		DqnMemStack memStack = {};
		DQN_ASSERT(memStack.Init(DQN_MEGABYTE(1), true));
		// Read data back in
		for (u32 i = 0; i < DQN_ARRAY_COUNT(fileNames); i++)
		{
			// Manual read the file contents
			{
				u32 permissions = DqnFile::PermissionFlag::FileRead;
				DqnFile *file = files + i;
				bool result = file->Open(fileNames[i], permissions, DqnFile::Action::OpenOnly);
				DQN_ASSERT(result);

				u8 *buffer = (u8 *)memStack.Push(file->size);
				DQN_ASSERT(buffer);

				size_t bytesRead = file->Read(buffer, file->size);
				DQN_ASSERT(bytesRead == file->size);

				// Verify the data is the same as we wrote out
				DQN_ASSERT(DqnStr_Cmp((char *)buffer, (writeData[i])) == 0);

				// Delete when we're done with it
				DQN_ASSERT(memStack.Pop(buffer, file->size));
				file->Close();
			}

			// Read using the ReadEntireFile api which doesn't need a file handle as an argument
			{
				size_t reqSize = 0;
				DQN_ASSERT(DqnFile::GetFileSize(fileNames[i], reqSize));

				u8 *buffer = (u8 *)memStack.Push(reqSize);
				DQN_ASSERT(buffer);

				size_t bytesRead = 0;
				DQN_ASSERT(DqnFile::ReadEntireFile(fileNames[i], buffer, reqSize, bytesRead));
				DQN_ASSERT(bytesRead == reqSize);

				// Verify the data is the same as we wrote out
				DQN_ASSERT(DqnStr_Cmp((char *)buffer, (writeData[i])) == 0);
				DQN_ASSERT(memStack.Pop(buffer, reqSize));
			}

			DQN_ASSERT(DqnFile::Delete(fileNames[i]));
		}

		// Then check delete actually worked, files should not exist.
		for (u32 i = 0; i < DQN_ARRAY_COUNT(fileNames); i++)
		{
			DqnFile dummy   = {};
			u32 permissions = DqnFile::PermissionFlag::FileRead;
			bool fileExists = dummy.Open(fileNames[i], permissions, DqnFile::Action::OpenOnly);
			DQN_ASSERT(!fileExists);
		}
		memStack.Free();

		LogSuccess("DqnFile(): Write file");
	}

	////////////////////////////////////////////////////////////////////////////
	// Test directory listing
	////////////////////////////////////////////////////////////////////////////
	if (1)
	{
		i32 numFiles;
#if defined(DQN_UNIX_IMPLEMENTATION)
		char **filelist = DqnFile::ListDir(".", numFiles);
#elif defined(DQN_WIN32_IMPLEMENTATION)
		char **filelist = DqnFile::ListDir("*", numFiles);
#endif

		printf("DqnFile(): Display read files\n");
		for (auto i = 0; i < numFiles; i++)
			printf("DqnFile(): DirRead: %s\n", filelist[i]);

		DqnFile::ListDirFree(filelist, numFiles);
		LogSuccess("DqnFile(): List directory files");
	}

}
#endif

FILE_SCOPE u32 volatile globalDebugCounter;
FILE_SCOPE DqnLock globalJobQueueLock;
const u32 QUEUE_SIZE = 256;
FILE_SCOPE void JobQueueDebugCallbackIncrementCounter(DqnJobQueue *const queue,
                                                      void *const userData)
{
	(void)userData;
	DQN_ASSERT(queue->size == QUEUE_SIZE);
	{
		DqnLockGuard guard = globalJobQueueLock.LockGuard();
		globalDebugCounter++;

#if 0
		u32 number = globalDebugCounter;
#if defined(DQN_WIN32_IMPLEMENTATION)
		printf("JobQueueDebugCallbackIncrementCounter(): Thread %d: Incrementing Number: %d\n",
		       GetCurrentThreadId(), number);
#elif defined(DQN_UNIX_IMPLEMENTATION)
		printf("JobQueueDebugCallbackIncrementCounter(): Thread unix: Incrementing Number: %d\n",
		       number);
#endif
#endif
	}

}

FILE_SCOPE void DqnJobQueue_Test()
{
	LogHeader("DqnJobQueue: Multithreading Test");
	globalDebugCounter = 0;

	DqnMemStack memStack = {};
	DQN_ASSERT_HARD(memStack.Init(DQN_MEGABYTE(1), true));

	u32 numThreads, numCores;
	DqnPlatform_GetNumThreadsAndCores(&numCores, &numThreads);
	DQN_ASSERT(numThreads > 0 && numCores > 0);

	u32 totalThreads = (numCores - 1) * numThreads;
	if (totalThreads == 0) totalThreads = 1;

	DqnJobQueue jobQueue = {};
	DqnJob *jobList      = (DqnJob *)memStack.Push(sizeof(*jobQueue.jobList) * QUEUE_SIZE);
	DQN_ASSERT(DqnJobQueue_Init(&jobQueue, jobList, QUEUE_SIZE, totalThreads));

	const u32 WORK_ENTRIES = 2048;
	DQN_ASSERT(DqnLock_Init(&globalJobQueueLock));
	for (u32 i = 0; i < WORK_ENTRIES; i++)
	{
		DqnJob job   = {};
		job.callback = JobQueueDebugCallbackIncrementCounter;
		while (!DqnJobQueue_AddJob(&jobQueue, job))
		{
			DqnJobQueue_TryExecuteNextJob(&jobQueue);
		}
	}

	DqnJobQueue_BlockAndCompleteAllJobs(&jobQueue);
	DQN_ASSERT(globalDebugCounter == WORK_ENTRIES);
	DqnLock_Delete(&globalJobQueueLock);

	printf("DqnJobQueue(): Final incremented value: %d\n", globalDebugCounter);
	LogSuccess("DqnJobQueue()");
}

#include <algorithm>
void DqnQuickSort_Test()
{
	LogHeader("DqnSort vs std::Sort");
	DqnRndPCG state; state.Init();
	if (1)
	{
		DqnMemStack stack = {};
		DQN_ASSERT(stack.Init(DQN_KILOBYTE(1), false));

		// Create array of ints
		u32 numInts      = 1000000;
		u32 sizeInBytes  = sizeof(u32) * numInts;
		u32 *dqnCPPArray = (u32 *)stack.Push(sizeInBytes);
		u32 *stdArray    = (u32 *)stack.Push(sizeInBytes);
		DQN_ASSERT(dqnCPPArray && stdArray);

		f64 dqnCPPTimings[5]                           = {};
		f64 stdTimings[DQN_ARRAY_COUNT(dqnCPPTimings)] = {};

		f64 dqnCPPAverage = 0;
		f64 stdAverage    = 0;

		for (u32 timingsIndex = 0; timingsIndex < DQN_ARRAY_COUNT(dqnCPPTimings); timingsIndex++)
		{
			// Populate with random numbers
			for (u32 i = 0; i < numInts; i++)
			{
				dqnCPPArray[i] = state.Next();
				stdArray[i]    = dqnCPPArray[i];
			}

			// Time Dqn_QuickSort
			{
				f64 start = DqnTimer_NowInS();
				Dqn_QuickSort(dqnCPPArray, numInts);

				f64 duration = DqnTimer_NowInS() - start;
				dqnCPPTimings[timingsIndex] = duration;
				dqnCPPAverage += duration;
				printf("Dqn_QuickSort: %f vs ", dqnCPPTimings[timingsIndex]);
			}

			// Time std::sort
			{
				f64 start = DqnTimer_NowInS();
				std::sort(stdArray, stdArray + numInts);
				f64 duration = DqnTimer_NowInS() - start;

				stdTimings[timingsIndex] = duration;
				stdAverage += duration;

				printf("std::sort: %f\n", stdTimings[timingsIndex]);
			}

			// Validate algorithm is correct
			for (u32 i = 0; i < numInts; i++)
			{
				DQN_ASSERT_MSG(dqnCPPArray[i] == stdArray[i], "DqnArray[%d]: %d, stdArray[%d]: %d", i,
				               dqnCPPArray[i], stdArray[i], i);
			}
		}

		// Print averages
		if (1)
		{
			dqnCPPAverage /= (f64)DQN_ARRAY_COUNT(dqnCPPTimings);
			stdAverage    /= (f64)DQN_ARRAY_COUNT(stdTimings);
			printf("\n- Average Timings\n");
			printf("    Dqn_QuickSort: %f vs std::sort: %f\n", dqnCPPAverage, stdAverage);
		}
		stack.Free();
	}
}

void DqnHashTable_Test()
{
	LogHeader("DqnHashTable");
	DqnHashTable<u32> hashTable = {};
	hashTable.Init(1);

	{
		hashTable.AddNewEntriesToFreeList(+2);
		DQN_ASSERT(hashTable.freeList && hashTable.freeList->next);
		DQN_ASSERT(hashTable.numFreeEntries == 2);

		hashTable.AddNewEntriesToFreeList(-1);
		DQN_ASSERT(hashTable.freeList && !hashTable.freeList->next);
		DQN_ASSERT(hashTable.numFreeEntries == 1);
	}

	{
		DQN_ASSERT(hashTable.Get("hello world") == nullptr);
		DQN_ASSERT(hashTable.Get("collide key") == nullptr);
		DQN_ASSERT(hashTable.Get("crash again") == nullptr);

		bool entryAlreadyExisted = true;
		auto helloEntry = hashTable.Make("hello world", -1, &entryAlreadyExisted);
		DQN_ASSERT(entryAlreadyExisted == false);

		entryAlreadyExisted = true;
		auto collideEntry   = hashTable.Make("collide key", -1, &entryAlreadyExisted);
		DQN_ASSERT(entryAlreadyExisted == false);

		entryAlreadyExisted = true;
		auto crashEntry     = hashTable.Make("crash again", -1, &entryAlreadyExisted);
		DQN_ASSERT(entryAlreadyExisted == false);

		helloEntry->data   = 5;
		collideEntry->data = 10;
		crashEntry->data   = 15;

		DQN_ASSERT(hashTable.numFreeEntries == 0);

		DqnHashTable<u32>::Entry *entry = *hashTable.entries;
		DQN_ASSERT(entry->data == 15);

		entry = entry->next;
		DQN_ASSERT(entry->data == 10);

		entry = entry->next;
		DQN_ASSERT(entry->data == 5);

		DQN_ASSERT(hashTable.usedEntriesIndex == 1);
		DQN_ASSERT(hashTable.usedEntries[0] == 0);
		DQN_ASSERT(hashTable.numFreeEntries == 0);
	}

	hashTable.Remove("hello world");
	DQN_ASSERT(hashTable.ChangeNumEntries(512));

	{
		auto helloEntry  = hashTable.Get("hello world");
		DQN_ASSERT(helloEntry == nullptr);

		auto collideEntry  = hashTable.Get("collide key");
		DQN_ASSERT(collideEntry->data == 10);

		auto crashEntry  = hashTable.Get("crash again");
		DQN_ASSERT(crashEntry->data == 15);

		bool entryAlreadyExisted = false;
		collideEntry = hashTable.Make("collide key", -1, &entryAlreadyExisted);
		DQN_ASSERT(entryAlreadyExisted == true);

		entryAlreadyExisted = false;
		crashEntry = hashTable.Make("crash again", -1, &entryAlreadyExisted);
		DQN_ASSERT(entryAlreadyExisted == true);
	}

	hashTable.Free();
	LogSuccess("DqnHashTable()");
}

void Dqn_BSearch_Test()
{
	LogHeader("Dqn_BSearch");
	if (1)
	{
		auto IsLessThan = [](const u32 &a, const u32 &b) -> bool {
			bool result = a < b;
			return result;
		};

		auto Equals = [](const u32 &a, const u32 &b) -> bool {
			bool result = (a == b);
			return result;
		};


		u32 array[] = {1, 2, 3};
		i64 result = Dqn_BSearch<u32>(array, DQN_ARRAY_COUNT(array), 1, Equals, IsLessThan);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch<u32>(array, DQN_ARRAY_COUNT(array), 2, Equals, IsLessThan);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch<u32>(array, DQN_ARRAY_COUNT(array), 3, Equals, IsLessThan);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch<u32>(array, DQN_ARRAY_COUNT(array), 4, Equals, IsLessThan);
		DQN_ASSERT(result == -1);
		LogSuccess("Dqn_BSearch(): With odd sized array and custom compare");
	}

	if (1)
	{
		i64 array[] = {1, 2, 3, 4};
		i64 result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 1);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 2);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 3);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 4);
		DQN_ASSERT(result == 3);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 5);
		DQN_ASSERT(result == -1);
		LogSuccess("Dqn_BSearch(): With even sized array");
	}

	if (1)
	{
		i64 array[] = {1, 2, 3};
		i64 result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 0, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 1, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 2, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 3, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 4, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 5, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 2);
		LogSuccess("Dqn_BSearch(): Lower bound with odd sized array");
	}

	if (1)
	{
		i64 array[] = {1, 2, 3, 4};

		i64 result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 0, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 1, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 2, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 3, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 4, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 5, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 3);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 6, Dqn_BSearchBound_Lower);
		DQN_ASSERT(result == 3);
		LogSuccess("Dqn_BSearch(): Lower bound with even sized array");
	}

	if (1)
	{
		i64 array[] = {1, 2, 3};
		i64 result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 0, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 1, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 2, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 3, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 4, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 5, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);
		LogSuccess("Dqn_BSearch(): Higher bound with odd sized array");
	}

	if (1)
	{
		i64 array[] = {1, 2, 3, 4};

		i64 result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 0, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 0);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 1, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 2, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 2);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 3, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == 3);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 4, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 5, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);

		result = Dqn_BSearch(array, DQN_ARRAY_COUNT(array), 6, Dqn_BSearchBound_Higher);
		DQN_ASSERT(result == -1);
		LogSuccess("Dqn_BSearch(): Higher bound with even sized array");
	}
}

void DqnMemSet_Test()
{
	LogHeader("DqnMemSet Test");
	DqnRndPCG rnd = DqnRndPCG_();

	const int NUM_TIMINGS = 5;
#if defined(DQN_WIN32_IMPLEMENTATION)
	f64 timings[3][NUM_TIMINGS] = {};
#else
	f64 timings[2][NUM_TIMINGS] = {};
#endif
	f64 avgTimings[DQN_ARRAY_COUNT(timings)] = {};
	void *buffers[DQN_ARRAY_COUNT(timings)]  = {};

	const i32 NUM_ITERATIONS = DQN_ARRAY_COUNT(timings[0]);
	for (auto i = 0; i < NUM_ITERATIONS; i++)
	{
		i32 size = rnd.Range(DQN_MEGABYTE(16), DQN_MEGABYTE(32));
		u8 value = (u8)rnd.Range(0, 255);

		printf("%04d:", i);

		i32 timingsIndex = 0;
		// DqnMem_Set
		{
			buffers[timingsIndex] = malloc(size); DQN_ASSERT(buffers[timingsIndex]);

			f64 start = DqnTimer_NowInMs();
			DqnMem_Set(buffers[timingsIndex], value, size);
			f64 duration = DqnTimer_NowInMs() - start;

			timings[timingsIndex++][i] = duration;
			printf("DqnMem_Set: %8.3f vs ", duration);
		}

#if defined(DQN_WIN32_IMPLEMENTATION)
		// DqnMem_Set64
		{
			buffers[timingsIndex] = malloc(size); DQN_ASSERT(buffers[timingsIndex]);

			f64 start = DqnTimer_NowInMs();
			DqnMem_Set64(buffers[timingsIndex], value, size);
			f64 duration = DqnTimer_NowInMs() - start;

			timings[timingsIndex++][i] = duration;
			printf("DqnMem_Set: %8.3f vs ", duration);
		}
#endif

		// crt memset
		{
			buffers[timingsIndex] = malloc(size); DQN_ASSERT(buffers[timingsIndex]);

			f64 start = DqnTimer_NowInMs();
			memset(buffers[timingsIndex], value, size);
			f64 duration = DqnTimer_NowInMs() - start;

			timings[timingsIndex++][i] = duration;
			printf("memset: %8.3f\n", duration);
		}

		for (auto testIndex = 0; testIndex < size; testIndex++)
		{
			DQN_ASSERT(((u8 *)buffers[0])[testIndex] == ((u8 *)buffers[1])[testIndex]);
			DQN_ASSERT(((u8 *)buffers[1])[testIndex] == ((u8 *)buffers[2])[testIndex]);
		}

		for (auto bufferIndex = 0; bufferIndex < DQN_ARRAY_COUNT(buffers); bufferIndex++)
		{
			free(buffers[bufferIndex]);
		}
	}

	for (auto timingsIndex = 0; timingsIndex < DQN_ARRAY_COUNT(timings); timingsIndex++)
	{
		f64 totalTime = 0;
		for (auto iterationIndex = 0; iterationIndex < NUM_ITERATIONS; iterationIndex++)
		{
			totalTime += timings[timingsIndex][iterationIndex];
		}
		avgTimings[timingsIndex] = totalTime / (f64)NUM_ITERATIONS;
	}

	printf("\n- Average Timings\n");
#if defined(DQN_WIN32_IMPLEMENTATION)
	printf("    DqnMem_Set: %f vs DqnMem_Set64: %f vs memset: %f\n", avgTimings[0], avgTimings[1],
	       avgTimings[2]);
#else
	printf("    DqnMem_Set: %f vs memset: %f\n", avgTimings[0], avgTimings[1]);
#endif
	LogSuccess("DqnMem_Set(): Completed succesfully");
}

int main(void)
{
	DqnString_Test();
	DqnChar_Test();
	DqnRnd_Test();
	DqnMath_Test();
	DqnVX_Test();
	DqnRect_Test();
	DqnArray_Test();
	DqnMemStack_Test();
	DqnQuickSort_Test();
	DqnHashTable_Test();
	Dqn_BSearch_Test();
	DqnMemSet_Test();

#ifdef DQN_XPLATFORM_LAYER
	DqnFile_Test();
	DqnTimer_Test();
	DqnJobQueue_Test();
#endif

	printf("\nPress 'Enter' Key to Exit\n");
	getchar();

	return 0;
}

#if defined(__GNUC__)
	#pragma GCC diagnostic pop
#endif

