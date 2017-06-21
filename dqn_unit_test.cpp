#if (defined(_WIN32) || defined(_WIN64))
	#define DQN_WIN32_IMPLEMENTATION
#endif

#if defined(__linux__)
	#define DQN_UNIX_IMPLEMENTATION
	#define HANDMADE_MATH_NO_SSE
#endif

#define DQN_IMPLEMENTATION
#include "dqn.h"

#define HANDMADE_MATH_IMPLEMENTATION
#define HANDMADE_MATH_CPP_MODE
#include "tests/HandmadeMath.h"

#include <stdio.h>
#include <limits.h>
void HandmadeMathVerifyMat4(DqnMat4 dqnMat, hmm_mat4 hmmMat)
{
	f32 *hmmMatf = (f32 *)&hmmMat;
	f32 *dqnMatf = (f32 *)&dqnMat;

	const u32 EXPECTED_SIZE = 16;
	u32 totalSize = DQN_ARRAY_COUNT(dqnMat.e) * DQN_ARRAY_COUNT(dqnMat.e[0]);
	DQN_ASSERT(totalSize == EXPECTED_SIZE);
	DQN_ASSERT(totalSize == (DQN_ARRAY_COUNT(hmmMat.Elements) * DQN_ARRAY_COUNT(hmmMat.Elements[0])));

	for (u32 i = 0; i < EXPECTED_SIZE; i++)
	{
		const f32 EPSILON = 0.001f;
		f32 diff = hmmMatf[i] - dqnMatf[i];
		diff = DQN_ABS(diff);
		DQN_ASSERT_MSG(diff < EPSILON, "hmmMatf[%d]: %f, dqnMatf[%d]: %f\n", i,
		               hmmMatf[i], i, dqnMatf[i]);
	}
}

void HandmadeMathTest()
{
	// Test Perspective/Projection matrix values
	{
		f32 aspectRatio = 1;
		DqnMat4 dqnPerspective = DqnMat4_Perspective(90, aspectRatio, 100, 1000);
		hmm_mat4 hmmPerspective = HMM_Perspective(90, aspectRatio, 100, 1000);
		HandmadeMathVerifyMat4(dqnPerspective, hmmPerspective);

		printf("HandmadeMathTest(): Perspective: Completed successfully\n");
	}

	// Test Mat4 translate * scale
	{
		hmm_vec3 hmmVec       = HMM_Vec3i(1, 2, 3);
		DqnV3 dqnVec          = DqnV3_3i(1, 2, 3);
		DqnMat4 dqnTranslate  = DqnMat4_Translate(dqnVec.x, dqnVec.y, dqnVec.z);
		hmm_mat4 hmmTranslate = HMM_Translate(hmmVec);
		HandmadeMathVerifyMat4(dqnTranslate, hmmTranslate);

#if 0
		hmm_vec3 hmmAxis      = HMM_Vec3(0.5f, 0.2f, 0.7f);
		DqnV3 dqnAxis         = DqnV3_3f(0.5f, 0.2f, 0.7f);
		f32 rotationInDegrees = 80.0f;

		// TODO(doyle): ?? Handmade Math does it a rotations in a different way
		// way, they normalise the given axis producing different results.
		// HandmadeMathVerifyMat4(dqnRotate, hmmRotate);
#endif

		dqnVec *= 2;
		hmmVec *= 2;
		DqnMat4 dqnScale  = DqnMat4_Scale(dqnVec.x, dqnVec.y, dqnVec.z);
		hmm_mat4 hmmScale = HMM_Scale(hmmVec);
		HandmadeMathVerifyMat4(dqnScale, hmmScale);

		DqnMat4 dqnTSMatrix  = DqnMat4_Mul(dqnTranslate, dqnScale);
		hmm_mat4 hmmTSMatrix = HMM_MultiplyMat4(hmmTranslate, hmmScale);
		HandmadeMathVerifyMat4(dqnTSMatrix, hmmTSMatrix);


		// Test Mat4 * MulV4
		{
			DqnV4 dqnV4    = DqnV4_4f(1, 2, 3, 4);
			hmm_vec4 hmmV4 = HMM_Vec4(1, 2, 3, 4);

			DqnV4 dqnResult    = DqnMat4_MulV4(dqnTSMatrix, dqnV4);
			hmm_vec4 hmmResult = HMM_MultiplyMat4ByVec4(hmmTSMatrix, hmmV4);

			DQN_ASSERT(dqnResult.x == hmmResult.X);
			DQN_ASSERT(dqnResult.y == hmmResult.Y);
			DQN_ASSERT(dqnResult.z == hmmResult.Z);
			DQN_ASSERT(dqnResult.w == hmmResult.W);

			printf(
			    "HandmadeMathTest(): Mat4 * MulV4: Completed successfully\n");
		}

		printf("HandmadeMathTest(): Translate/Scale/Rotate Mat4_Mul: Completed successfully\n");
	}

	// Test LookAt/Camera/View matrix returns same results
	{
		DqnMat4 dqnViewMatrix = DqnMat4_LookAt(DqnV3_3f(4, 3, 3), DqnV3_1f(0), DqnV3_3f(0, 1, 0));
		hmm_mat4 hmmViewMatrix =
		    HMM_LookAt(HMM_Vec3(4, 3, 3), HMM_Vec3(0, 0, 0), HMM_Vec3(0, 1, 0));

		HandmadeMathVerifyMat4(dqnViewMatrix, hmmViewMatrix);
		printf("HandmadeMathTest(): LookAt: Completed successfully\n");
	}

}

void StringsTest()
{
	{ // Char Checks
		DQN_ASSERT(DqnChar_IsAlpha('a') == true);
		DQN_ASSERT(DqnChar_IsAlpha('A') == true);
		DQN_ASSERT(DqnChar_IsAlpha('0') == false);
		DQN_ASSERT(DqnChar_IsAlpha('@') == false);
		DQN_ASSERT(DqnChar_IsAlpha(' ') == false);
		DQN_ASSERT(DqnChar_IsAlpha('\n') == false);

		DQN_ASSERT(DqnChar_IsDigit('1') == true);
		DQN_ASSERT(DqnChar_IsDigit('n') == false);
		DQN_ASSERT(DqnChar_IsDigit('N') == false);
		DQN_ASSERT(DqnChar_IsDigit('*') == false);
		DQN_ASSERT(DqnChar_IsDigit(' ') == false);
		DQN_ASSERT(DqnChar_IsDigit('\n') == false);

		DQN_ASSERT(DqnChar_IsAlphaNum('1') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('a') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('A') == true);
		DQN_ASSERT(DqnChar_IsAlphaNum('*') == false);
		DQN_ASSERT(DqnChar_IsAlphaNum(' ') == false);
		DQN_ASSERT(DqnChar_IsAlphaNum('\n') == false);

		DQN_ASSERT(DqnChar_ToLower(L'A') == L'a');
		DQN_ASSERT(DqnChar_ToLower(L'a') == L'a');
		DQN_ASSERT(DqnChar_ToLower(L' ') == L' ');

		DQN_ASSERT(DqnChar_ToUpper(L'A') == L'A');
		DQN_ASSERT(DqnChar_ToUpper(L'a') == L'A');
		DQN_ASSERT(DqnChar_ToUpper(L' ') == L' ');

		printf("StringsTest(): CharChecks: Completed successfully\n");
	}

	// String Checks
	{
		// strcmp
		{
			const char *const a = "str_a";

			// Check simple compares
			{
				DQN_ASSERT(DqnStr_Cmp(a, "str_a") == +0);
				DQN_ASSERT(DqnStr_Cmp(a, "str_b") == -1);
				DQN_ASSERT(DqnStr_Cmp("str_b", a) == +1);
				DQN_ASSERT(DqnStr_Cmp(a, "") == +1);
				DQN_ASSERT(DqnStr_Cmp("", "") == 0);

				// NOTE: Check that the string has not been trashed.
				DQN_ASSERT(DqnStr_Cmp(a, "str_a") == +0);
			}

			// Check ops against null
			{
				DQN_ASSERT(DqnStr_Cmp(NULL, NULL) != +0);
				DQN_ASSERT(DqnStr_Cmp(a, NULL) != +0);
				DQN_ASSERT(DqnStr_Cmp(NULL, a) != +0);
			}

			printf("StringsTest(): strcmp: Completed successfully\n");
		}

		// strlen
		{
			const char *const a = "str_a";
			DQN_ASSERT(DqnStr_Len(a) == 5);
			DQN_ASSERT(DqnStr_Len("") == 0);
			DQN_ASSERT(DqnStr_Len("   a  ") == 6);
			DQN_ASSERT(DqnStr_Len("a\n") == 2);

			// NOTE: Check that the string has not been trashed.
			DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);

			DQN_ASSERT(DqnStr_Len(NULL) == 0);

			printf("StringsTest(): strlen: Completed successfully\n");
		}

		// strncpy
		{
			{
				const char *const a    = "str_a";
				char b[10] = {};
				// Check copy into empty array
				{
					char *result = DqnStr_Copy(b, a, DqnStr_Len(a));
					DQN_ASSERT(DqnStr_Cmp(b, "str_a") == 0);
					DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);
					DQN_ASSERT(DqnStr_Cmp(result, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(result) == 5);
				}

				// Check copy into array offset, overlap with old results
				{
					char *newResult = DqnStr_Copy(&b[1], a, DqnStr_Len(a));
					DQN_ASSERT(DqnStr_Cmp(newResult, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(newResult) == 5);

					DQN_ASSERT(DqnStr_Cmp(a, "str_a") == 0);
					DQN_ASSERT(DqnStr_Len(a) == 5);

					DQN_ASSERT(DqnStr_Cmp(b, "sstr_a") == 0);
					DQN_ASSERT(DqnStr_Len(b) == 6);
				}
			}
		}

		// StrReverse
		{
			// Basic reverse operations
			{
				char a[] = "aba";
				DQN_ASSERT(DqnStr_Reverse(a, DqnStr_Len(a)) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);

				DQN_ASSERT(DqnStr_Reverse(a, 2) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "baa") == 0);

				DQN_ASSERT(DqnStr_Reverse(a, DqnStr_Len(a)) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "aab") == 0);

				DQN_ASSERT(DqnStr_Reverse(&a[1], 2) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);

				DQN_ASSERT(DqnStr_Reverse(a, 0) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "aba") == 0);
			}

			// Try reverse empty string
			{
				char a[] = "";
				DQN_ASSERT(DqnStr_Reverse(a, DqnStr_Len(a)) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "") == 0);
			}

			// Try reverse single char string
			{
				char a[] = "a";
				DQN_ASSERT(DqnStr_Reverse(a, DqnStr_Len(a)) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);

				DQN_ASSERT(DqnStr_Reverse(a, 0) == true);
				DQN_ASSERT(DqnStr_Cmp(a, "a") == 0);
			}

			printf(
			    "StringsTest(): StrReverse: Completed successfully\n");
		}

		// const u64 LARGEST_NUM  = (u64)-1;
		const i64 SMALLEST_NUM = LLONG_MIN;
		// StrToI64
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

	        printf("StringsTest(): StrToI64: Completed successfully\n");
		}

		// i64 to str
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

			char e[DQN_64BIT_NUM_MAX_STR_SIZE] = {};
			Dqn_I64ToStr(SMALLEST_NUM, e, DQN_ARRAY_COUNT(e));
			DQN_ASSERT(DqnStr_Cmp(e, "-9223372036854775808") == 0);

			printf("StringsTest(): I64ToStr: Completed successfully\n");
		}
	}

	// StrToF32
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
		printf("StringsTest(): StrToF32: Completed successfully\n");
	}

	{

		{
		    const char *const a  = "Microsoft";
		    const char *const b  = "icro";
		    i32 lenA = DqnStr_Len(a);
		    i32 lenB = DqnStr_Len(b);
		    DQN_ASSERT(DqnStr_HasSubstring(a, lenA, b, lenB) == true);
		    DQN_ASSERT(DqnStr_HasSubstring(a, lenA, "iro",
		                                     DqnStr_Len("iro")) == false);
		    DQN_ASSERT(DqnStr_HasSubstring(b, lenB, a, lenA) == false);
		    DQN_ASSERT(DqnStr_HasSubstring("iro", DqnStr_Len("iro"), a,
		                                     lenA) == false);
		    DQN_ASSERT(DqnStr_HasSubstring("", 0, "iro", 4) == false);
		    DQN_ASSERT(DqnStr_HasSubstring("", 0, "", 0) == false);
		    DQN_ASSERT(DqnStr_HasSubstring(NULL, 0, NULL, 0) == false);
	    }

		{
		    const char *const a  = "Micro";
		    const char *const b  = "irob";
		    i32 lenA = DqnStr_Len(a);
		    i32 lenB = DqnStr_Len(b);
		    DQN_ASSERT(DqnStr_HasSubstring(a, lenA, b, lenB) == false);
		    DQN_ASSERT(DqnStr_HasSubstring(b, lenB, a, lenA) == false);
	    }

	    printf("StringsTest(): StrHasSubstring: Completed successfully\n");
    }

    // UCS <-> UTF8 Checks
    {
	    // Test ascii characters
	    {
		    u32 codepoint = '@';
		    u32 string[1] = {};

		    u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 1);
		    DQN_ASSERT(string[0] == '@');

		    bytesUsed = Dqn_UTF8ToUCS(&string[0], codepoint);
		    DQN_ASSERT(string[0] >= 0 && string[0] < 0x80);
		    DQN_ASSERT(bytesUsed == 1);
	    }

	    // Test 2 byte characters
		{
		    u32 codepoint = 0x278;
		    u32 string[1] = {};

		    u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 2);
		    DQN_ASSERT(string[0] == 0xC9B8);

		    bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 2);
	    }

	    // Test 3 byte characters
		{
		    u32 codepoint = 0x0A0A;
		    u32 string[1] = {};

			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 3);
			DQN_ASSERT(string[0] == 0xE0A88A);

		    bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 3);
	    }

	    // Test 4 byte characters
		{
		    u32 codepoint = 0x10912;
		    u32 string[1] = {};
			u32 bytesUsed = Dqn_UCSToUTF8(&string[0], codepoint);

		    DQN_ASSERT(bytesUsed == 4);
		    DQN_ASSERT(string[0] == 0xF090A492);

		    bytesUsed = Dqn_UTF8ToUCS(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 4);
	    }

		{
		    u32 codepoint = 0x10912;
			u32 bytesUsed = Dqn_UCSToUTF8(NULL, codepoint);
		    DQN_ASSERT(bytesUsed == 0);

		    bytesUsed = Dqn_UTF8ToUCS(NULL, codepoint);
		    DQN_ASSERT(bytesUsed == 0);
	    }

	    printf("StringsTest(): ucs <-> utf8: Completed successfully\n");
    }

    printf("StringsTest(): Completed successfully\n");
}

#ifdef DQN_WIN32_IMPLEMENTATION
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
void OtherTest()
{
	{ // Test Win32 Sleep
		// NOTE: Win32 Sleep is not granular to a certain point so sleep excessively
		u32 sleepInMs = 1000;
		f64 startInMs = DqnTime_NowInMs();
		Sleep(sleepInMs);
		f64 endInMs = DqnTime_NowInMs();

		DQN_ASSERT(startInMs < endInMs);
		printf("OtherTest(): TimeNow: Completed successfully\n");
	}
	printf("OtherTest(): Completed successfully\n");
}
#endif

void RandomTest() {

	DqnRandPCGState pcg;
	DqnRnd_PCGInit(&pcg);
	for (i32 i = 0; i < 10; i++)
	{
		i32 min    = -100;
		i32 max    = 100000;
		i32 result = DqnRnd_PCGRange(&pcg, min, max);
		DQN_ASSERT(result >= min && result <= max);

		f32 randF32 = DqnRnd_PCGNextf(&pcg);
		DQN_ASSERT(randF32 >= 0.0f && randF32 <= 1.0f);
		printf("RandomTest(): RndPCG: Completed successfully\n");
	}

	printf("RandomTest(): Completed successfully\n");
}

void MathTest()
{
	{ // Lerp
		{
			f32 start = 10;
			f32 t     = 0.5f;
			f32 end   = 20;
			DQN_ASSERT(DqnMath_Lerp(start, t, end) == 15);
		}

		{
			f32 start = 10;
			f32 t     = 2.0f;
			f32 end   = 20;
			DQN_ASSERT(DqnMath_Lerp(start, t, end) == 30);
		}

		printf("MathTest(): Lerp: Completed successfully\n");
	}

	{ // sqrtf
		DQN_ASSERT(DqnMath_Sqrtf(4.0f) == 2.0f);
		printf("MathTest(): Sqrtf: Completed successfully\n");
	}

	printf("MathTest(): Completed successfully\n");
}

void VecTest()
{
	{ // V2

		// V2 Creating
		{
			DqnV2 vec = DqnV2_2f(5.5f, 5.0f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 5.5f && vec.h == 5.0f);
		}

		// V2 with 2 integers
		{
			DqnV2 vec = DqnV2_2i(3, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 3 && vec.h == 5.0f);
		}
		
		// V2 Arithmetic
		{
			DqnV2 vecA = DqnV2_2f(5, 10);
			DqnV2 vecB = DqnV2_2i(2, 3);
			DQN_ASSERT(DqnV2_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV2_Equals(vecA, DqnV2_2f(5, 10)) == true);
			DQN_ASSERT(DqnV2_Equals(vecB, DqnV2_2f(2, 3)) == true);

			DqnV2 result = DqnV2_Add(vecA, DqnV2_2f(5, 10));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_2f(10, 20)) == true);

			result = DqnV2_Sub(result, DqnV2_2f(5, 10));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_2f(5, 10)) == true);

			result = DqnV2_Scalef(result, 5);
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_2f(25, 50)) == true);

			result = DqnV2_Hadamard(result, DqnV2_2f(10, 0.5f));
			DQN_ASSERT(DqnV2_Equals(result, DqnV2_2f(250, 25)) == true);

			f32 dotResult = DqnV2_Dot(DqnV2_2f(5, 10), DqnV2_2f(3, 4));
			DQN_ASSERT(dotResult == 55);
		}

		// Test operator overloading
		{
			DqnV2 vecA = DqnV2_2f(5, 10);
			DqnV2 vecB = DqnV2_2i(2, 3);
			DQN_ASSERT((vecA == vecB) == false);
			DQN_ASSERT((vecA == DqnV2_2f(5, 10)) == true);
			DQN_ASSERT((vecB == DqnV2_2f(2, 3)) == true);

			DqnV2 result = vecA + DqnV2_2f(5, 10);
			DQN_ASSERT((result == DqnV2_2f(10, 20)) == true);

			result -= DqnV2_2f(5, 10);
			DQN_ASSERT((result == DqnV2_2f(5, 10)) == true);

			result *= 5;
			DQN_ASSERT((result == DqnV2_2f(25, 50)) == true);

			result = result * DqnV2_2f(10, 0.5f);
			DQN_ASSERT((result == DqnV2_2f(250, 25)) == true);

			result += DqnV2_2f(1, 1);
			DQN_ASSERT((result == DqnV2_2f(251, 26)) == true);

			result = result - DqnV2_2f(1, 1);
			DQN_ASSERT((result == DqnV2_2f(250, 25)) == true);
		}

		// V2 Properties
		{
			const f32 EPSILON = 0.001f;
			DqnV2 a = DqnV2_2f(0, 0);
			DqnV2 b = DqnV2_2f(3, 4);

			f32 lengthSq = DqnV2_LengthSquared(a, b);
			DQN_ASSERT(lengthSq == 25);

			f32 length = DqnV2_Length(a, b);
			DQN_ASSERT(length == 5);

			DqnV2 normalised = DqnV2_Normalise(b);
			f32 normX        = b.x / 5.0f;
			f32 normY        = b.y / 5.0f;
			f32 diffNormX = normalised.x - normX;
			f32 diffNormY = normalised.y - normY;
			DQN_ASSERT_MSG(diffNormX < EPSILON, "normalised.x: %f, normX: %f\n", normalised.x, normX);
			DQN_ASSERT_MSG(diffNormY < EPSILON, "normalised.y: %f, normY: %f\n", normalised.y, normY);

			DqnV2 c = DqnV2_2f(3.5f, 8.0f);
			DQN_ASSERT(DqnV2_Overlaps(b, c) == true);
			DQN_ASSERT(DqnV2_Overlaps(b, a) == false);

			DqnV2 d = DqnV2_Perpendicular(c);
			DQN_ASSERT(DqnV2_Dot(c, d) == 0);
		}

		{ // constrain_to_ratio
			DqnV2 ratio  = DqnV2_2f(16, 9);
			DqnV2 dim    = DqnV2_2f(2000, 1080);
			DqnV2 result = DqnV2_ConstrainToRatio(dim, ratio);
			DQN_ASSERT(result.w == 1920 && result.h == 1080);
		}

		printf("VecTest(): Vec2: Completed successfully\n");
	}

	{ // V3

		// V3i Creating
		{
			DqnV3 vec = DqnV3_3f(5.5f, 5.0f, 5.875f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f);
		}

		// V3i Creating
		{
			DqnV3 vec = DqnV3_3i(3, 4, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5);
			DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5);
		}

		// V3 Arithmetic
		{
			DqnV3 vecA = DqnV3_3f(5, 10, 15);
			DqnV3 vecB = DqnV3_3f(2, 3, 6);
			DQN_ASSERT(DqnV3_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV3_Equals(vecA, DqnV3_3f(5, 10, 15)) == true);
			DQN_ASSERT(DqnV3_Equals(vecB, DqnV3_3f(2, 3, 6)) == true);

			DqnV3 result = DqnV3_Add(vecA, DqnV3_3f(5, 10, 15));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_3f(10, 20, 30)) == true);

			result = DqnV3_Sub(result, DqnV3_3f(5, 10, 15));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_3f(5, 10, 15)) == true);

			result = DqnV3_Scalef(result, 5);
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_3f(25, 50, 75)) == true);

			result = DqnV3_Hadamard(result, DqnV3_3f(10.0f, 0.5f, 10.0f));
			DQN_ASSERT(DqnV3_Equals(result, DqnV3_3f(250, 25, 750)) == true);

			f32 dotResult = DqnV3_Dot(DqnV3_3f(5, 10, 2), DqnV3_3f(3, 4, 6));
			DQN_ASSERT(dotResult == 67);

			DqnV3 cross = DqnV3_Cross(vecA, vecB);
			DQN_ASSERT(DqnV3_Equals(cross, DqnV3_3f(15, 0, -5)) == true);
		}

		{
			DqnV3 vecA = DqnV3_3f(5, 10, 15);
			DqnV3 vecB = DqnV3_3f(2, 3, 6);
			DQN_ASSERT((vecA ==  vecB) == false);
			DQN_ASSERT((vecA ==  DqnV3_3f(5, 10, 15)) == true);
			DQN_ASSERT((vecB ==  DqnV3_3f(2, 3, 6)) == true);

			DqnV3 result = vecA + DqnV3_3f(5, 10, 15);
			DQN_ASSERT((result == DqnV3_3f(10, 20, 30)) == true);

			result -= DqnV3_3f(5, 10, 15);
			DQN_ASSERT((result == DqnV3_3f(5, 10, 15)) == true);

			result = result * 5;
			DQN_ASSERT((result == DqnV3_3f(25, 50, 75)) == true);

			result *= DqnV3_3f(10.0f, 0.5f, 10.0f);
			DQN_ASSERT((result == DqnV3_3f(250, 25, 750)) == true);

			result = result - DqnV3_3f(1, 1, 1);
			DQN_ASSERT((result == DqnV3_3f(249, 24, 749)) == true);

			result += DqnV3_3f(1, 1, 1);
			DQN_ASSERT((result == DqnV3_3f(250, 25, 750)) == true);
		}

		printf("VecTest(): Vec3: Completed successfully\n");
	}

	{ // V4

		// V4 Creating
		{
			DqnV4 vec = DqnV4_4f(5.5f, 5.0f, 5.875f, 5.928f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f && vec.w == 5.928f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f && vec.a == 5.928f);
		}

		// V4i Creating
		{
			DqnV4 vec = DqnV4_4i(3, 4, 5, 6);
		    DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5 && vec.w == 6);
		    DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5 && vec.a == 6);
	    }

		// V4 Arithmetic
		{
			DqnV4 vecA = DqnV4_4f(5, 10, 15, 20);
			DqnV4 vecB = DqnV4_4i(2, 3, 6, 8);
			DQN_ASSERT(DqnV4_Equals(vecA, vecB) == false);
			DQN_ASSERT(DqnV4_Equals(vecA, DqnV4_4f(5, 10, 15, 20)) == true);
			DQN_ASSERT(DqnV4_Equals(vecB, DqnV4_4f(2, 3, 6, 8)) == true);

			DqnV4 result = DqnV4_Add(vecA, DqnV4_4f(5, 10, 15, 20));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_4f(10, 20, 30, 40)) == true);

			result = DqnV4_Sub(result, DqnV4_4f(5, 10, 15, 20));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_4f(5, 10, 15, 20)) == true);

			result = DqnV4_Scalef(result, 5);
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_4f(25, 50, 75, 100)) == true);

			result = DqnV4_Hadamard(result, DqnV4_4f(10, 0.5f, 10, 0.25f));
			DQN_ASSERT(DqnV4_Equals(result, DqnV4_4f(250, 25, 750, 25)) == true);

			f32 dotResult = DqnV4_Dot(DqnV4_4f(5, 10, 2, 8), DqnV4_4f(3, 4, 6, 5));
			DQN_ASSERT(dotResult == 107);
		}

		{
			DqnV4 vecA = DqnV4_4f(5, 10, 15, 20);
			DqnV4 vecB = DqnV4_4i(2, 3, 6, 8);
			DQN_ASSERT((vecA == vecB) == false);
			DQN_ASSERT((vecA == DqnV4_4f(5, 10, 15, 20)) == true);
			DQN_ASSERT((vecB == DqnV4_4f(2, 3, 6, 8)) == true);

			DqnV4 result = vecA + DqnV4_4f(5, 10, 15, 20);
			DQN_ASSERT((result == DqnV4_4f(10, 20, 30, 40)) == true);

			result = result - DqnV4_4f(5, 10, 15, 20);
			DQN_ASSERT((result == DqnV4_4f(5, 10, 15, 20)) == true);

			result = result * 5;
			DQN_ASSERT((result == DqnV4_4f(25, 50, 75, 100)) == true);

			result *= DqnV4_4f(10, 0.5f, 10, 0.25f);
			DQN_ASSERT((result == DqnV4_4f(250, 25, 750, 25)) == true);

			result += DqnV4_4f(1, 1, 1, 1);
			DQN_ASSERT((result == DqnV4_4f(251, 26, 751, 26)) == true);

			result -= DqnV4_4f(1, 1, 1, 1);
			DQN_ASSERT((result == DqnV4_4f(250, 25, 750, 25)) == true);
		}

		printf("VecTest(): Vec4: Completed successfully\n");
	}

	// Rect
	{
		// Test rect init functions
		{
			DqnRect rect4f = DqnRect_4f(1.1f, 2.2f, 3.3f, 4.4f);
			DqnRect rect4i = DqnRect_4i(1, 2, 3, 4);

			DQN_ASSERT(rect4i.min.x == 1 && rect4i.min.y == 2);
			DQN_ASSERT(rect4i.max.x == 3 && rect4i.max.y == 4);
			DQN_ASSERT(rect4f.min.x == 1.1f && rect4f.min.y == 2.2f);
			DQN_ASSERT(rect4f.max.x == 3.3f && rect4f.max.y == 4.4f);

			DqnRect rect = DqnRect_Init(DqnV2_2f(-10, -10), DqnV2_2f(20, 20));
			DQN_ASSERT(DqnV2_Equals(rect.min, DqnV2_2f(-10, -10)));
			DQN_ASSERT(DqnV2_Equals(rect.max, DqnV2_2f(10, 10)));
		}

		// Test rect get size function
		{
			// Test float rect
			{
				DqnRect rect =
				    DqnRect_Init(DqnV2_2f(-10, -10), DqnV2_2f(20, 20));

				f32 width, height;
				DqnRect_GetSize2f(rect, &width, &height);
				DQN_ASSERT(width == 20);
				DQN_ASSERT(height == 20);

				DqnV2 dim = DqnRect_GetSizeV2(rect);
				DQN_ASSERT(DqnV2_Equals(dim, DqnV2_2f(20, 20)));
			}

			// Test rect with float values and GetSize as 2 integers
			{
				DqnRect rect = DqnRect_Init(DqnV2_2f(-10.5f, -10.5f),
				                            DqnV2_2f(20.5f, 20.5f));
				i32 width, height;
				DqnRect_GetSize2i(rect, &width, &height);
				DQN_ASSERT(width == 20);
				DQN_ASSERT(height == 20);
			}
		}

		// Test rect get centre
		DqnRect rect     = DqnRect_Init(DqnV2_2f(-10, -10), DqnV2_2f(20, 20));
		DqnV2 rectCenter = DqnRect_GetCentre(rect);
		DQN_ASSERT(DqnV2_Equals(rectCenter, DqnV2_2f(0, 0)));

		// Test clipping rect get centre
		DqnRect clipRect   = DqnRect_4i(-15, -15, 10, 10);
		DqnRect clipResult = DqnRect_ClipRect(rect, clipRect);
		DQN_ASSERT(clipResult.min.x == -10 && clipResult.min.y == -10);
		DQN_ASSERT(clipResult.max.x == 10 && clipResult.max.y == 10);

		// Test shifting rect
		{
			DqnRect shiftedRect = DqnRect_Move(rect, DqnV2_2f(10, 0));
			DQN_ASSERT(DqnV2_Equals(shiftedRect.min, DqnV2_2f(0, -10)));
			DQN_ASSERT(DqnV2_Equals(shiftedRect.max, DqnV2_2f(20, 10)));

			// Ensure dimensions have remained the same
			{
				f32 width, height;
				DqnRect_GetSize2f(shiftedRect, &width, &height);
				DQN_ASSERT(width == 20);
				DQN_ASSERT(height == 20);

				DqnV2 dim = DqnRect_GetSizeV2(shiftedRect);
				DQN_ASSERT(DqnV2_Equals(dim, DqnV2_2f(20, 20)));
			}

			// Test rect contains p
			{
				DqnV2 inP  = DqnV2_2f(5, 5);
				DqnV2 outP = DqnV2_2f(100, 100);
				DQN_ASSERT(DqnRect_ContainsP(shiftedRect, inP));
				DQN_ASSERT(!DqnRect_ContainsP(shiftedRect, outP));
			}
		}

		printf("VecTest(): Rect: Completed successfully\n");
	}

	printf("VecTest(): Completed successfully\n");
}

void ArrayTestMemAPIInternal(DqnArray<DqnV2> *array, DqnMemAPI memAPI)
{
	{
		DQN_ASSERT(DqnArray_Init(array, 1, memAPI));
		DQN_ASSERT(array->capacity == 1);
		DQN_ASSERT(array->count == 0);

		// Test basic insert
		{
			DqnV2 va = DqnV2_2f(5, 10);
			DQN_ASSERT(DqnArray_Push(array, va));

			DqnV2 vb = array->data[0];
			DQN_ASSERT(DqnV2_Equals(va, vb));

			DQN_ASSERT(array->capacity == 1);
			DQN_ASSERT(array->count == 1);
		}

		// Test array resizing and freeing
		{
			DqnV2 va = DqnV2_2f(10, 15);
			DQN_ASSERT(DqnArray_Push(array, va));

			DqnV2 vb = array->data[0];
			DQN_ASSERT(DqnV2_Equals(va, vb) == false);

			vb = array->data[1];
			DQN_ASSERT(DqnV2_Equals(va, vb) == true);

			DQN_ASSERT(array->capacity == 2);
			DQN_ASSERT(array->count == 2);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 3);
			DQN_ASSERT(array->count == 3);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 4);
			DQN_ASSERT(array->count == 4);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 5);
			DQN_ASSERT(array->count == 5);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 6);
			DQN_ASSERT(array->count == 6);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 7);
			DQN_ASSERT(array->count == 7);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 8);
			DQN_ASSERT(array->count == 8);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 9);
			DQN_ASSERT(array->count == 9);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 10);
			DQN_ASSERT(array->count == 10);

			DQN_ASSERT(DqnArray_Push(array, va));
			DQN_ASSERT(array->capacity == 12);
			DQN_ASSERT(array->count == 11);

			DqnV2 vc = DqnV2_2f(90, 100);
			DQN_ASSERT(DqnArray_Push(array, vc));
			DQN_ASSERT(array->capacity == 12);
			DQN_ASSERT(array->count == 12);
			DQN_ASSERT(DqnV2_Equals(vc, array->data[11]));
		}
	}
	DQN_ASSERT(DqnArray_Free(array));

	{
		DQN_ASSERT(DqnArray_Init(array, 1, memAPI));
		DQN_ASSERT(array->capacity == 1);
		DQN_ASSERT(array->count == 0);
	}
	DQN_ASSERT(DqnArray_Free(array));

	{
		DqnV2 a = DqnV2_2f(1, 2);
		DqnV2 b = DqnV2_2f(3, 4);
		DqnV2 c = DqnV2_2f(5, 6);
		DqnV2 d = DqnV2_2f(7, 8);

		DQN_ASSERT(DqnArray_Init(array, 16, memAPI));
		DQN_ASSERT(DqnArray_Remove(array, 0) == false);
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 0);

		DQN_ASSERT(DqnArray_Clear(array));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 0);

		DQN_ASSERT(DqnArray_Push(array, a));
		DQN_ASSERT(DqnArray_Push(array, b));
		DQN_ASSERT(DqnArray_Push(array, c));
		DQN_ASSERT(DqnArray_Push(array, d));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 4);

		DQN_ASSERT(DqnArray_Remove(array, 0));
		DQN_ASSERT(DqnV2_Equals(array->data[0], d));
		DQN_ASSERT(DqnV2_Equals(array->data[1], b));
		DQN_ASSERT(DqnV2_Equals(array->data[2], c));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 3);

		DQN_ASSERT(DqnArray_Remove(array, 2));
		DQN_ASSERT(DqnV2_Equals(array->data[0], d));
		DQN_ASSERT(DqnV2_Equals(array->data[1], b));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 2);

		DQN_ASSERT(DqnArray_Remove(array, 100) == false);
		DQN_ASSERT(DqnV2_Equals(array->data[0], d));
		DQN_ASSERT(DqnV2_Equals(array->data[1], b));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 2);

		DQN_ASSERT(DqnArray_Clear(array));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 0);

	}
	DQN_ASSERT(DqnArray_Free(array));

	{
		DqnV2 a = DqnV2_2f(1, 2);
		DqnV2 b = DqnV2_2f(3, 4);
		DqnV2 c = DqnV2_2f(5, 6);
		DqnV2 d = DqnV2_2f(7, 8);

		DQN_ASSERT(DqnArray_Init(array, 16, memAPI));

		DQN_ASSERT(DqnArray_Push(array, a));
		DQN_ASSERT(DqnArray_Push(array, b));
		DQN_ASSERT(DqnArray_Push(array, c));
		DQN_ASSERT(DqnArray_Push(array, d));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 4);

		DqnArray_RemoveStable(array, 0);
		DQN_ASSERT(DqnV2_Equals(array->data[0], b));
		DQN_ASSERT(DqnV2_Equals(array->data[1], c));
		DQN_ASSERT(DqnV2_Equals(array->data[2], d));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 3);

		DqnArray_RemoveStable(array, 1);
		DQN_ASSERT(DqnV2_Equals(array->data[0], b));
		DQN_ASSERT(DqnV2_Equals(array->data[1], d));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 2);

		DqnArray_RemoveStable(array, 1);
		DQN_ASSERT(DqnV2_Equals(array->data[0], b));
		DQN_ASSERT(array->capacity == 16);
		DQN_ASSERT(array->count == 1);
	}
	DQN_ASSERT(DqnArray_Free(array));
	printf("ArrayTestMemAPIInternal(): Completed successfully\n");

}

void ArrayTest()
{
	DqnArray<DqnV2> array = {};
	ArrayTestMemAPIInternal(&array, DqnMemAPI_DefaultUseCalloc());
	printf("ArrayTest(): Completed successfully\n");
}

void MemStackTest()
{
	// Test over allocation, alignments, temp regions
	{
		size_t allocSize    = DQN_KILOBYTE(1);
		DqnMemStack stack = {};
		const u32 ALIGNMENT = 4;
		DqnMemStack_Init(&stack, allocSize, false, ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Alocate A
		size_t sizeA    = (size_t)(allocSize * 0.5f);
		void *resultA   = DqnMemStack_Push(&stack, sizeA);
		u64 resultAddrA = *((u64 *)resultA);
		DQN_ASSERT(resultAddrA % ALIGNMENT == 0);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used >= sizeA + 0 &&
		           stack.block->used <= sizeA + 3);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(resultA);
		u8 *ptrA = (u8 *)resultA;
		for (u32 i  = 0; i < sizeA; i++)
			ptrA[i] = 1;

		DqnMemStackBlock *blockA = stack.block;
		// Alocate B
		size_t sizeB    = (size_t)(allocSize * 2.0f);
		void *resultB   = DqnMemStack_Push(&stack, sizeB);
		u64 resultAddrB = *((u64 *)resultB);
		DQN_ASSERT(resultAddrB % ALIGNMENT == 0);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == DQN_KILOBYTE(2));

		// Since we alignment the pointers we return they can be within 0-3
		// bytes of what we expect and since this is in a new block as well used
		// will reflect just this allocation.
		DQN_ASSERT(stack.block->used >= sizeB + 0 &&
		           stack.block->used <= sizeB + 3);
		DQN_ASSERT(resultB);
		u8 *ptrB = (u8 *)resultB;
		for (u32 i  = 0; i < sizeB; i++)
			ptrB[i] = 2;

		// Check that a new block was created since there wasn't enough space
		DQN_ASSERT(stack.block->prevBlock == blockA);
		DQN_ASSERT(stack.block != blockA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(blockA->used == sizeA);
		DqnMemStackBlock *blockB = stack.block;

		// Check temp regions work
		DqnMemStackTempRegion tempBuffer;
		DQN_ASSERT(DqnMemStackTempRegion_Begin(&tempBuffer, &stack));

		size_t sizeC             = 1024 + 1;
		void *resultC   = DqnMemStack_Push(tempBuffer.stack, sizeC);
		u64 resultAddrC = *((u64 *)resultC);
		DQN_ASSERT(resultAddrC % ALIGNMENT == 0);
		DQN_ASSERT(stack.block != blockB && stack.block != blockA);
		DQN_ASSERT(stack.block->used >= sizeC + 0 &&
		           stack.block->used <= sizeC + 3);
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
		DqnMemStackTempRegion_End(tempBuffer);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == sizeB);
		DQN_ASSERT(stack.block->used >= sizeB + 0 &&
		           stack.block->used <= sizeB + 3);
		DQN_ASSERT(stack.tempRegionCount == 0);
		DQN_ASSERT(resultB);

		DQN_ASSERT(stack.block->prevBlock == blockA);
		DQN_ASSERT(stack.block != blockA);
		DQN_ASSERT(blockA->used == sizeA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Release the last linked stack from the push stack
		DqnMemStack_FreeLastBlock(&stack);

		// Which should return back to the 1st allocation
		DQN_ASSERT(stack.block == blockA);
		DQN_ASSERT(stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == sizeA);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Free once more to release stack A memory
		DqnMemStack_FreeLastBlock(&stack);
		DQN_ASSERT(!stack.block);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
		DQN_ASSERT(stack.tempRegionCount == 0);
	}

	// Test stack with fixed memory does not allocate more
	{
		u8 memory[DQN_KILOBYTE(1)] = {};
		DqnMemStack stack        = {};
		const u32 ALIGNMENT        = 4;
		DqnMemStack_InitWithFixedMem(&stack, memory, DQN_ARRAY_COUNT(memory),
		                              ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size ==
		           DQN_ARRAY_COUNT(memory) - sizeof(DqnMemStackBlock));
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		// Allocation larger than stack mem size should fail
		DQN_ASSERT(!DqnMemStack_Push(&stack, DQN_ARRAY_COUNT(memory) * 2));

		// Check free does nothing
		DqnMemStack_Free(&stack);
		DqnMemStack_FreeLastBlock(&stack);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size ==
		           DQN_ARRAY_COUNT(memory) - sizeof(DqnMemStackBlock));
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);
	}

	// Test stack with fixed size, allocates once from platform but does not
	// grow further
	{
		size_t allocSize    = DQN_KILOBYTE(1);
		DqnMemStack stack = {};
		const u32 ALIGNMENT = 4;
		DqnMemStack_InitWithFixedSize(&stack, allocSize, false, ALIGNMENT);
		DQN_ASSERT(stack.block && stack.block->memory);
		DQN_ASSERT(stack.block->size == allocSize);
		DQN_ASSERT(stack.block->used == 0);
		DQN_ASSERT(stack.byteAlign == ALIGNMENT);

		void *result = DqnMemStack_Push(&stack, (size_t)(0.5f * allocSize));
		DQN_ASSERT(result);

		// Allocating more should fail
		DQN_ASSERT(!DqnMemStack_Push(&stack, allocSize));

		// Freeing should work
		DqnMemStack_Free(&stack);
		DQN_ASSERT(!stack.block);
	}

	// Test freeing/clear block and alignment
	{
		size_t firstBlockSize = DQN_KILOBYTE(1);
		DqnMemStack stack = {};
		const u32 ALIGNMENT = 16;
		DqnMemStack_Init(&stack, firstBlockSize, false, ALIGNMENT);

		DqnMemStackBlock *firstBlock = stack.block;
		u8 *first                     = NULL;
		{
			u32 allocate40Bytes = 40;
			u8 *data = (u8 *)DqnMemStack_Push(&stack, allocate40Bytes);

			// Test that the allocation got aligned to 16 byte boundary
			DQN_ASSERT(data);
			DQN_ASSERT(stack.block->size == firstBlockSize);
			DQN_ASSERT((size_t)data % ALIGNMENT == 0);

			for (u32 i  = 0; i < allocate40Bytes; i++)
				data[i] = 'a';

			// Clear the block, but don't zero it out
			DqnMemStack_ClearCurrBlock(&stack, false);
			for (u32 i = 0; i < allocate40Bytes; i++)
				DQN_ASSERT(data[i] == 'a');

			// Test clear reverted the use pointer
			DQN_ASSERT(stack.block->used == 0);
			DQN_ASSERT(stack.block->size == firstBlockSize);

			// Reallocate the data
			data = (u8 *)DqnMemStack_Push(&stack, firstBlockSize);
			DQN_ASSERT(stack.block->size == firstBlockSize);
			DQN_ASSERT((size_t)data % ALIGNMENT == 0);

			// Fill with 'b's
			for (u32 i  = 0; i < firstBlockSize; i++)
				data[i] = 'b';

			// Clear block and zero it out
			DqnMemStack_ClearCurrBlock(&stack, true);
			for (u32 i = 0; i < firstBlockSize; i++)
				DQN_ASSERT(data[i] == 0);

			// General Check stack struct contains the values we expect from
			// initialisation
			DQN_ASSERT(stack.flags == 0);
			DQN_ASSERT(stack.tempRegionCount == 0);
			DQN_ASSERT(stack.byteAlign == ALIGNMENT);
			DQN_ASSERT(stack.block->size == firstBlockSize);

			// Write out data to current block
			data = (u8 *)DqnMemStack_Push(&stack, firstBlockSize);
			for (u32 i  = 0; i < firstBlockSize; i++)
				data[i] = 'c';

			first = data;
		}

		// Force it to allocate three new blocks and write out data to each
		size_t secondBlockSize = DQN_KILOBYTE(2);
		u8 *second = (u8 *)DqnMemStack_Push(&stack, secondBlockSize);
		DqnMemStackBlock *secondBlock = stack.block;
		for (u32 i  = 0; i < secondBlockSize; i++)
			second[i] = 'd';

		size_t thirdBlockSize = DQN_KILOBYTE(3);
		u8 *third = (u8 *)DqnMemStack_Push(&stack, thirdBlockSize);
		DqnMemStackBlock *thirdBlock = stack.block;
		for (u32 i  = 0; i < thirdBlockSize; i++)
			third[i] = 'e';

		size_t fourthBlockSize = DQN_KILOBYTE(4);
		u8 *fourth = (u8 *)DqnMemStack_Push(&stack, fourthBlockSize);
		DqnMemStackBlock *fourthBlock = stack.block;
		for (u32 i  = 0; i < fourthBlockSize; i++)
			fourth[i] = 'f';

		DQN_ASSERT((firstBlock != secondBlock) && (secondBlock != thirdBlock) && (thirdBlock != fourthBlock));
		DQN_ASSERT(firstBlock->prevBlock  == NULL);
		DQN_ASSERT(secondBlock->prevBlock == firstBlock);
		DQN_ASSERT(thirdBlock->prevBlock  == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		// NOTE: Making blocks manually is not really recommended ..
		// Try and free an invalid block by mocking a fake block
		u8 fakeBlockMem[DQN_KILOBYTE(3)] = {};
		DqnMemStackBlock fakeBlock   = {};
		fakeBlock.memory              = fakeBlockMem;
		fakeBlock.size                = DQN_ARRAY_COUNT(fakeBlockMem);
		fakeBlock.used                = 0;
		DQN_ASSERT(!DqnMemStack_FreeStackBlock(&stack, &fakeBlock));

		//Ensure that the actual blocks are still valid and freeing did nothing
		DQN_ASSERT(firstBlock->size  == firstBlockSize);
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(thirdBlock->size  == thirdBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(firstBlock->used  == firstBlockSize);
		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(thirdBlock->used  == thirdBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT((firstBlock != secondBlock) && (secondBlock != thirdBlock) && (thirdBlock != fourthBlock));
		DQN_ASSERT(firstBlock->prevBlock  == NULL);
		DQN_ASSERT(secondBlock->prevBlock == firstBlock);
		DQN_ASSERT(thirdBlock->prevBlock  == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		for (u32 i  = 0; i < firstBlockSize; i++)
			DQN_ASSERT(first[i] == 'c');

		for (u32 i  = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');
		
		for (u32 i  = 0; i < thirdBlockSize; i++)
			DQN_ASSERT(third[i] == 'e');

		for (u32 i  = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the first block
		DqnMemStack_FreeStackBlock(&stack, firstBlock);

		// Revalidate state
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(thirdBlock->size  == thirdBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(thirdBlock->used  == thirdBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT((secondBlock != thirdBlock) && (thirdBlock != fourthBlock));
		DQN_ASSERT(secondBlock->prevBlock == NULL);
		DQN_ASSERT(thirdBlock->prevBlock == secondBlock);
		DQN_ASSERT(fourthBlock->prevBlock == thirdBlock);

		for (u32 i  = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');
		
		for (u32 i  = 0; i < thirdBlockSize; i++)
			DQN_ASSERT(third[i] == 'e');

		for (u32 i  = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the third block
		DqnMemStack_FreeStackBlock(&stack, thirdBlock);

		// Revalidate state
		DQN_ASSERT(secondBlock->size == secondBlockSize);
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);

		DQN_ASSERT(secondBlock->used == secondBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);

		DQN_ASSERT(secondBlock != fourthBlock);
		DQN_ASSERT(secondBlock->prevBlock == NULL);
		DQN_ASSERT(fourthBlock->prevBlock == secondBlock);

		for (u32 i  = 0; i < secondBlockSize; i++)
			DQN_ASSERT(second[i] == 'd');
		
		for (u32 i  = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the second block
		DqnMemStack_FreeStackBlock(&stack, secondBlock);

		// Revalidate state
		DQN_ASSERT(fourthBlock->size == fourthBlockSize);
		DQN_ASSERT(fourthBlock->used == fourthBlockSize);
		DQN_ASSERT(fourthBlock->prevBlock == NULL);

		for (u32 i  = 0; i < fourthBlockSize; i++)
			DQN_ASSERT(fourth[i] == 'f');

		// Free the stack
		DqnMemStack_Free(&stack);
		DQN_ASSERT(!stack.block);
	}

	// Test pop
	{
		DqnMemStack stack = {};
		DqnMemStack_Init(&stack, DQN_KILOBYTE(1), true);

		size_t allocSize = 512;
		void *alloc = DqnMemStack_Push(&stack, allocSize);
		DQN_ASSERT(stack.block->used == allocSize);

		DQN_ASSERT(DqnMemStack_Pop(&stack, alloc, allocSize));
		DQN_ASSERT(stack.block->used == 0);
	}
}

#ifdef DQN_WIN32_IMPLEMENTATION

void FileTest()
{
	// File i/o
	{
		{
			DqnFile file = {};
			DQN_ASSERT(DqnFile_Open(
				".clang-format", &file,
				(DqnFilePermissionFlag_Write | DqnFilePermissionFlag_Read),
				DqnFileAction_OpenOnly));
			DQN_ASSERT(file.size == 1320);

			u8 *buffer = (u8 *)calloc(1, (size_t)file.size * sizeof(u8));
			DQN_ASSERT(DqnFile_Read(file, buffer, (u32)file.size) == file.size);
			free(buffer);

			DqnFile_Close(&file);
	        DQN_ASSERT(!file.handle && file.size == 0 &&
	                   file.permissionFlags == 0);
        }

		{
			DqnFile file = {};
			DQN_ASSERT(!DqnFile_Open(
				"asdljasdnel;kajdf", &file,
				(DqnFilePermissionFlag_Write | DqnFilePermissionFlag_Read),
				DqnFileAction_OpenOnly));
			DQN_ASSERT(file.size == 0);
			DQN_ASSERT(file.permissionFlags == 0);
			DQN_ASSERT(!file.handle);
			printf("FileTest(): FileIO: Completed successfully\n");
		}
	}

	{
		u32 numFiles;
		char **filelist = DqnDir_Read("*", &numFiles);
		printf("FileTest(): DirRead: Display read files\n");

		for (u32 i = 0; i < numFiles; i++)
			printf("FileTest(): DirRead: %s\n", filelist[i]);

		DqnDir_ReadFree(filelist, numFiles);
		printf("FileTest(): DirRead: Completed successfully\n");
	}

	printf("FileTest(): Completed successfully\n");
}

FILE_SCOPE u32 volatile globalDebugCounter;
FILE_SCOPE bool volatile globalDebugCounterMemoize[2048];
FILE_SCOPE DqnLock globalJobQueueLock;
FILE_SCOPE void JobQueueDebugCallbackIncrementCounter(DqnJobQueue *const queue,
                                                      void *const userData)
{
	DqnLock_Acquire(&globalJobQueueLock);
	DQN_ASSERT(!globalDebugCounterMemoize[globalDebugCounter]);
	globalDebugCounterMemoize[globalDebugCounter] = true;
	globalDebugCounter++;
	u32 number = globalDebugCounter;
	DqnLock_Release(&globalJobQueueLock);

	printf("JobQueueDebugCallbackIncrementCounter(): Thread %d: Incrementing Number: %d\n", GetCurrentThreadId(),
	       number);
}

FILE_SCOPE void JobQueueTest()
{
	size_t requiredSize;
	const i32 QUEUE_SIZE = 256;
	DqnJobQueue_InitWithMem(NULL, &requiredSize, QUEUE_SIZE, 0);

	DqnMemStack memStack;
	DQN_ASSERT_HARD(DqnMemStack_Init(&memStack, requiredSize, true));

	i32 numThreads, numCores;
	DqnWin32_GetNumThreadsAndCores(&numCores, &numThreads);
	DQN_ASSERT(numThreads > 0 && numCores > 0);
	i32 totalThreads = (numCores - 1) * numThreads;

	void *jobQueueMem = DqnMemStack_Push(&memStack, requiredSize);
	DQN_ASSERT_HARD(jobQueueMem);
	DqnJobQueue *jobQueue = DqnJobQueue_InitWithMem(jobQueueMem, &requiredSize,
	                                                QUEUE_SIZE, totalThreads);
	DQN_ASSERT_HARD(jobQueue);

	DQN_ASSERT(DqnLock_Init(&globalJobQueueLock));
	for (i32 i = 0; i < DQN_ARRAY_COUNT(globalDebugCounterMemoize); i++)
	{
		DqnJob job   = {};
		job.callback = JobQueueDebugCallbackIncrementCounter;
		while (!DqnJobQueue_AddJob(jobQueue, job))
		{
			DqnJobQueue_TryExecuteNextJob(jobQueue);
		}
	}

	while (DqnJobQueue_TryExecuteNextJob(jobQueue))
		;

	for (i32 i = 0; i < DQN_ARRAY_COUNT(globalDebugCounterMemoize); i++)
		DQN_ASSERT(globalDebugCounterMemoize[i]);

	printf("\nJobQueueTest(): Final incremented value: %d\n", globalDebugCounter);
	DQN_ASSERT(globalDebugCounter == DQN_ARRAY_COUNT(globalDebugCounterMemoize));
}
#endif

int main(void)
{
	StringsTest();
	RandomTest();
	MathTest();
	HandmadeMathTest();
	VecTest();
	ArrayTest();
	MemStackTest();

#ifdef DQN_WIN32_IMPLEMENTATION
	OtherTest();
	FileTest();
	JobQueueTest();
#endif

	printf("\nPress 'Enter' Key to Exit\n");
	getchar();

	return 0;
}

