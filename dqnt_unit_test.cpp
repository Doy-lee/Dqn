#define DQNT_IMPLEMENTATION
#define DQNT_MAKE_STATIC
#include "dqnt.h"

#include "stdio.h"

void dqnt_strings_test()
{
	{ // Char Checks
		DQNT_ASSERT(dqnt_char_is_alpha('a') == true);
		DQNT_ASSERT(dqnt_char_is_alpha('A') == true);
		DQNT_ASSERT(dqnt_char_is_alpha('0') == false);
		DQNT_ASSERT(dqnt_char_is_alpha('@') == false);
		DQNT_ASSERT(dqnt_char_is_alpha(' ') == false);
		DQNT_ASSERT(dqnt_char_is_alpha('\n') == false);

		DQNT_ASSERT(dqnt_char_is_digit('1') == true);
		DQNT_ASSERT(dqnt_char_is_digit('n') == false);
		DQNT_ASSERT(dqnt_char_is_digit('N') == false);
		DQNT_ASSERT(dqnt_char_is_digit('*') == false);
		DQNT_ASSERT(dqnt_char_is_digit(' ') == false);
		DQNT_ASSERT(dqnt_char_is_digit('\n') == false);

		DQNT_ASSERT(dqnt_char_is_alphanum('1') == true);
		DQNT_ASSERT(dqnt_char_is_alphanum('a') == true);
		DQNT_ASSERT(dqnt_char_is_alphanum('A') == true);
		DQNT_ASSERT(dqnt_char_is_alphanum('*') == false);
		DQNT_ASSERT(dqnt_char_is_alphanum(' ') == false);
		DQNT_ASSERT(dqnt_char_is_alphanum('\n') == false);

		printf("dqnt_strings_test(): char_checks: Completed successfully\n");
	}

	// String Checks
	{
		// strcmp
		{
			char *a = "str_a";

			// Check simple compares
			{
				DQNT_ASSERT(dqnt_strcmp(a, "str_a") == +0);
				DQNT_ASSERT(dqnt_strcmp(a, "str_b") == -1);
				DQNT_ASSERT(dqnt_strcmp("str_b", a) == +1);
				DQNT_ASSERT(dqnt_strcmp(a, "") == +1);
				DQNT_ASSERT(dqnt_strcmp("", "") == 0);

				// NOTE: Check that the string has not been trashed.
				DQNT_ASSERT(dqnt_strcmp(a, "str_a") == +0);
			}

			// Check ops against null
			{
				DQNT_ASSERT(dqnt_strcmp(NULL, NULL) != +0);
				DQNT_ASSERT(dqnt_strcmp(a, NULL) != +0);
				DQNT_ASSERT(dqnt_strcmp(NULL, a) != +0);
			}

			printf("dqnt_strings_test(): strcmp: Completed successfully\n");
		}

		// strlen
		{
			char *a = "str_a";
			DQNT_ASSERT(dqnt_strlen(a) == 5);
			DQNT_ASSERT(dqnt_strlen("") == 0);
			DQNT_ASSERT(dqnt_strlen("   a  ") == 6);
			DQNT_ASSERT(dqnt_strlen("a\n") == 2);

			// NOTE: Check that the string has not been trashed.
			DQNT_ASSERT(dqnt_strcmp(a, "str_a") == 0);

			DQNT_ASSERT(dqnt_strlen(NULL) == 0);

			printf("dqnt_strings_test(): strlen: Completed successfully\n");
		}

		// strncpy
		{
			{
				char *a    = "str_a";
				char b[10] = {};
				// Check copy into empty array
				{
					char *result = dqnt_strncpy(b, a, dqnt_strlen(a));
					DQNT_ASSERT(dqnt_strcmp(b, "str_a") == 0);
					DQNT_ASSERT(dqnt_strcmp(a, "str_a") == 0);
					DQNT_ASSERT(dqnt_strcmp(result, "str_a") == 0);
					DQNT_ASSERT(dqnt_strlen(result) == 5);
				}

				// Check copy into array offset, overlap with old results
				{
					char *newResult = dqnt_strncpy(&b[1], a, dqnt_strlen(a));
					DQNT_ASSERT(dqnt_strcmp(newResult, "str_a") == 0);
					DQNT_ASSERT(dqnt_strlen(newResult) == 5);

					DQNT_ASSERT(dqnt_strcmp(a, "str_a") == 0);
					DQNT_ASSERT(dqnt_strlen(a) == 5);

					DQNT_ASSERT(dqnt_strcmp(b, "sstr_a") == 0);
					DQNT_ASSERT(dqnt_strlen(b) == 6);
				}
			}

			// Check strncpy with NULL pointers
			{
				DQNT_ASSERT(dqnt_strncpy(NULL, NULL, 5) == NULL);

				char *a      = "str";
				char *result = dqnt_strncpy(a, NULL, 5);

				DQNT_ASSERT(dqnt_strcmp(a, "str") == 0);
				DQNT_ASSERT(dqnt_strcmp(result, "str") == 0);
				DQNT_ASSERT(dqnt_strcmp(result, a) == 0);
			}

			// Check strncpy with 0 chars to copy
			{
				char *a = "str";
				char *b = "ing";

				char *result = dqnt_strncpy(a, b, 0);
				DQNT_ASSERT(dqnt_strcmp(a, "str") == 0);
				DQNT_ASSERT(dqnt_strcmp(b, "ing") == 0);
				DQNT_ASSERT(dqnt_strcmp(result, "str") == 0);
			}

			printf("dqnt_strings_test(): strncpy: Completed successfully\n");
		}

		// str_reverse
		{
			// Basic reverse operations
			{
				char a[] = "aba";
				DQNT_ASSERT(dqnt_str_reverse(a, dqnt_strlen(a)) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "aba") == 0);

				DQNT_ASSERT(dqnt_str_reverse(a, 2) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "baa") == 0);

				DQNT_ASSERT(dqnt_str_reverse(a, dqnt_strlen(a)) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "aab") == 0);

				DQNT_ASSERT(dqnt_str_reverse(&a[1], 2) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "aba") == 0);

				DQNT_ASSERT(dqnt_str_reverse(a, 0) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "aba") == 0);
			}

			// Try reverse empty string
			{
				char a[] = "";
				DQNT_ASSERT(dqnt_str_reverse(a, dqnt_strlen(a)) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "") == 0);
			}

			// Try reverse single char string
			{
				char a[] = "a";
				DQNT_ASSERT(dqnt_str_reverse(a, dqnt_strlen(a)) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "a") == 0);

				DQNT_ASSERT(dqnt_str_reverse(a, 0) == true);
				DQNT_ASSERT(dqnt_strcmp(a, "a") == 0);
			}

			printf(
			    "dqnt_strings_test(): str_reverse: Completed successfully\n");
		}

		// str_to_i32
		{
			char *a = "123";
			DQNT_ASSERT(dqnt_str_to_i32(a, dqnt_strlen(a)) == 123);

			char *b = "-123";
			DQNT_ASSERT(dqnt_str_to_i32(b, dqnt_strlen(b)) == -123);
			DQNT_ASSERT(dqnt_str_to_i32(b, 1) == 0);
			DQNT_ASSERT(dqnt_str_to_i32(&b[1], dqnt_strlen(&b[1])) == 123);

			char *c = "-0";
			DQNT_ASSERT(dqnt_str_to_i32(c, dqnt_strlen(c)) == 0);

			char *d = "+123";
			DQNT_ASSERT(dqnt_str_to_i32(d, dqnt_strlen(d)) == 123);
			DQNT_ASSERT(dqnt_str_to_i32(&d[1], dqnt_strlen(&d[1])) == 123);

			printf("dqnt_strings_test(): str_to_i32: Completed successfully\n");
		}

		// i32_to_str
		{
			char a[DQNT_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqnt_i32_to_str(+100, a, DQNT_ARRAY_COUNT(a));
			DQNT_ASSERT(dqnt_strcmp(a, "100") == 0);

			char b[DQNT_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqnt_i32_to_str(-100, b, DQNT_ARRAY_COUNT(b));
			DQNT_ASSERT(dqnt_strcmp(b, "-100") == 0);

			char c[DQNT_I32_TO_STR_MAX_BUF_SIZE] = {};
			dqnt_i32_to_str(0, c, DQNT_ARRAY_COUNT(c));
			DQNT_ASSERT(dqnt_strcmp(c, "0") == 0);

			printf("dqnt_strings_test(): str_to_i32: Completed successfully\n");
		}
    }

	// Wide String Checks
	{
		// wstrcmp
		{
			wchar_t *a = L"str_a";

			// Check simple compares
			{
				DQNT_ASSERT(dqnt_wstrcmp(a, L"str_a") == +0);
				DQNT_ASSERT(dqnt_wstrcmp(a, L"str_b") == -1);
				DQNT_ASSERT(dqnt_wstrcmp(L"str_b", a) == +1);
				DQNT_ASSERT(dqnt_wstrcmp(a, L"") == +1);
				DQNT_ASSERT(dqnt_wstrcmp(L"", L"") == 0);

				// NOTE: Check that the string has not been trashed.
				DQNT_ASSERT(dqnt_wstrcmp(a, L"str_a") == +0);
			}

			// Check ops against null
			{
				DQNT_ASSERT(dqnt_wstrcmp(NULL, NULL) != +0);
				DQNT_ASSERT(dqnt_wstrcmp(a, NULL) != +0);
				DQNT_ASSERT(dqnt_wstrcmp(NULL, a) != +0);
			}

			printf("dqnt_strings_test(): wstrcmp: Completed successfully\n");
		}

	    // wstrlen
	    {
		    wchar_t *a = L"str_a";
		    DQNT_ASSERT(dqnt_wstrlen(a) == 5);
		    DQNT_ASSERT(dqnt_wstrlen(L"") == 0);
		    DQNT_ASSERT(dqnt_wstrlen(L"   a  ") == 6);
		    DQNT_ASSERT(dqnt_wstrlen(L"a\n") == 2);

		    // NOTE: Check that the string has not been trashed.
		    DQNT_ASSERT(dqnt_wstrcmp(a, L"str_a") == 0);

		    DQNT_ASSERT(dqnt_wstrlen(NULL) == 0);

		    printf("dqnt_strings_test(): wstrlen: Completed successfully\n");
	    }
    }

	printf("dqnt_strings_test(): Completed successfully\n");
}

#include "Windows.h"
#define WIN32_LEAN_AND_MEAN
void dqnt_other_test()
{
	{ // Test Win32 Sleep
		// NOTE: Win32 Sleep is not granular to a certain point so sleep excessively to meet 
		u32 sleepInMs = 100;
		f64 startInMs = dqnt_time_now_in_ms();
		Sleep(sleepInMs);
		f64 endInMs = dqnt_time_now_in_ms();

		DQNT_ASSERT(startInMs < endInMs);
		printf("dqnt_other_test(): time_now: Completed successfully\n");
	}
	printf("dqnt_other_test(): Completed successfully\n");
}

void dqnt_random_test() {

	DqntRandPCGState pcg;
	dqnt_rnd_pcg_init(&pcg);
	for (i32 i = 0; i < 10; i++)
	{
		i32 min    = -100;
		i32 max    = 100000;
		i32 result = dqnt_rnd_pcg_range(&pcg, min, max);
		DQNT_ASSERT(result >= min && result <= max)

		f32 randF32 = dqnt_rnd_pcg_nextf(&pcg);
		DQNT_ASSERT(randF32 >= 0.0f && randF32 <= 1.0f);
		printf("dqnt_random_test(): rnd_pcg: Completed successfully\n");
	}

	printf("dqnt_random_test(): Completed successfully\n");
}

void dqnt_vec_test()
{
	{ // V2
		{
			dqnt_v2 vec = dqnt_vec2(5.5f, 5.0f);
			DQNT_ASSERT(vec.x == 5.5f && vec.y == 5.0f);
			DQNT_ASSERT(vec.w == 5.5f && vec.h == 5.0f);
		}

		{
			dqnt_v2 vec = dqnt_vec2i(3, 5);
			DQNT_ASSERT(vec.x == 3 && vec.y == 5.0f);
			DQNT_ASSERT(vec.w == 3 && vec.h == 5.0f);
		}

		{ // constrain_to_ratio
			dqnt_v2 ratio  = dqnt_vec2(16, 9);
			dqnt_v2 dim    = dqnt_vec2(2000, 1080);
			dqnt_v2 result = dqnt_vec2_constrain_to_ratio(dim, ratio);
			DQNT_ASSERT(result.w == 1920 && result.h == 1080);
		}

		printf("dqnt_vec_test(): vec2: Completed successfully\n");
	}

	{ // V3
		{
			dqnt_v3 vec = dqnt_vec3(5.5f, 5.0f, 5.875f);
			DQNT_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f);
			DQNT_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f);
		}

		{
			dqnt_v3 vec = dqnt_vec3(3, 4, 5);
			DQNT_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5);
			DQNT_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5);
		}

		printf("dqnt_vec_test(): vec3: Completed successfully\n");
	}

	{ // V4
		{
			dqnt_v4 vec = dqnt_vec4(5.5f, 5.0f, 5.875f, 5.928f);
			DQNT_ASSERT(vec.x == 5.5f && vec.y == 5.0f && vec.z == 5.875f && vec.w == 5.928f);
			DQNT_ASSERT(vec.r == 5.5f && vec.g == 5.0f && vec.b == 5.875f && vec.a == 5.928f);
		}

		{
			dqnt_v4 vec = dqnt_vec4i(3, 4, 5, 6);
		    DQNT_ASSERT(vec.x == 3 && vec.y == 4 && vec.z == 5 && vec.w == 6);
		    DQNT_ASSERT(vec.r == 3 && vec.g == 4 && vec.b == 5 && vec.a == 6);
	    }

		printf("dqnt_vec_test(): vec4: Completed successfully\n");
	}

	printf("dqnt_vec_test(): Completed successfully\n");
}

int main(void)
{
	dqnt_strings_test();
	dqnt_random_test();
	dqnt_vec_test();
	dqnt_other_test();
	return 0;
}
