#ifndef DQNT_H
#define DQNT_H

/*
	#define DQNT_IMPLEMENTATION // Enable the implementation
	#define DQNT_MAKE_STATIC    // Make all functions be static
	#include "dqnt.h"
 */

#ifdef DQNT_MAKE_STATIC
	#define DQNT_FILE_SCOPE static
#else
	#define DQNT_FILE_SCOPE
#endif

////////////////////////////////////////////////////////////////////////////////
//
// HEADER
//
////////////////////////////////////////////////////////////////////////////////
#include "stdint.h"

#define LOCAL_PERSIST static
#define FILE_SCOPE    static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int32_t i64;
typedef int32_t i32;
typedef int64_t i16;

typedef double f64;
typedef float  f32;

#define DQNT_INVALID_CODE_PATH 0
#define DQNT_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define DQNT_ASSERT(expr) if (!(expr)) { (*((i32 *)0)) = 0; }

#define DQNT_MATH_PI 3.14159265359f
#define DQNT_MATH_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQNT_MATH_DEGREES_TO_RADIANS(x) ((x * (MATH_PI / 180.0f)))
#define DQNT_MATH_RADIANS_TO_DEGREES(x) ((x * (180.0f / MATH_PI)))
#define DQNT_MATH_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQNT_MATH_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQNT_MATH_SQUARED(x) ((x) * (x))
#define DQNT_MATH_SQRT(x) (sqrtf(x))

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
typedef union dqnt_v2 {
	struct { f32 x, y; };
	struct { f32 w, h; };
	struct { f32 min, max; };
	f32 e[2];
} dqnt_v2;

DQNT_FILE_SCOPE dqnt_v2 V2(f32 x, f32 y);
// Create a 2d-vector using ints and typecast to floats
DQNT_FILE_SCOPE dqnt_v2 V2i(i32 x, i32 y);


////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
typedef union dqnt_v3
{
	struct { f32 x, y, z; };
    struct { f32 r, g, b; };
	f32 e[3];
} dqnt_v3;

DQNT_FILE_SCOPE dqnt_v3 V3i(i32 x, i32 y, i32 z);
DQNT_FILE_SCOPE dqnt_v3 V3(f32 x, f32 y, f32 z);


////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
typedef union dqnt_v4
{
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
	f32 e[4];
	dqnt_v2 vec2[2];
} dqnt_v4;

DQNT_FILE_SCOPE dqnt_v3 V3i(i32 x, i32 y, i32 z);
DQNT_FILE_SCOPE dqnt_v4 V4(f32 x, f32 y, f32 z, f32 w);

////////////////////////////////////////////////////////////////////////////////
// Other Math
////////////////////////////////////////////////////////////////////////////////
typedef struct Rect
{
	dqnt_v2 min;
	dqnt_v2 max;
} dqnt_rect;

////////////////////////////////////////////////////////////////////////////////
// String Ops
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE bool  dqnt_char_is_digit   (char c);
DQNT_FILE_SCOPE bool  dqnt_char_is_alpha   (char c);
DQNT_FILE_SCOPE bool  dqnt_char_is_alphanum(char c);

DQNT_FILE_SCOPE i32   dqnt_strcmp (const char *a, const char *b);
// Returns the length without the null terminator
DQNT_FILE_SCOPE i32   dqnt_strlen (const char *a);
DQNT_FILE_SCOPE char *dqnt_strncpy(char *dest, const char *src, i32 numChars);

#define DQNT_I32_TO_STR_MAX_BUF_SIZE 11
DQNT_FILE_SCOPE bool  dqnt_str_reverse(char *buf, const i32 bufSize);
DQNT_FILE_SCOPE i32   dqnt_str_to_i32 (char *const buf, const i32 bufSize);
DQNT_FILE_SCOPE void  dqnt_i32_to_str (i32 value, char *buf, i32 bufSize);

DQNT_FILE_SCOPE i32     dqnt_wstrcmp(const wchar_t *a, const wchar_t *b);
DQNT_FILE_SCOPE void    dqnt_wstrcat(const wchar_t *a, i32 lenA, const wchar_t *b, i32 lenB, wchar_t *out, i32 outLen);
DQNT_FILE_SCOPE i32     dqnt_wstrlen(const wchar_t *a);

////////////////////////////////////////////////////////////////////////////////
// Timer
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE f64 dqnt_time_now_in_s();
DQNT_FILE_SCOPE f64 dqnt_time_now_in_ms();

////////////////////////////////////////////////////////////////////////////////
// PCG (Permuted Congruential Generator) Random Number Generator
////////////////////////////////////////////////////////////////////////////////
typedef struct DqntRandPCGState
{
	u64 state[2];
} DqntRandPCGState;

// You can manually specify a seed by calling init_with_seed, otherwise it
// automatically creates a seed using rdtsc. The generator is not valid until
// it's been seeded.
DQNT_FILE_SCOPE void dqnt_rnd_pcg_init_with_seed(DqntRandPCGState *pcg, u32 seed);
DQNT_FILE_SCOPE void dqnt_rnd_pcg_init(DqntRandPCGState *pcg);

// Returns a random number N between [0, 0xFFFFFFFF]
DQNT_FILE_SCOPE u32  dqnt_rnd_pcg_next (DqntRandPCGState *pcg);
// Returns a random float N between [0.0, 1.0f]
DQNT_FILE_SCOPE f32  dqnt_rnd_pcg_nextf(DqntRandPCGState *pcg);
// Returns a random integer N between [min, max]
DQNT_FILE_SCOPE i32  dqnt_rnd_pcg_range(DqntRandPCGState *pcg, i32 min, i32 max);

#endif  /* DQNT_H */

////////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
////////////////////////////////////////////////////////////////////////////////
#ifdef DQNT_IMPLEMENTATION
#undef DQNT_IMPLEMENTATION

#ifdef _WIN32
	#define DQNT_WIN32

	#include "Windows.h"
	#define WIN32_LEAN_AND_MEAN
#endif

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline dqnt_v2 dqnt_vec2(f32 x, f32 y)
{
	dqnt_v2 result = {};
	result.x  = x;
	result.y  = y;

	return result;
}

DQNT_FILE_SCOPE inline dqnt_v2 dqnt_vec2i(i32 x, i32 y)
{
	dqnt_v2 result = dqnt_vec2((f32)x, (f32)y);
	return result;
}

DQNT_FILE_SCOPE dqnt_v2 dqnt_vec2_constrain_to_ratio(dqnt_v2 dim,
                                                     dqnt_v2 ratio)
{
	dqnt_v2 result = {};
	f32 numRatioIncrementsToWidth  = (f32)(dim.w / ratio.w);
	f32 numRatioIncrementsToHeight = (f32)(dim.h / ratio.h);

	f32 leastIncrementsToSide =
	    DQNT_MATH_MIN(numRatioIncrementsToHeight, numRatioIncrementsToWidth);

	result.w = (f32)(ratio.w * leastIncrementsToSide);
	result.h = (f32)(ratio.h * leastIncrementsToSide);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline dqnt_v3 dqnt_vec3(f32 x, f32 y, f32 z)
{
	dqnt_v3 result = {};
	result.x       = x;
	result.y       = y;
	result.z       = z;
	return result;
}

DQNT_FILE_SCOPE inline dqnt_v3 dqnt_vec3i(i32 x, i32 y, i32 z)
{
	dqnt_v3 result = dqnt_vec3((f32)x, (f32)y, (f32)z);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec4
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline dqnt_v4 dqnt_vec4(f32 x, f32 y, f32 z, f32 w)
{
	dqnt_v4 result = {x, y, z, w};
	return result;
}

DQNT_FILE_SCOPE inline dqnt_v4 dqnt_vec4i(i32 x, i32 y, i32 z, i32 w) {
	dqnt_v4 result = dqnt_vec4((f32)x, (f32)y, (f32)z, (f32)w);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// String Ops
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE bool dqnt_char_is_digit(char c)
{
	if (c >= '0' && c <= '9') return true;
	return false;
}

DQNT_FILE_SCOPE bool dqnt_char_is_alpha(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
	return false;
}

DQNT_FILE_SCOPE bool dqnt_char_is_alphanum(char c)
{
	if (dqnt_char_is_alpha(c) || dqnt_char_is_digit(c)) return true;
	return false;
}

DQNT_FILE_SCOPE i32 dqnt_strcmp(const char *a, const char *b)
{
	if (!a && !b) return -1;
	if (!a) return -1;
	if (!b) return -1;

	while ((*a) == (*b))
	{
		if (!(*a)) return 0;
		a++;
		b++;
	}

	return (((*a) < (*b)) ? -1 : 1);
}

DQNT_FILE_SCOPE i32 dqnt_strlen(const char *a)
{
	i32 result = 0;
	while (a && a[result]) result++;

	return result;
}

DQNT_FILE_SCOPE char *dqnt_strncpy(char *dest, const char *src, i32 numChars)
{
	if (!dest) return NULL;
	if (!src)  return dest;

	for (i32 i  = 0; i < numChars; i++)
		dest[i] = src[i];

	return dest;
}

DQNT_FILE_SCOPE bool dqnt_str_reverse(char *buf, const i32 bufSize)
{
	if (!buf) return false;
	i32 mid = bufSize / 2;

	for (i32 i = 0; i < mid; i++)
	{
		char tmp               = buf[i];
		buf[i]                 = buf[(bufSize - 1) - i];
		buf[(bufSize - 1) - i] = tmp;
	}

	return true;
}

DQNT_FILE_SCOPE i32 dqnt_str_to_i32(char *const buf, const i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	i32 index       = 0;
	bool isNegative = false;
	if (buf[index] == '-' || buf[index] == '+')
	{
		if (buf[index] == '-') isNegative = true;
		index++;
	}
	else if (!dqnt_char_is_digit(buf[index]))
	{
		return 0;
	}

	i32 result = 0;
	for (i32 i = index; i < bufSize; i++)
	{
		if (dqnt_char_is_digit(buf[i]))
		{
			result *= 10;
			result += (buf[i] - '0');
		}
		else
		{
			break;
		}
	}

	if (isNegative) result *= -1;

	return result;
}

DQNT_FILE_SCOPE void dqnt_i32_to_str(i32 value, char *buf, i32 bufSize)
{
	if (!buf || bufSize == 0) return;

	if (value == 0)
	{
		buf[0] = '0';
		return;
	}
	
	// NOTE(doyle): Max 32bit integer (+-)2147483647
	i32 charIndex = 0;
	bool negative           = false;
	if (value < 0) negative = true;

	if (negative) buf[charIndex++] = '-';

	i32 val = DQNT_MATH_ABS(value);
	while (val != 0 && charIndex < bufSize)
	{
		i32 rem          = val % 10;
		buf[charIndex++] = (u8)rem + '0';
		val /= 10;
	}

	// NOTE(doyle): If string is negative, we only want to reverse starting
	// from the second character, so we don't put the negative sign at the end
	if (negative)
	{
		dqnt_str_reverse(buf + 1, charIndex - 1);
	}
	else
	{
		dqnt_str_reverse(buf, charIndex);
	}
}

DQNT_FILE_SCOPE i32 dqnt_wstrcmp(const wchar_t *a, const wchar_t *b)
{
	if (!a && !b) return -1;
	if (!a) return -1;
	if (!b) return -1;

	while ((*a) == (*b))
	{
		if (!(*a)) return 0;
		a++;
		b++;
	}

	return (((*a) < (*b)) ? -1 : 1);
}

DQNT_FILE_SCOPE i32 dqnt_wstrlen(const wchar_t *a)
{
	i32 result = 0;
	while (a && a[result]) result++;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Timer
////////////////////////////////////////////////////////////////////////////////
#ifdef DQNT_WIN32
inline FILE_SCOPE f64 dqnt_win32_query_perf_counter_time_in_s_internal()
{
	LOCAL_PERSIST LARGE_INTEGER queryPerformanceFrequency = {};
	if (queryPerformanceFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&queryPerformanceFrequency);
		DQNT_ASSERT(queryPerformanceFrequency.QuadPart != 0);
	}

	LARGE_INTEGER qpcResult;
	QueryPerformanceCounter(&qpcResult);

	// Convert to seconds
	f64 timestamp =
	    (f64)(qpcResult.QuadPart / queryPerformanceFrequency.QuadPart);
	return timestamp;
}
#endif

f64 dqnt_time_now_in_s()
{
#ifdef _WIN32
	return dqnt_win32_query_perf_counter_time_in_s_internal();
#else
	DQNT_ASSERT(DQNT_INVALID_CODE_PATH);
	return 0;
#endif
};

f64 dqnt_time_now_in_ms()
{
	return dqnt_time_now_in_s() * 1000.0f;
}

////////////////////////////////////////////////////////////////////////////////
// PCG (Permuted Congruential Generator) Random Number Generator
////////////////////////////////////////////////////////////////////////////////
// Public Domain library with thanks to Mattias Gustavsson
// https://github.com/mattiasgustavsson/libs/blob/master/docs/rnd.md

// Convert a randomized u32 value to a float value x in the range 0.0f <= x
// < 1.0f. Contributed by Jonatan Hedborg
FILE_SCOPE f32 dqnt_rnd_f32_normalized_from_u32_internal(u32 value)
{
	u32 exponent = 127;
	u32 mantissa = value >> 9;
	u32 result   = (exponent << 23) | mantissa;
	f32 fresult  = *(f32 *)(&result);
	return fresult - 1.0f;
}

FILE_SCOPE u64 dqnt_rnd_murmur3_avalanche64_internal(u64 h)
{
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccd;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53;
	h ^= h >> 33;
	return h;
}

FILE_SCOPE u32 dqnt_rnd_make_seed_internal()
{
#ifdef _WIN32
	__int64 numClockCycles = __rdtsc();
	return (u32)numClockCycles;
#else
	unsigned long long numClockCycles = rdtsc();
	return (u32)numClockCycles;
#endif
}

DQNT_FILE_SCOPE void dqnt_rnd_pcg_init_with_seed(DqntRandPCGState *pcg, u32 seed)
{
	u64 value     = (((u64)seed) << 1ULL) | 1ULL;
	value         = dqnt_rnd_murmur3_avalanche64_internal(value);
	pcg->state[0] = 0U;
	pcg->state[1] = (value << 1ULL) | 1ULL;
	dqnt_rnd_pcg_next(pcg);
	pcg->state[0] += dqnt_rnd_murmur3_avalanche64_internal(value);
	dqnt_rnd_pcg_next(pcg);
}

DQNT_FILE_SCOPE void dqnt_rnd_pcg_init(DqntRandPCGState *pcg)
{
	u32 seed = dqnt_rnd_make_seed_internal();
	dqnt_rnd_pcg_init_with_seed(pcg, seed);
}

DQNT_FILE_SCOPE u32 dqnt_rnd_pcg_next(DqntRandPCGState *pcg)
{
	u64 oldstate   = pcg->state[0];
	pcg->state[0]  = oldstate * 0x5851f42d4c957f2dULL + pcg->state[1];
	u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
	u32 rot        = (u32)(oldstate >> 59ULL);
	return (xorshifted >> rot) | (xorshifted << ((-(i32)rot) & 31));
}

DQNT_FILE_SCOPE f32 dqnt_rnd_pcg_nextf(DqntRandPCGState *pcg)
{
	return dqnt_rnd_f32_normalized_from_u32_internal(dqnt_rnd_pcg_next(pcg));
}

DQNT_FILE_SCOPE i32 dqnt_rnd_pcg_range(DqntRandPCGState *pcg, i32 min, i32 max)
{
	i32 const range = (max - min) + 1;
	if (range <= 0) return min;
	i32 const value = (i32)(dqnt_rnd_pcg_nextf(pcg) * range);
	return min + value;
}

#endif /* DQNT_IMPLEMENTATION */
