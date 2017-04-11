#define DQN_IMPLEMENTATION
#include "dqn.h"

#include "stdio.h"

void dqn_strings_test()
{
	{ // Char Checks
		DQN_ASSERT(dqn_char_is_alpha('a') == true);
		DQN_ASSERT(dqn_char_is_alpha('A') == true);
		DQN_ASSERT(dqn_char_is_alpha('0') == false);
		DQN_ASSERT(dqn_char_is_alpha('@') == false);
		DQN_ASSERT(dqn_char_is_alpha(' ') == false);
		DQN_ASSERT(dqn_char_is_alpha('\n') == false);

		DQN_ASSERT(dqn_char_is_digit('1') == true);
		DQN_ASSERT(dqn_char_is_digit('n') == false);
		DQN_ASSERT(dqn_char_is_digit('N') == false);
		DQN_ASSERT(dqn_char_is_digit('*') == false);
		DQN_ASSERT(dqn_char_is_digit(' ') == false);
		DQN_ASSERT(dqn_char_is_digit('\n') == false);

		DQN_ASSERT(dqn_char_is_alphanum('1') == true);
		DQN_ASSERT(dqn_char_is_alphanum('a') == true);
		DQN_ASSERT(dqn_char_is_alphanum('A') == true);
		DQN_ASSERT(dqn_char_is_alphanum('*') == false);
		DQN_ASSERT(dqn_char_is_alphanum(' ') == false);
		DQN_ASSERT(dqn_char_is_alphanum('\n') == false);

		printf("dqn_strings_test(): char_checks: Completed successfully\n");
	}

	// String Checks
	{
		// strcmp
		{
			char *a = "str_a";

			// Check simple compares
			{
				DQN_ASSERT(dqn_strcmp(a, "str_a") == +0);
				DQN_ASSERT(dqn_strcmp(a, "str_b") == -1);
				DQN_ASSERT(dqn_strcmp("str_b", a) == +1);
				DQN_ASSERT(dqn_strcmp(a, "") == +1);
				DQN_ASSERT(dqn_strcmp("", "") == 0);

				// NOTE: Check that the string has not been trashed.
				DQN_ASSERT(dqn_strcmp(a, "str_a") == +0);
			}

			// Check ops against null
			{
				DQN_ASSERT(dqn_strcmp(NULL, NULL) != +0);
				DQN_ASSERT(dqn_strcmp(a, NULL) != +0);
				DQN_ASSERT(dqn_strcmp(NULL, a) != +0);
			}

			printf("dqn_strings_test(): strcmp: Completed successfully\n");
		}

		// strlen
		{
			char *a = "str_a";
			DQN_ASSERT(dqn_strlen(a) == 5);
			DQN_ASSERT(dqn_strlen("") == 0);
			DQN_ASSERT(dqn_strlen("   a  ") == 6);
			DQN_ASSERT(dqn_strlen("a\n") == 2);

			// NOTE: Check that the string has not been trashed.
			DQN_ASSERT(dqn_strcmp(a, "str_a") == 0);

			DQN_ASSERT(dqn_strlen(NULL) == 0);

			printf("dqn_strings_test(): strlen: Completed successfully\n");
		}

		// strncpy
		{
			{
				char *a    = "str_a";
				char b[10] = {};
				// Check copy into empty array
				{
					char *result = dqn_strncpy(b, a, dqn_strlen(a));
					DQN_ASSERT(dqn_strcmp(b, "str_a") == 0);
					DQN_ASSERT(dqn_strcmp(a, "str_a") == 0);
					DQN_ASSERT(dqn_strcmp(result, "str_a") == 0);
					DQN_ASSERT(dqn_strlen(result) == 5);
				}

				// Check copy into array offset, overlap with old results
				{
					char *newResult = dqn_strncpy(&b[1], a, dqn_strlen(a));
					DQN_ASSERT(dqn_strcmp(newResult, "str_a") == 0);
					DQN_ASSERT(dqn_strlen(newResult) == 5);

					DQN_ASSERT(dqn_strcmp(a, "str_a") == 0);
					DQN_ASSERT(dqn_strlen(a) == 5);

					DQN_ASSERT(dqn_strcmp(b, "sstr_a") == 0);
					DQN_ASSERT(dqn_strlen(b) == 6);
				}
			}

			// Check strncpy with NULL pointers
			{
				DQN_ASSERT(dqn_strncpy(NULL, NULL, 5) == NULL);

				char *a      = "str";
				char *result = dqn_strncpy(a, NULL, 5);

				DQN_ASSERT(dqn_strcmp(a, "str") == 0);
				DQN_ASSERT(dqn_strcmp(result, "str") == 0);
				DQN_ASSERT(dqn_strcmp(result, a) == 0);
			}

			// Check strncpy with 0 chars to copy
			{
				char *a = "str";
				char *b = "ing";

				char *result = dqn_strncpy(a, b, 0);
				DQN_ASSERT(dqn_strcmp(a, "str") == 0);
				DQN_ASSERT(dqn_strcmp(b, "ing") == 0);
				DQN_ASSERT(dqn_strcmp(result, "str") == 0);
			}

			printf("dqn_strings_test(): strncpy: Completed successfully\n");
		}

		// str_reverse
		{
			// Basic reverse operations
			{
				char a[] = "aba";
				DQN_ASSERT(dqn_str_reverse(a, dqn_strlen(a)) == true);
				DQN_ASSERT(dqn_strcmp(a, "aba") == 0);

				DQN_ASSERT(dqn_str_reverse(a, 2) == true);
				DQN_ASSERT(dqn_strcmp(a, "baa") == 0);

				DQN_ASSERT(dqn_str_reverse(a, dqn_strlen(a)) == true);
				DQN_ASSERT(dqn_strcmp(a, "aab") == 0);

				DQN_ASSERT(dqn_str_reverse(&a[1], 2) == true);
				DQN_ASSERT(dqn_strcmp(a, "aba") == 0);

				DQN_ASSERT(dqn_str_reverse(a, 0) == true);
				DQN_ASSERT(dqn_strcmp(a, "aba") == 0);
			}

			// Try reverse empty string
			{
				char a[] = "";
				DQN_ASSERT(dqn_str_reverse(a, dqn_strlen(a)) == true);
				DQN_ASSERT(dqn_strcmp(a, "") == 0);
			}

			// Try reverse single char string
			{
				char a[] = "a";
				DQN_ASSERT(dqn_str_reverse(a, dqn_strlen(a)) == true);
				DQN_ASSERT(dqn_strcmp(a, "a") == 0);

				DQN_ASSERT(dqn_str_reverse(a, 0) == true);
				DQN_ASSERT(dqn_strcmp(a, "a") == 0);
			}

			printf(
			    "dqn_strings_test(): str_reverse: Completed successfully\n");
		}

		// str_to_i32
		{
			char *a = "123";
			DQN_ASSERT(dqn_str_to_i32(a, dqn_strlen(a)) == 123);

			char *b = "-123";
			DQN_ASSERT(dqn_str_to_i32(b, dqn_strlen(b)) == -123);
			DQN_ASSERT(dqn_str_to_i32(b, 1) == 0);
			DQN_ASSERT(dqn_str_to_i32(&b[1], dqn_strlen(&b[1])) == 123);

			char *c = "-0";
			DQN_ASSERT(dqn_str_to_i32(c, dqn_strlen(c)) == 0);

			char *d = "+123";
			DQN_ASSERT(dqn_str_to_i32(d, dqn_strlen(d)) == 123);
			DQN_ASSERT(dqn_str_to_i32(&d[1], dqn_strlen(&d[1])) == 123);

			printf("dqn_strings_test(): str_to_i32: Completed successfully\n");
		}

		// i32_to_str
		{
			char a[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqn_i32_to_str(+100, a, DQN_ARRAY_COUNT(a));
			DQN_ASSERT(dqn_strcmp(a, "100") == 0);

			char b[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqn_i32_to_str(-100, b, DQN_ARRAY_COUNT(b));
			DQN_ASSERT(dqn_strcmp(b, "-100") == 0);

			char c[DQN_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqn_i32_to_str(0, c, DQN_ARRAY_COUNT(c));
			DQN_ASSERT(dqn_strcmp(c, "0") == 0);

			printf("dqn_strings_test(): str_to_i32: Completed successfully\n");
		}
	}

    // UCS <-> UTF8 Checks
    {
	    // Test ascii characters
	    {
		    u32 codepoint = '@';
		    u32 string[1] = {};

		    u32 bytesUsed = dqn_ucs_to_utf8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 1);
		    DQN_ASSERT(string[0] == '@');

		    bytesUsed = dqn_utf8_to_ucs(&string[0], codepoint);
		    DQN_ASSERT(string[0] >= 0 && string[0] < 0x80);
		    DQN_ASSERT(bytesUsed == 1);
	    }

	    // Test 2 byte characters
		{
		    u32 codepoint = 0x278;
		    u32 string[1] = {};

		    u32 bytesUsed = dqn_ucs_to_utf8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 2);
		    DQN_ASSERT(string[0] == 0xC9B8);

		    bytesUsed = dqn_utf8_to_ucs(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 2);
	    }

	    // Test 3 byte characters
		{
		    u32 codepoint = 0x0A0A;
		    u32 string[1] = {};

			u32 bytesUsed = dqn_ucs_to_utf8(&string[0], codepoint);
		    DQN_ASSERT(bytesUsed == 3);
			DQN_ASSERT(string[0] == 0xE0A88A);

		    bytesUsed = dqn_utf8_to_ucs(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 3);
	    }

	    // Test 4 byte characters
		{
		    u32 codepoint = 0x10912;
		    u32 string[1] = {};
			u32 bytesUsed = dqn_ucs_to_utf8(&string[0], codepoint);

		    DQN_ASSERT(bytesUsed == 4);
		    DQN_ASSERT(string[0] == 0xF090A492);

		    bytesUsed = dqn_utf8_to_ucs(&string[0], string[0]);
		    DQN_ASSERT(string[0] == codepoint);
		    DQN_ASSERT(bytesUsed == 4);
	    }

		{
		    u32 codepoint = 0x10912;
			u32 bytesUsed = dqn_ucs_to_utf8(NULL, codepoint);
		    DQN_ASSERT(bytesUsed == 0);

		    bytesUsed = dqn_utf8_to_ucs(NULL, codepoint);
		    DQN_ASSERT(bytesUsed == 0);
	    }

	    printf("dqn_strings_test(): ucs <-> utf8: Completed successfully\n");
    }

    printf("dqn_strings_test(): Completed successfully\n");
}

#include "Windows.h"
#define WIN32_LEAN_AND_MEAN
void dqn_other_test()
{
	{ // Test Win32 Sleep
		// NOTE: Win32 Sleep is not granular to a certain point so sleep excessively
		u32 sleepInMs = 1000;
		f64 startInMs = dqn_time_now_in_ms();
		Sleep(sleepInMs);
		f64 endInMs = dqn_time_now_in_ms();

		DQN_ASSERT(startInMs < endInMs);
		printf("dqn_other_test(): time_now: Completed successfully\n");
	}
	printf("dqn_other_test(): Completed successfully\n");
}

void dqn_random_test() {

	DqnRandPCGState pcg;
	dqn_rnd_pcg_init(&pcg);
	for (i32 i = 0; i < 10; i++)
	{
		i32 min    = -100;
		i32 max    = 100000;
		i32 result = dqn_rnd_pcg_range(&pcg, min, max);
		DQN_ASSERT(result >= min && result <= max)

		f32 randF32 = dqn_rnd_pcg_nextf(&pcg);
		DQN_ASSERT(randF32 >= 0.0f && randF32 <= 1.0f);
		printf("dqn_random_test(): rnd_pcg: Completed successfully\n");
	}

	printf("dqn_random_test(): Completed successfully\n");
}

void dqn_math_test()
{
	{ // Lerp
		{
			f32 start = 10;
			f32 t     = 0.5f;
			f32 end   = 20;
			DQN_ASSERT(dqn_math_lerp(start, t, end) == 15);
		}

		{
			f32 start = 10;
			f32 t     = 2.0f;
			f32 end   = 20;
			DQN_ASSERT(dqn_math_lerp(start, t, end) == 30);
		}

		printf("dqn_math_test(): lerp: Completed successfully\n");
	}

	{ // sqrtf
		DQN_ASSERT(dqn_math_sqrtf(4.0f) == 2.0f);
		printf("dqn_math_test(): sqrtf: Completed successfully\n");
	}

	printf("dqn_math_test(): Completed successfully\n");
}

void dqn_vec_test()
{
	{ // V2

		// V2 Creating
		{
			DqnV2 vec = dqn_v2(5.5f, 5.0f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 5.5f && vec.h == 5.0f);
		}

		// V2i Creating
		{
			DqnV2 vec = dqn_v2i(3, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 5.0f);
			DQN_ASSERT(vec.w == 3 && vec.h == 5.0f);
		}

		// V2 Arithmetic
		{
			DqnV2 vecA = dqn_v2(5, 10);
			DqnV2 vecB = dqn_v2i(2, 3);
			DQN_ASSERT(dqn_v2_equals(vecA, vecB) == false);
			DQN_ASSERT(dqn_v2_equals(vecA, dqn_v2(5, 10)) == true);
			DQN_ASSERT(dqn_v2_equals(vecB, dqn_v2(2, 3)) == true);

			DqnV2 result = dqn_v2_add(vecA, dqn_v2(5, 10));
			DQN_ASSERT(dqn_v2_equals(result, dqn_v2(10, 20)) == true);

			result = dqn_v2_sub(result, dqn_v2(5, 10));
			DQN_ASSERT(dqn_v2_equals(result, dqn_v2(5, 10)) == true);

			result = dqn_v2_scale(result, 5);
			DQN_ASSERT(dqn_v2_equals(result, dqn_v2(25, 50)) == true);

			result = dqn_v2_hadamard(result, dqn_v2(10, 0.5f));
			DQN_ASSERT(dqn_v2_equals(result, dqn_v2(250, 25)) == true);

			f32 dotResult = dqn_v2_dot(dqn_v2(5, 10), dqn_v2(3, 4));
			DQN_ASSERT(dotResult == 55);
		}

		// V2 Properties
		{
			DqnV2 a = dqn_v2(0, 0);
			DqnV2 b = dqn_v2(3, 4);

			f32 lengthSq = dqn_v2_length_squared(a, b);
			DQN_ASSERT(lengthSq == 25);

			f32 length = dqn_v2_length(a, b);
			DQN_ASSERT(length == 5);

			DqnV2 normalised = dqn_v2_normalise(b);
			DQN_ASSERT(normalised.x == (b.x / 5.0f));
			DQN_ASSERT(normalised.y == (b.y / 5.0f));

			DqnV2 c = dqn_v2(3.5f, 8.0f);
			DQN_ASSERT(dqn_v2_overlaps(b, c) == true);
			DQN_ASSERT(dqn_v2_overlaps(b, a) == false);

			DqnV2 d = dqn_v2_perpendicular(c);
			DQN_ASSERT(dqn_v2_dot(c, d) == 0);
		}

		{ // constrain_to_ratio
			DqnV2 ratio  = dqn_v2(16, 9);
			DqnV2 dim    = dqn_v2(2000, 1080);
			DqnV2 result = dqn_v2_constrain_to_ratio(dim, ratio);
			DQN_ASSERT(result.w == 1920 && result.h == 1080);
		}

		printf("dqn_vec_test(): vec2: Completed successfully\n");
	}

	{ // V3

		// V3i Creating
		{
			DqnV3 vec = dqn_v3(5.5f, 5.0f, 5.875f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f);
		}

		// V3i Creating
		{
			DqnV3 vec = dqn_v3i(3, 4, 5);
			DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5);
			DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5);
		}

		// V3 Arithmetic
		{
			DqnV3 vecA = dqn_v3(5, 10, 15);
			DqnV3 vecB = dqn_v3(2, 3, 6);
			DQN_ASSERT(dqn_v3_equals(vecA, vecB) == false);
			DQN_ASSERT(dqn_v3_equals(vecA, dqn_v3(5, 10, 15)) == true);
			DQN_ASSERT(dqn_v3_equals(vecB, dqn_v3(2, 3, 6)) == true);

			DqnV3 result = dqn_v3_add(vecA, dqn_v3(5, 10, 15));
			DQN_ASSERT(dqn_v3_equals(result, dqn_v3(10, 20, 30)) == true);

			result = dqn_v3_sub(result, dqn_v3(5, 10, 15));
			DQN_ASSERT(dqn_v3_equals(result, dqn_v3(5, 10, 15)) == true);

			result = dqn_v3_scale(result, 5);
			DQN_ASSERT(dqn_v3_equals(result, dqn_v3(25, 50, 75)) == true);

			result = dqn_v3_hadamard(result, dqn_v3(10.0f, 0.5f, 10.0f));
			DQN_ASSERT(dqn_v3_equals(result, dqn_v3(250, 25, 750)) == true);

			f32 dotResult = dqn_v3_dot(dqn_v3(5, 10, 2), dqn_v3(3, 4, 6));
			DQN_ASSERT(dotResult == 67);

			DqnV3 cross = dqn_v3_cross(vecA, vecB);
			DQN_ASSERT(dqn_v3_equals(cross, dqn_v3(15, 0, -5)) == true);
		}

		printf("dqn_vec_test(): vec3: Completed successfully\n");
	}

	{ // V4

		// V4 Creating
		{
			DqnV4 vec = dqn_v4(5.5f, 5.0f, 5.875f, 5.928f);
			DQN_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f && vec.w == 5.928f);
			DQN_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f && vec.a == 5.928f);
		}

		// V4i Creating
		{
			DqnV4 vec = dqn_v4i(3, 4, 5, 6);
		    DQN_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5 && vec.w == 6);
		    DQN_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5 && vec.a == 6);
	    }

		// V4 Arithmetic
		{
			DqnV4 vecA = dqn_v4(5, 10, 15, 20);
			DqnV4 vecB = dqn_v4i(2, 3, 6, 8);
			DQN_ASSERT(dqn_v4_equals(vecA, vecB) == false);
			DQN_ASSERT(dqn_v4_equals(vecA, dqn_v4(5, 10, 15, 20)) == true);
			DQN_ASSERT(dqn_v4_equals(vecB, dqn_v4(2, 3, 6, 8)) == true);

			DqnV4 result = dqn_v4_add(vecA, dqn_v4(5, 10, 15, 20));
			DQN_ASSERT(dqn_v4_equals(result, dqn_v4(10, 20, 30, 40)) == true);

			result = dqn_v4_sub(result, dqn_v4(5, 10, 15, 20));
			DQN_ASSERT(dqn_v4_equals(result, dqn_v4(5, 10, 15, 20)) == true);

			result = dqn_v4_scale(result, 5);
			DQN_ASSERT(dqn_v4_equals(result, dqn_v4(25, 50, 75, 100)) == true);

			result = dqn_v4_hadamard(result, dqn_v4(10, 0.5f, 10, 0.25f));
			DQN_ASSERT(dqn_v4_equals(result, dqn_v4(250, 25, 750, 25)) == true);

			f32 dotResult = dqn_v4_dot(dqn_v4(5, 10, 2, 8), dqn_v4(3, 4, 6, 5));
			DQN_ASSERT(dotResult == 107);
		}

		printf("dqn_vec_test(): vec4: Completed successfully\n");
	}

	// Rect
	{
		DqnRect rect = dqn_rect(dqn_v2(-10, -10), dqn_v2(20, 20));
		DQN_ASSERT(dqn_v2_equals(rect.min, dqn_v2(-10, -10)));
		DQN_ASSERT(dqn_v2_equals(rect.max, dqn_v2(10, 10)));

		f32 width, height;
		dqn_rect_get_size_2f(rect, &width, &height);
		DQN_ASSERT(width == 20);
		DQN_ASSERT(height == 20);

		DqnV2 dim = dqn_rect_get_size_v2(rect);
		DQN_ASSERT(dqn_v2_equals(dim, dqn_v2(20, 20)));

		DqnV2 rectCenter = dqn_rect_get_centre(rect);
		DQN_ASSERT(dqn_v2_equals(rectCenter, dqn_v2(0, 0)));

		// Test shifting rect
		DqnRect shiftedRect = dqn_rect_move(rect, dqn_v2(10, 0));
		DQN_ASSERT(dqn_v2_equals(shiftedRect.min, dqn_v2(0, -10)));
		DQN_ASSERT(dqn_v2_equals(shiftedRect.max, dqn_v2(20, 10)));

		dqn_rect_get_size_2f(shiftedRect, &width, &height);
		DQN_ASSERT(width == 20);
		DQN_ASSERT(height == 20);

		dim = dqn_rect_get_size_v2(shiftedRect);
		DQN_ASSERT(dqn_v2_equals(dim, dqn_v2(20, 20)));

		// Test rect contains p
		DqnV2 inP  = dqn_v2(5, 5);
		DqnV2 outP = dqn_v2(100, 100);
		DQN_ASSERT(dqn_rect_contains_p(shiftedRect, inP));
		DQN_ASSERT(!dqn_rect_contains_p(shiftedRect, outP));

		printf("dqn_vec_test(): rect: Completed successfully\n");
	}

	printf("dqn_vec_test(): Completed successfully\n");
}

void dqn_darray_test()
{
	{
		DqnArray<DqnV2> array = {};
		DQN_ASSERT(dqn_darray_init(&array, 1));
		DQN_ASSERT(array.capacity == 1);
		DQN_ASSERT(array.count == 0);

		// Test basic insert
		{
			DqnV2 va = dqn_v2(5, 10);
			DQN_ASSERT(dqn_darray_push(&array, va));

			DqnV2 vb = array.data[0];
			DQN_ASSERT(dqn_v2_equals(va, vb));

			DQN_ASSERT(array.capacity == 1);
			DQN_ASSERT(array.count == 1);
		}

		// Test array resizing and freeing
		{
			DqnV2 va = dqn_v2(10, 15);
			DQN_ASSERT(dqn_darray_push(&array, va));

			DqnV2 vb = array.data[0];
			DQN_ASSERT(dqn_v2_equals(va, vb) == false);

			vb = array.data[1];
			DQN_ASSERT(dqn_v2_equals(va, vb) == true);

			DQN_ASSERT(array.capacity == 2);
			DQN_ASSERT(array.count == 2);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 3);
			DQN_ASSERT(array.count == 3);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 4);
			DQN_ASSERT(array.count == 4);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 5);
			DQN_ASSERT(array.count == 5);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 6);
			DQN_ASSERT(array.count == 6);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 7);
			DQN_ASSERT(array.count == 7);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 8);
			DQN_ASSERT(array.count == 8);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 9);
			DQN_ASSERT(array.count == 9);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 10);
			DQN_ASSERT(array.count == 10);

			DQN_ASSERT(dqn_darray_push(&array, va));
			DQN_ASSERT(array.capacity == 12);
			DQN_ASSERT(array.count == 11);

			DqnV2 vc = dqn_v2(90, 100);
			DQN_ASSERT(dqn_darray_push(&array, vc));
			DQN_ASSERT(array.capacity == 12);
			DQN_ASSERT(array.count == 12);
			DQN_ASSERT(dqn_v2_equals(vc, array.data[11]));

			DQN_ASSERT(dqn_darray_free(&array) == true);
		}
	}

	{
		DqnArray<f32> array = {};
		DQN_ASSERT(dqn_darray_init(&array, 1));
		DQN_ASSERT(array.capacity == 1);
		DQN_ASSERT(array.count == 0);
		dqn_darray_free(&array);
	}

	{
		DqnV2 a = dqn_v2(1, 2);
		DqnV2 b = dqn_v2(3, 4);
		DqnV2 c = dqn_v2(5, 6);
		DqnV2 d = dqn_v2(7, 8);

		DqnArray<DqnV2> array = {};
		DQN_ASSERT(dqn_darray_init(&array, 16));
		DQN_ASSERT(dqn_darray_remove(&array, 0) == false);
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		DQN_ASSERT(dqn_darray_clear(&array));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		DQN_ASSERT(dqn_darray_push(&array, a));
		DQN_ASSERT(dqn_darray_push(&array, b));
		DQN_ASSERT(dqn_darray_push(&array, c));
		DQN_ASSERT(dqn_darray_push(&array, d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 4);

		DQN_ASSERT(dqn_darray_remove(&array, 0));
		DQN_ASSERT(dqn_v2_equals(array.data[0], d));
		DQN_ASSERT(dqn_v2_equals(array.data[1], b));
		DQN_ASSERT(dqn_v2_equals(array.data[2], c));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 3);

		DQN_ASSERT(dqn_darray_remove(&array, 2));
		DQN_ASSERT(dqn_v2_equals(array.data[0], d));
		DQN_ASSERT(dqn_v2_equals(array.data[1], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		DQN_ASSERT(dqn_darray_remove(&array, 100) == false);
		DQN_ASSERT(dqn_v2_equals(array.data[0], d));
		DQN_ASSERT(dqn_v2_equals(array.data[1], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		DQN_ASSERT(dqn_darray_clear(&array));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 0);

		dqn_darray_free(&array);
	}

	{
		DqnV2 a = dqn_v2(1, 2);
		DqnV2 b = dqn_v2(3, 4);
		DqnV2 c = dqn_v2(5, 6);
		DqnV2 d = dqn_v2(7, 8);

		DqnArray<DqnV2> array = {};
		DQN_ASSERT(dqn_darray_init(&array, 16));

		DQN_ASSERT(dqn_darray_push(&array, a));
		DQN_ASSERT(dqn_darray_push(&array, b));
		DQN_ASSERT(dqn_darray_push(&array, c));
		DQN_ASSERT(dqn_darray_push(&array, d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 4);

		dqn_darray_remove_stable(&array, 0);
		DQN_ASSERT(dqn_v2_equals(array.data[0], b));
		DQN_ASSERT(dqn_v2_equals(array.data[1], c));
		DQN_ASSERT(dqn_v2_equals(array.data[2], d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 3);

		dqn_darray_remove_stable(&array, 1);
		DQN_ASSERT(dqn_v2_equals(array.data[0], b));
		DQN_ASSERT(dqn_v2_equals(array.data[1], d));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 2);

		dqn_darray_remove_stable(&array, 1);
		DQN_ASSERT(dqn_v2_equals(array.data[0], b));
		DQN_ASSERT(array.capacity == 16);
		DQN_ASSERT(array.count == 1);
		dqn_darray_free(&array);
	}

	printf("dqn_darray_test(): Completed successfully\n");
}

void dqn_file_test()
{
	// File i/o
	{
		DqnFile file = {};
		DQN_ASSERT(dqn_file_open(".clang-format", &file));
		DQN_ASSERT(file.size == 1320);

		u8 *buffer = (u8 *)calloc(1, (size_t)file.size * sizeof(u8));
		DQN_ASSERT(dqn_file_read(file, buffer, (u32)file.size) == file.size);
		free(buffer);

		dqn_file_close(&file);
		DQN_ASSERT(!file.handle && file.size == 0);

		printf("dqn_file_test(): file_io: Completed successfully\n");
	}

	{
		u32 numFiles;
		char **filelist = dqn_dir_read("*", &numFiles);
		printf("dqn_file_test(): dir_read: Display read files\n");

		for (u32 i = 0; i < numFiles; i++)
			printf("dqn_file_test(): dir_read: %s\n", filelist[i]);

		dqn_dir_read_free(filelist, numFiles);
		printf("dqn_file_test(): dir_read: Completed successfully\n");
	}

	printf("dqn_file_test(): Completed successfully\n");
}

int main(void)
{
	dqn_strings_test();
	dqn_random_test();
	dqn_math_test();
	dqn_vec_test();
	dqn_other_test();
	dqn_darray_test();
	dqn_file_test();

	printf("\nPress 'Enter' Key to Exit\n");
	getchar();

	return 0;
}

