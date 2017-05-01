#define DQN_WIN32_IMPLEMENTATION
#define DQN_IMPLEMENTATION
#include "dqn.h"

#include <stdio.h>
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
			char *a = "str_a";

			// Check simple compares
			{
				DQN_ASSERT(Dqn_strcmp(a, "str_a") == +0);
				DQN_ASSERT(Dqn_strcmp(a, "str_b") == -1);
				DQN_ASSERT(Dqn_strcmp("str_b", a) == +1);
				DQN_ASSERT(Dqn_strcmp(a, "") == +1);
				DQN_ASSERT(Dqn_strcmp("", "") == 0);

				// NOTE: Check that the string has not been trashed.
				DQN_ASSERT(Dqn_strcmp(a, "str_a") == +0);
			}

			// Check ops against null
			{
				DQN_ASSERT(Dqn_strcmp(NULL, NULL) != +0);
				DQN_ASSERT(Dqn_strcmp(a, NULL) != +0);
				DQN_ASSERT(Dqn_strcmp(NULL, a) != +0);
			}

			printf("StringsTest(): strcmp: Completed successfully\n");
		}

		// strlen
		{
			char *a = "str_a";
			DQN_ASSERT(Dqn_strlen(a) == 5);
			DQN_ASSERT(Dqn_strlen("") == 0);
			DQN_ASSERT(Dqn_strlen("   a  ") == 6);
			DQN_ASSERT(Dqn_strlen("a\n") == 2);

			// NOTE: Check that the string has not been trashed.
			DQN_ASSERT(Dqn_strcmp(a, "str_a") == 0);

			DQN_ASSERT(Dqn_strlen(NULL) == 0);

			printf("StringsTest(): strlen: Completed successfully\n");
		}

		// strncpy
		{
			{
				char *a    = "str_a";
				char b[10] = {};
				// Check copy into empty array
				{
					char *result = Dqn_strncpy(b, a, Dqn_strlen(a));
					DQN_ASSERT(Dqn_strcmp(b, "str_a") == 0);
					DQN_ASSERT(Dqn_strcmp(a, "str_a") == 0);
					DQN_ASSERT(Dqn_strcmp(result, "str_a") == 0);
					DQN_ASSERT(Dqn_strlen(result) == 5);
				}

				// Check copy into array offset, overlap with old results
				{
					char *newResult = Dqn_strncpy(&b[1], a, Dqn_strlen(a));
					DQN_ASSERT(Dqn_strcmp(newResult, "str_a") == 0);
					DQN_ASSERT(Dqn_strlen(newResult) == 5);

					DQN_ASSERT(Dqn_strcmp(a, "str_a") == 0);
					DQN_ASSERT(Dqn_strlen(a) == 5);

					DQN_ASSERT(Dqn_strcmp(b, "sstr_a") == 0);
					DQN_ASSERT(Dqn_strlen(b) == 6);
				}
			}

			// Check strncpy with NULL pointers
			{
				DQN_ASSERT(Dqn_strncpy(NULL, NULL, 5) == NULL);

				char *a      = "str";
				char *result = Dqn_strncpy(a, NULL, 5);

				DQN_ASSERT(Dqn_strcmp(a, "str") == 0);
				DQN_ASSERT(Dqn_strcmp(result, "str") == 0);
				DQN_ASSERT(Dqn_strcmp(result, a) == 0);
			}

			// Check strncpy with 0 chars to copy
			{
				char *a = "str";
				char *b = "ing";

				char *result = Dqn_strncpy(a, b, 0);
				DQN_ASSERT(Dqn_strcmp(a, "str") == 0);
				DQN_ASSERT(Dqn_strcmp(b, "ing") == 0);
				DQN_ASSERT(Dqn_strcmp(result, "str") == 0);
			}

			printf("StringsTest(): strncpy: Completed successfully\n");
		}

		// StrReverse
		{
			// Basic reverse operations
			{
				char a[] = "aba";
				DQN_ASSERT(Dqn_StrReverse(a, Dqn_strlen(a)) == true);
				DQN_ASSERT(Dqn_strcmp(a, "aba") == 0);

				DQN_ASSERT(Dqn_StrReverse(a, 2) == true);
				DQN_ASSERT(Dqn_strcmp(a, "baa") == 0);

				DQN_ASSERT(Dqn_StrReverse(a, Dqn_strlen(a)) == true);
				DQN_ASSERT(Dqn_strcmp(a, "aab") == 0);

				DQN_ASSERT(Dqn_StrReverse(&a[1], 2) == true);
				DQN_ASSERT(Dqn_strcmp(a, "aba") == 0);

				DQN_ASSERT(Dqn_StrReverse(a, 0) == true);
				DQN_ASSERT(Dqn_strcmp(a, "aba") == 0);
			}

			// Try reverse empty string
			{
				char a[] = "";
				DQN_ASSERT(Dqn_StrReverse(a, Dqn_strlen(a)) == true);
				DQN_ASSERT(Dqn_strcmp(a, "") == 0);
			}

			// Try reverse single char string
			{
				char a[] = "a";
				DQN_ASSERT(Dqn_StrReverse(a, Dqn_strlen(a)) == true);
				DQN_ASSERT(Dqn_strcmp(a, "a") == 0);

				DQN_ASSERT(Dqn_StrReverse(a, 0) == true);
				DQN_ASSERT(Dqn_strcmp(a, "a") == 0);
			}

			printf(
			    "StringsTest(): StrReverse: Completed successfully\n");
		}

		// StrToI32
		{
			char *a = "123";
			DQN_ASSERT(Dqn_StrToI32(a, Dqn_strlen(a)) == 123);

			char *b = "-123";
			DQN_ASSERT(Dqn_StrToI32(b, Dqn_strlen(b)) == -123);
			DQN_ASSERT(Dqn_StrToI32(b, 1) == 0);
			DQN_ASSERT(Dqn_StrToI32(&b[1], Dqn_strlen(&b[1])) == 123);

			char *c = "-0";
			DQN_ASSERT(Dqn_StrToI32(c, Dqn_strlen(c)) == 0);

			char *d = "+123";
			DQN_ASSERT(Dqn_StrToI32(d, Dqn_strlen(d)) == 123);
			DQN_ASSERT(Dqn_StrToI32(&d[1], Dqn_strlen(&d[1])) == 123);

			printf("StringsTest(): StrToI32: Completed successfully\n");
		}

		// i32_to_str
		{
			char a[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			Dqn_I32ToStr(+100, a, DQN_ARRAY_COUNT(a));
			DQN_ASSERT(Dqn_strcmp(a, "100") == 0);

			char b[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			Dqn_I32ToStr(-100, b, DQN_ARRAY_COUNT(b));
			DQN_ASSERT(Dqn_strcmp(b, "-100") == 0);

			char c[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			Dqn_I32ToStr(0, c, DQN_ARRAY_COUNT(c));
			DQN_ASSERT(Dqn_strcmp(c, "0") == 0);

			printf("StringsTest(): StrToI32: Completed successfully\n");
		}
	}

	{

		{
		    char *a  = "Microsoft";
		    char *b  = "icro";
		    i32 lenA = Dqn_strlen(a);
		    i32 lenB = Dqn_strlen(b);
		    DQN_ASSERT(Dqn_StrHasSubstring(a, lenA, b, lenB) == true);
		    DQN_ASSERT(Dqn_StrHasSubstring(a, lenA, "iro",
		                                     Dqn_strlen("iro")) == false);
		    DQN_ASSERT(Dqn_StrHasSubstring(b, lenB, a, lenA) == true);
		    DQN_ASSERT(Dqn_StrHasSubstring("iro", Dqn_strlen("iro"), a,
		                                     lenA) == false);
		    DQN_ASSERT(Dqn_StrHasSubstring("", 0, "iro", 4) == false);
		    DQN_ASSERT(Dqn_StrHasSubstring("", 0, "", 0) == false);
		    DQN_ASSERT(Dqn_StrHasSubstring(NULL, 0, NULL, 0) == false);
	    }

		{
		    char *a  = "Micro";
		    char *b  = "irob";
		    i32 lenA = Dqn_strlen(a);
		    i32 lenB = Dqn_strlen(b);
		    DQN_ASSERT(Dqn_StrHasSubstring(a, lenA, b, lenB) == false);
		    DQN_ASSERT(Dqn_StrHasSubstring(b, lenB, a, lenA) == false);
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

void RandomTest() {

	DqnRandPCGState pcg;
	DqnRnd_PCGInit(&pcg);
	for (i32 i = 0; i < 10; i++)
	{
		i32 min    = -100;
		i32 max    = 100000;
		i32 result = DqnRnd_PCGRange(&pcg, min, max);
		DQN_ASSERT(result >= min && result <= max)

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

		// V2i Creating
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

		// Test operator overloadign
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
			DqnV2 a = DqnV2_2f(0, 0);
			DqnV2 b = DqnV2_2f(3, 4);

			f32 lengthSq = DqnV2_LengthSquared(a, b);
			DQN_ASSERT(lengthSq == 25);

			f32 length = DqnV2_Length(a, b);
			DQN_ASSERT(length == 5);

			DqnV2 normalised = DqnV2_Normalise(b);
			DQN_ASSERT(normalised.x == (b.x / 5.0f));
			DQN_ASSERT(normalised.y == (b.y / 5.0f));

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
		DqnRect rect = DqnRect_Init(DqnV2_2f(-10, -10), DqnV2_2f(20, 20));
		DQN_ASSERT(DqnV2_Equals(rect.min, DqnV2_2f(-10, -10)));
		DQN_ASSERT(DqnV2_Equals(rect.max, DqnV2_2f(10, 10)));

		f32 width, height;
		DqnRect_GetSize2f(rect, &width, &height);
		DQN_ASSERT(width == 20);
		DQN_ASSERT(height == 20);

		DqnV2 dim = DqnRect_GetSizeV2(rect);
		DQN_ASSERT(DqnV2_Equals(dim, DqnV2_2f(20, 20)));

		DqnV2 rectCenter = DqnRect_GetCentre(rect);
		DQN_ASSERT(DqnV2_Equals(rectCenter, DqnV2_2f(0, 0)));

		// Test shifting rect
		DqnRect shiftedRect = DqnRect_Move(rect, DqnV2_2f(10, 0));
		DQN_ASSERT(DqnV2_Equals(shiftedRect.min, DqnV2_2f(0, -10)));
		DQN_ASSERT(DqnV2_Equals(shiftedRect.max, DqnV2_2f(20, 10)));

		DqnRect_GetSize2f(shiftedRect, &width, &height);
		DQN_ASSERT(width == 20);
		DQN_ASSERT(height == 20);

		dim = DqnRect_GetSizeV2(shiftedRect);
		DQN_ASSERT(DqnV2_Equals(dim, DqnV2_2f(20, 20)));

		// Test rect contains p
		DqnV2 inP  = DqnV2_2f(5, 5);
		DqnV2 outP = DqnV2_2f(100, 100);
		DQN_ASSERT(DqnRect_ContainsP(shiftedRect, inP));
		DQN_ASSERT(!DqnRect_ContainsP(shiftedRect, outP));

		printf("VecTest(): Rect: Completed successfully\n");
	}

	printf("VecTest(): Completed successfully\n");
}

void ArrayTest()
{
	{
		DqnArray<DqnV2> array = {};
		DQN_ASSERT(DqnArray_Init(&array, 1));
		DQN_ASSERT(array.capacity == 1);
		DQN_ASSERT(array.count == 0);

		// Test basic insert
		{
			DqnV2 va = DqnV2_2f(5, 10);
			DQN_ASSERT(DqnArray_Push(&array, va));

			DqnV2 vb = array.data[0];
			DQN_ASSERT(DqnV2_Equals(va, vb));

			DQN_ASSERT(array.capacity == 1);
			DQN_ASSERT(array.count == 1);
		}

		// Test array resizing and freeing
		{
			DqnV2 va = DqnV2_2f(10, 15);
			DQN_ASSERT(DqnArray_Push(&array, va));

			DqnV2 vb = array.data[0];
			DQN_ASSERT(DqnV2_Equals(va, vb) == false);

			vb = array.data[1];
			DQN_ASSERT(DqnV2_Equals(va, vb) == true);

			DQN_ASSERT(array.capacity == 2);
			DQN_ASSERT(array.count == 2);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 3);
			DQN_ASSERT(array.count == 3);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 4);
			DQN_ASSERT(array.count == 4);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 5);
			DQN_ASSERT(array.count == 5);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 6);
			DQN_ASSERT(array.count == 6);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 7);
			DQN_ASSERT(array.count == 7);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 8);
			DQN_ASSERT(array.count == 8);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 9);
			DQN_ASSERT(array.count == 9);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 10);
			DQN_ASSERT(array.count == 10);

			DQN_ASSERT(DqnArray_Push(&array, va));
			DQN_ASSERT(array.capacity == 12);
			DQN_ASSERT(array.count == 11);

			DqnV2 vc = DqnV2_2f(90, 100);
			DQN_ASSERT(DqnArray_Push(&array, vc));
			DQN_ASSERT(array.capacity == 12);
			DQN_ASSERT(array.count == 12);
			DQN_ASSERT(DqnV2_Equals(vc, array.data[11]));

			DQN_ASSERT(DqnArray_Free(&array) == true);
		}
	}

	{
		DqnArray<f32> array = {};
		DQN_ASSERT(DqnArray_Init(&array, 1));
		DQN_ASSERT(array.capacity == 1);
		DQN_ASSERT(array.count == 0);
		DqnArray_Free(&array);
	}

	{
		DqnV2 a = DqnV2_2f(1, 2);
		DqnV2 b = DqnV2_2f(3, 4);
		DqnV2 c = DqnV2_2f(5, 6);
		DqnV2 d = DqnV2_2f(7, 8);

		DqnArray<DqnV2> array = {};
		DQN_ASSERT(DqnArray_Init(&array, 16));
		DQN_ASSERT(DqnArray_Remove(&array, 0) == false);
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		DQN_ASSERT(DqnArray_Clear(&array));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		DQN_ASSERT(DqnArray_Push(&array, a));
		DQN_ASSERT(DqnArray_Push(&array, b));
		DQN_ASSERT(DqnArray_Push(&array, c));
		DQN_ASSERT(DqnArray_Push(&array, d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 4);

		DQN_ASSERT(DqnArray_Remove(&array, 0));
		DQN_ASSERT(DqnV2_Equals(array.data[0], d));
		DQN_ASSERT(DqnV2_Equals(array.data[1], b));
		DQN_ASSERT(DqnV2_Equals(array.data[2], c));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 3);

		DQN_ASSERT(DqnArray_Remove(&array, 2));
		DQN_ASSERT(DqnV2_Equals(array.data[0], d));
		DQN_ASSERT(DqnV2_Equals(array.data[1], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		DQN_ASSERT(DqnArray_Remove(&array, 100) == false);
		DQN_ASSERT(DqnV2_Equals(array.data[0], d));
		DQN_ASSERT(DqnV2_Equals(array.data[1], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		DQN_ASSERT(DqnArray_Clear(&array));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		DqnArray_Free(&array);
	}

	{
		DqnV2 a = DqnV2_2f(1, 2);
		DqnV2 b = DqnV2_2f(3, 4);
		DqnV2 c = DqnV2_2f(5, 6);
		DqnV2 d = DqnV2_2f(7, 8);

		DqnArray<DqnV2> array = {};
		DQN_ASSERT(DqnArray_Init(&array, 16));

		DQN_ASSERT(DqnArray_Push(&array, a));
		DQN_ASSERT(DqnArray_Push(&array, b));
		DQN_ASSERT(DqnArray_Push(&array, c));
		DQN_ASSERT(DqnArray_Push(&array, d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 4);

		DqnArray_RemoveStable(&array, 0);
		DQN_ASSERT(DqnV2_Equals(array.data[0], b));
		DQN_ASSERT(DqnV2_Equals(array.data[1], c));
		DQN_ASSERT(DqnV2_Equals(array.data[2], d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 3);

		DqnArray_RemoveStable(&array, 1);
		DQN_ASSERT(DqnV2_Equals(array.data[0], b));
		DQN_ASSERT(DqnV2_Equals(array.data[1], d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		DqnArray_RemoveStable(&array, 1);
		DQN_ASSERT(DqnV2_Equals(array.data[0], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 1);
		DqnArray_Free(&array);
	}

	printf("ArrayTest(): Completed successfully\n");
}

void FileTest()
{
	// File i/o
	{
		{
			DqnFile file = {};
			DQN_ASSERT(DqnFile_Open(
				".clang-format", &file,
				(dqnfilepermissionflag_write | dqnfilepermissionflag_read),
				dqnfileaction_open_only));
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
				(dqnfilepermissionflag_write | dqnfilepermissionflag_read),
				dqnfileaction_open_only));
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

void PushBufferTest()
{
	size_t allocSize  = DQN_KILOBYTE(1);
	DqnPushBuffer buffer = {};
	const u32 ALIGNMENT  = 4;
	DqnPushBuffer_Init(&buffer, allocSize, ALIGNMENT);
	DQN_ASSERT(buffer.block && buffer.block->memory);
	DQN_ASSERT(buffer.block->size == allocSize);
	DQN_ASSERT(buffer.block->used == 0);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);

	// Alocate A
	size_t sizeA = (size_t)(allocSize * 0.5f);
	void *resultA = DqnPushBuffer_Allocate(&buffer, sizeA);
	u64 resultAddrA = *((u64 *)resultA);
	DQN_ASSERT(resultAddrA % ALIGNMENT == 0);
	DQN_ASSERT(buffer.block && buffer.block->memory);
	DQN_ASSERT(buffer.block->size == allocSize);
	DQN_ASSERT(buffer.block->used >= sizeA + 0 &&
	           buffer.block->used <= sizeA + 3);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);
	DQN_ASSERT(resultA);
	u8 *ptrA = (u8 *)resultA;
	for (u32 i  = 0; i < sizeA; i++)
		ptrA[i] = 1;

	DqnPushBufferBlock *blockA = buffer.block;
	// Alocate B
	size_t sizeB  = (size_t)(allocSize * 2.0f);
	void *resultB = DqnPushBuffer_Allocate(&buffer, sizeB);
	u64 resultAddrB = *((u64 *)resultB);
	DQN_ASSERT(resultAddrB % ALIGNMENT == 0);
	DQN_ASSERT(buffer.block && buffer.block->memory);
	DQN_ASSERT(buffer.block->size == DQN_KILOBYTE(2));

	// Since we alignment the pointers we return they can be within 0-3 bytes of
	// what we expect and since this is in a new block as well used will reflect
	// just this allocation.
	DQN_ASSERT(buffer.block->used >= sizeB + 0 &&
	           buffer.block->used <= sizeB + 3);
	DQN_ASSERT(resultB);
	u8 *ptrB = (u8 *)resultB;
	for (u32 i  = 0; i < sizeB; i++)
		ptrB[i] = 2;

	// Check that a new block was created since there wasn't enough space
	DQN_ASSERT(buffer.block->prevBlock == blockA);
	DQN_ASSERT(buffer.block != blockA);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);
	DQN_ASSERT(blockA->used == sizeA);
	DqnPushBufferBlock *blockB = buffer.block;

	// Check temp regions work
	DqnTempBuffer tempBuffer = DqnPushBuffer_BeginTempRegion(&buffer);
	size_t sizeC          = 1024 + 1;
	void *resultC = DqnPushBuffer_Allocate(tempBuffer.buffer, sizeC);
	u64 resultAddrC = *((u64 *)resultC);
	DQN_ASSERT(resultAddrC % ALIGNMENT == 0);
	DQN_ASSERT(buffer.block != blockB && buffer.block != blockA);
	DQN_ASSERT(buffer.block->used >= sizeC + 0 &&
	           buffer.block->used <= sizeC + 3);
	DQN_ASSERT(buffer.tempBufferCount == 1);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);

	// NOTE: Allocation should be aligned to 4 byte boundary
	DQN_ASSERT(tempBuffer.buffer->block->size == 2048);
	u8 *ptrC = (u8 *)resultC;
	for (u32 i  = 0; i < sizeC; i++)
		ptrC[i] = 3;

	// Check that a new block was created since there wasn't enough space
	DQN_ASSERT(buffer.block->prevBlock == blockB);
	DQN_ASSERT(buffer.block != blockB);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);

	for (u32 i  = 0; i < sizeA; i++)
		DQN_ASSERT(ptrA[i] == 1);
	for (u32 i  = 0; i < sizeB; i++)
		DQN_ASSERT(ptrB[i] == 2);
	for (u32 i  = 0; i < sizeC; i++)
		DQN_ASSERT(ptrC[i] == 3);

	// End temp region which should revert back to 2 linked buffers, A and B
	DqnPushBuffer_EndTempRegion(tempBuffer);
	DQN_ASSERT(buffer.block && buffer.block->memory);
	DQN_ASSERT(buffer.block->size == sizeB);
	DQN_ASSERT(buffer.block->used >= sizeB + 0 &&
	           buffer.block->used <= sizeB + 3);
	DQN_ASSERT(buffer.tempBufferCount == 0);
	DQN_ASSERT(resultB);

	DQN_ASSERT(buffer.block->prevBlock == blockA);
	DQN_ASSERT(buffer.block != blockA);
	DQN_ASSERT(blockA->used == sizeA);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);

	// Release the last linked buffer from the push buffer
	DqnPushBuffer_FreeLastBuffer(&buffer);

	// Which should return back to the 1st allocation
	DQN_ASSERT(buffer.block == blockA);
	DQN_ASSERT(buffer.block->memory);
	DQN_ASSERT(buffer.block->size == allocSize);
	DQN_ASSERT(buffer.block->used == sizeA);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);

	// Free once more to release buffer A memory
	DqnPushBuffer_FreeLastBuffer(&buffer);
	DQN_ASSERT(!buffer.block);
	DQN_ASSERT(buffer.alignment == ALIGNMENT);
	DQN_ASSERT(buffer.tempBufferCount == 0);
}

int main(void)
{
	StringsTest();
	RandomTest();
	MathTest();
	VecTest();
	OtherTest();
	ArrayTest();
	FileTest();
	PushBufferTest();

	printf("\nPress 'Enter' Key to Exit\n");
	getchar();

	return 0;
}

