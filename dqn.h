#ifndef DQN_H
#define DQN_H

/*
	#define DQN_IMPLEMENTATION // Enable the implementation
	#define DQN_MAKE_STATIC    // Make all functions be static
	#include "dqn.h"
 */

#ifdef DQN_MAKE_STATIC
	#define DQN_FILE_SCOPE static
#else
	#define DQN_FILE_SCOPE
#endif

////////////////////////////////////////////////////////////////////////////////
//
// HEADER
//
////////////////////////////////////////////////////////////////////////////////
#include "stdint.h"
#include "math.h"
#define STB_SPRINTF_IMPLEMENTATION

#ifdef _WIN32
	#define DQN_WIN32_ERROR_BOX(text, title) MessageBoxA(NULL, text, title, MB_OK);
#endif

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

#define DQN_INVALID_CODE_PATH 0
#define DQN_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))
#define DQN_ASSERT(expr) if (!(expr)) { (*((i32 *)0)) = 0; }

#define DQN_PI 3.14159265359f
#define DQN_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQN_DEGREES_TO_RADIANS(x) ((x * (DQN_PI / 180.0f)))
#define DQN_RADIANS_TO_DEGREES(x) ((x * (180.0f / DQN_PI)))
#define DQN_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQN_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQN_SQUARED(x) ((x) * (x))

////////////////////////////////////////////////////////////////////////////////
// DArray - Dynamic Array
////////////////////////////////////////////////////////////////////////////////
template <typename T>
struct DqnArray
{
	u64 count;
	u64 capacity;
	T *data;
};

template <typename T>
bool  dqn_darray_init (DqnArray<T> *array, size_t capacity);
template <typename T>
bool  dqn_darray_grow (DqnArray<T> *array);
template <typename T>
bool dqn_darray_push(DqnArray<T> *array, T item);
template <typename T>
T    *dqn_darray_get  (DqnArray<T> *array, u64 index);
template <typename T>
bool  dqn_darray_clear(DqnArray<T> *array);
template <typename T>
bool  dqn_darray_free (DqnArray<T> *array);
template <typename T>
bool  dqn_darray_remove(DqnArray<T> *array, u64 index);
template <typename T>
bool  dqn_darray_remove_stable(DqnArray<T> *array, u64 index);

////////////////////////////////////////////////////////////////////////////////
// Math
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f32 dqn_math_lerp(f32 a, f32 t, f32 b);
DQN_FILE_SCOPE f32 dqn_math_sqrtf(f32 a);

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV2 {
	struct { f32 x, y; };
	struct { f32 w, h; };
	struct { f32 min, max; };
	f32 e[2];
} DqnV2;

// Create a vector using ints and typecast to floats
DQN_FILE_SCOPE DqnV2 dqn_v2i(i32 x, i32 y);
DQN_FILE_SCOPE DqnV2 dqn_v2 (f32 x, f32 y);

DQN_FILE_SCOPE DqnV2 dqn_v2_add     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 dqn_v2_sub     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 dqn_v2_scale   (DqnV2 a, f32 b);
DQN_FILE_SCOPE DqnV2 dqn_v2_hadamard(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   dqn_v2_dot     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE bool  dqn_v2_equals  (DqnV2 a, DqnV2 b);

DQN_FILE_SCOPE f32   dqn_v2_length_squared(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   dqn_v2_length        (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 dqn_v2_normalise     (DqnV2 a);
DQN_FILE_SCOPE bool  dqn_v2_overlaps      (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 dqn_v2_perpendicular (DqnV2 a);

// Resize the dimension to fit the aspect ratio provided. Downscale only.
DQN_FILE_SCOPE DqnV2 dqn_v2_constrain_to_ratio(DqnV2 dim, DqnV2 ratio);

////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV3
{
	struct { f32 x, y, z; };
    struct { f32 r, g, b; };
	f32 e[3];
} DqnV3;

// Create a vector using ints and typecast to floats
DQN_FILE_SCOPE DqnV3 dqn_v3i(i32 x, i32 y, i32 z);
DQN_FILE_SCOPE DqnV3 dqn_v3 (f32 x, f32 y, f32 z);

DQN_FILE_SCOPE DqnV3 dqn_v3_add     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 dqn_v3_sub     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 dqn_v3_scale   (DqnV3 a, f32 b);
DQN_FILE_SCOPE DqnV3 dqn_v3_hadamard(DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   dqn_v3_dot     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE bool  dqn_v3_equals  (DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE DqnV3 dqn_v3_cross(DqnV3 a, DqnV3 b);

////////////////////////////////////////////////////////////////////////////////
// Vec4
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV4
{
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
	f32 e[4];
	DqnV2 v2[2];
} DqnV4;

// Create a vector using ints and typecast to floats
DQN_FILE_SCOPE DqnV4 dqn_v4i(i32 x, i32 y, i32 z, f32 w);
DQN_FILE_SCOPE DqnV4 dqn_v4 (f32 x, f32 y, f32 z, f32 w);

DQN_FILE_SCOPE DqnV4 dqn_v4_add     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 dqn_v4_sub     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 dqn_v4_scale   (DqnV4 a, f32 b);
DQN_FILE_SCOPE DqnV4 dqn_v4_hadamard(DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE f32   dqn_v4_dot     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE bool  dqn_v4_equals  (DqnV4 a, DqnV4 b);

////////////////////////////////////////////////////////////////////////////////
// 4D Matrix Mat4
////////////////////////////////////////////////////////////////////////////////
typedef union DqnMat4
{
	DqnV4 col[4];
	// Column/row
	f32 e[4][4];
} DqnMat4;

DQN_FILE_SCOPE DqnMat4 dqn_mat4_identity ();
DQN_FILE_SCOPE DqnMat4 dqn_mat4_ortho    (f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
DQN_FILE_SCOPE DqnMat4 dqn_mat4_translate(f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 dqn_mat4_rotate   (f32 radians, f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 dqn_mat4_scale    (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 dqn_mat4_mul      (DqnMat4 a, DqnMat4 b);
DQN_FILE_SCOPE DqnV4   dqn_mat4_mul_vec4 (DqnMat4 a, DqnV4 b);

////////////////////////////////////////////////////////////////////////////////
// Other Math
////////////////////////////////////////////////////////////////////////////////
typedef struct DqnRect
{
	DqnV2 min;
	DqnV2 max;
} DqnRect;

DQN_FILE_SCOPE DqnRect dqn_rect            (DqnV2 origin, DqnV2 size);
DQN_FILE_SCOPE void    dqn_rect_get_size_2f(DqnRect rect, f32 *width, f32 *height);
DQN_FILE_SCOPE DqnV2   dqn_rect_get_size_v2(DqnRect rect);
DQN_FILE_SCOPE DqnV2   dqn_rect_get_centre (DqnRect rect);
DQN_FILE_SCOPE DqnRect dqn_rect_move       (DqnRect rect, DqnV2 shift);
DQN_FILE_SCOPE bool    dqn_rect_contains_p (DqnRect rect, DqnV2 p);

////////////////////////////////////////////////////////////////////////////////
// String Ops
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool  dqn_char_is_digit   (char c);
DQN_FILE_SCOPE bool  dqn_char_is_alpha   (char c);
DQN_FILE_SCOPE bool  dqn_char_is_alphanum(char c);

DQN_FILE_SCOPE i32   dqn_strcmp (const char *a, const char *b);
// Returns the length without the null terminator
DQN_FILE_SCOPE i32   dqn_strlen (const char *a);
DQN_FILE_SCOPE char *dqn_strncpy(char *dest, const char *src, i32 numChars);

#define DQN_I32_TO_STR_MAX_BUF_SIZE 11
DQN_FILE_SCOPE bool  dqn_str_reverse(char *buf, const i32 bufSize);
DQN_FILE_SCOPE i32   dqn_str_to_i32 (char *const buf, const i32 bufSize);
DQN_FILE_SCOPE void  dqn_i32_to_str (i32 value, char *buf, i32 bufSize);

// Both return the number of bytes read, return 0 if invalid codepoint or UTF8
DQN_FILE_SCOPE u32 dqn_ucs_to_utf8(u32 *dest, u32 character);
DQN_FILE_SCOPE u32 dqn_utf8_to_ucs(u32 *dest, u32 character);

////////////////////////////////////////////////////////////////////////////////
// Win32 Specific
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool dqn_win32_utf8_to_wchar(char *in, wchar_t *out, i32 outLen);
DQN_FILE_SCOPE bool dqn_win32_wchar_to_utf8(wchar_t *in, char *out, i32 outLen);

////////////////////////////////////////////////////////////////////////////////
// File Operations
////////////////////////////////////////////////////////////////////////////////
typedef struct DqnFile
{
	void *handle;
	u64   size;
} DqnFile;

// Open a handle to the file
DQN_FILE_SCOPE bool dqn_file_open(char *const file, DqnFile *fileHandle);

// Return the number of bytes read
DQN_FILE_SCOPE u32  dqn_file_read (DqnFile file, void *buffer, u32 numBytesToRead);
DQN_FILE_SCOPE void dqn_file_close(DqnFile *file);

// Return an array of strings of the files in the directory in UTF-8. numFiles
// returns the number of strings read.
// This is allocated using malloc and MUST BE FREED! Can be done manually or
// using the helper function.
DQN_FILE_SCOPE char **dqn_dir_read     (char *dir, u32 *numFiles);
DQN_FILE_SCOPE void   dqn_dir_read_free(char **fileList, u32 numFiles);

////////////////////////////////////////////////////////////////////////////////
// Timer
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f64 dqn_time_now_in_s();
DQN_FILE_SCOPE f64 dqn_time_now_in_ms();

////////////////////////////////////////////////////////////////////////////////
// PCG (Permuted Congruential Generator) Random Number Generator
////////////////////////////////////////////////////////////////////////////////
typedef struct DqnRandPCGState
{
	u64 state[2];
} DqnRandPCGState;

// Initialise the random number generator using a seed. If not given it is
// automatically created by using rdtsc. The generator is not valid until it's
// been seeded.
DQN_FILE_SCOPE void dqn_rnd_pcg_init_with_seed(DqnRandPCGState *pcg, u32 seed);
DQN_FILE_SCOPE void dqn_rnd_pcg_init(DqnRandPCGState *pcg);

// Returns a random number N between [0, 0xFFFFFFFF]
DQN_FILE_SCOPE u32  dqn_rnd_pcg_next (DqnRandPCGState *pcg);
// Returns a random float N between [0.0, 1.0f]
DQN_FILE_SCOPE f32  dqn_rnd_pcg_nextf(DqnRandPCGState *pcg);
// Returns a random integer N between [min, max]
DQN_FILE_SCOPE i32  dqn_rnd_pcg_range(DqnRandPCGState *pcg, i32 min, i32 max);

#endif  /* DQN_H */

////////////////////////////////////////////////////////////////////////////////
//
// IMPLEMENTATION
//
////////////////////////////////////////////////////////////////////////////////
#ifdef DQN_IMPLEMENTATION
#undef DQN_IMPLEMENTATION

// Enable sprintf implementation only when we enable DQN implementation
#define STB_SPRINTF_IMPLEMENTATION

#ifdef _WIN32
	#define DQN_WIN32

	#include "Windows.h"
	#define WIN32_LEAN_AND_MEAN
#endif

////////////////////////////////////////////////////////////////////////////////
// DArray - Dynamic Array
////////////////////////////////////////////////////////////////////////////////
// Implementation taken from Milton, developed by Serge at
// https://github.com/serge-rgb/milton#license
template <typename T>
bool dqn_darray_init(DqnArray<T> *array, size_t capacity)
{
	if (!array) return false;

	array->data     = (T *)calloc((size_t)capacity, sizeof(T));
	if (!array->data) return false;

	array->count    = 0;
	array->capacity = capacity;
	return true;
}

template <typename T>
bool dqn_darray_grow(DqnArray<T> *array)
{
	if (!array || !array->data) return false;

	const f32 GROWTH_FACTOR = 1.2f;
	size_t newCapacity         = (size_t)(array->capacity * GROWTH_FACTOR);
	if (newCapacity == array->capacity) newCapacity++;

	T *newMem = (T *)realloc(array->data, (size_t)(newCapacity * sizeof(T)));
	if (newMem)
	{
		array->data     = newMem;
		array->capacity = newCapacity;
		return true;
	}
	else
	{
		return false;
	}
}

template <typename T>
bool dqn_darray_push(DqnArray<T> *array, T item)
{
	if (!array) return false;

	if (array->count >= array->capacity)
	{
		if (!dqn_darray_grow(array)) return false;
	}

	DQN_ASSERT(array->count < array->capacity);
	array->data[array->count++] = item;

	return true;
}

template <typename T>
T *dqn_darray_get(DqnArray<T> *array, u64 index)
{
	T *result = NULL;
	if (index >= 0 && index <= array->count) result = &array->data[index];
	return result;
}

template <typename T>
bool dqn_darray_clear(DqnArray<T> *array)
{
	if (array)
	{
		array->count = 0;
		return true;
	}

	return false;
}

template <typename T>
bool dqn_darray_free(DqnArray<T> *array)
{
	if (array && array->data)
	{
		free(array->data);
		array->count    = 0;
		array->capacity = 0;
		return true;
	}

	return false;
}

template <typename T>
bool dqn_darray_remove(DqnArray<T> *array, u64 index)
{
	if (!array) return false;
	if (index >= array->count) return false;

	bool firstElementAndOnlyElement = (index == 0 && array->count == 1);
	bool isLastElement              = (index == (array->count - 1));
	if (firstElementAndOnlyElement || isLastElement)
	{
		array->count--;
		return true;
	}

	array->data[index] = array->data[array->count - 1];
	array->count--;
	return true;
}

template <typename T>
bool dqn_darray_remove_stable(DqnArray<T> *array, u64 index)
{
	if (!array) return false;
	if (index >= array->count) return false;

	bool firstElementAndOnlyElement = (index == 0 && array->count == 1);
	bool isLastElement              = (index == (array->count - 1));
	if (firstElementAndOnlyElement || isLastElement)
	{
		array->count--;
		return true;
	}

	size_t itemToRemoveByteOffset         = (size_t)(index * sizeof(T));
	size_t oneAfterItemToRemoveByteOffset = (size_t)((index + 1) * sizeof(T));
	size_t lastItemByteOffset = (size_t)(array->count * sizeof(T));
	size_t numBytesToMove = lastItemByteOffset - oneAfterItemToRemoveByteOffset;

	u8 *bytePtr = (u8 *)array->data;
	u8 *dest    = &bytePtr[itemToRemoveByteOffset];
	u8 *src     = &bytePtr[oneAfterItemToRemoveByteOffset];
	memmove(dest, src, numBytesToMove);

	array->count--;
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// Math
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f32 dqn_math_lerp(f32 a, f32 t, f32 b)
{
	/*
	    Linear blend between two values. We having a starting point "a", and
	    the distance to "b" is defined as (b - a). Then we can say

	    a + t(b - a)

	    As our linear blend fn. We start from "a" and choosing a t from 0->1
	    will vary the value of (b - a) towards b. If we expand this, this
	    becomes

	    a + (t * b) - (a * t) == (1 - t)a + t*b
	*/
	f32 result =  a + (b - a) * t;
	return result;
}

DQN_FILE_SCOPE f32 dqn_math_sqrtf(f32 a)
{
	f32 result = sqrtf(a);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE inline DqnV2 dqn_v2(f32 x, f32 y)
{
	DqnV2 result = {};
	result.x  = x;
	result.y  = y;

	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2i(i32 x, i32 y)
{
	DqnV2 result = dqn_v2((f32)x, (f32)y);
	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_add(DqnV2 a, DqnV2 b)
{
	DqnV2 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_sub(DqnV2 a, DqnV2 b)
{
	DqnV2 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_scale(DqnV2 a, f32 b)
{
	DqnV2 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_hadamard(DqnV2 a, DqnV2 b)
{
	DqnV2 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE inline f32 dqn_v2_dot(DqnV2 a, DqnV2 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE inline bool dqn_v2_equals(DqnV2 a, DqnV2 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

DQN_FILE_SCOPE inline f32 dqn_v2_length_squared(DqnV2 a, DqnV2 b)
{
	f32 x      = b.x - a.x;
	f32 y      = b.y - a.y;
	f32 result = (DQN_SQUARED(x) + DQN_SQUARED(y));
	return result;
}

DQN_FILE_SCOPE inline f32 dqn_v2_length(DqnV2 a, DqnV2 b)
{
	f32 lengthSq = dqn_v2_length_squared(a, b);
	f32 result   = dqn_math_sqrtf(lengthSq);
	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_normalise(DqnV2 a)
{
	f32 magnitude = dqn_v2_length(dqn_v2(0, 0), a);
	DqnV2 result = dqn_v2(a.x, a.y);
	result        = dqn_v2_scale(a, 1 / magnitude);
	return result;
}

DQN_FILE_SCOPE inline bool dqn_v2_overlaps(DqnV2 a, DqnV2 b)
{
	bool result = false;
	
	f32 lenOfA = a.max - a.min;
	f32 lenOfB = b.max - b.min;

	if (lenOfA > lenOfB)
	{
		DqnV2 tmp = a;
		a          = b;
		b          = tmp;
	}

	if ((a.min >= b.min && a.min <= b.max) ||
	    (a.max >= b.min && a.max <= b.max))
	{
		result = true;
	}

	return result;
}

DQN_FILE_SCOPE inline DqnV2 dqn_v2_perpendicular(DqnV2 a)
{
	DqnV2 result = {a.y, -a.x};
	return result;
}


DQN_FILE_SCOPE DqnV2 dqn_v2_constrain_to_ratio(DqnV2 dim, DqnV2 ratio)
{
	DqnV2 result                  = {};
	f32 numRatioIncrementsToWidth  = (f32)(dim.w / ratio.w);
	f32 numRatioIncrementsToHeight = (f32)(dim.h / ratio.h);

	f32 leastIncrementsToSide =
	    DQN_MIN(numRatioIncrementsToHeight, numRatioIncrementsToWidth);

	result.w = (f32)(ratio.w * leastIncrementsToSide);
	result.h = (f32)(ratio.h * leastIncrementsToSide);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE inline DqnV3 dqn_v3(f32 x, f32 y, f32 z)
{
	DqnV3 result = {};
	result.x       = x;
	result.y       = y;
	result.z       = z;
	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3i(i32 x, i32 y, i32 z)
{
	DqnV3 result = dqn_v3((f32)x, (f32)y, (f32)z);
	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3_add(DqnV3 a, DqnV3 b)
{
	DqnV3 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3_sub(DqnV3 a, DqnV3 b)
{
	DqnV3 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3_scale(DqnV3 a, f32 b)
{
	DqnV3 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3_hadamard(DqnV3 a, DqnV3 b)
{
	DqnV3 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE inline f32 dqn_v3_dot(DqnV3 a, DqnV3 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE inline bool dqn_v3_equals(DqnV3 a, DqnV3 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

DQN_FILE_SCOPE inline DqnV3 dqn_v3_cross(DqnV3 a, DqnV3 b)
{
	/*
	   CROSS PRODUCT
	   Generate a perpendicular vector to the 2 vectors
	   |a|   |d|   |bf - ce|
	   |b| x |e| = |cd - af|
	   |c|   |f|   |ae - be|
	 */
	DqnV3 result = {};
	result.e[0] = (a.e[1] * b.e[2]) - (a.e[2] * b.e[1]);
	result.e[1] = (a.e[2] * b.e[0]) - (a.e[0] * b.e[2]);
	result.e[2] = (a.e[0] * b.e[1]) - (a.e[1] * b.e[0]);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec4
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE inline DqnV4 dqn_v4(f32 x, f32 y, f32 z, f32 w)
{
	DqnV4 result = {x, y, z, w};
	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_v4i(i32 x, i32 y, i32 z, i32 w) {
	DqnV4 result = dqn_v4((f32)x, (f32)y, (f32)z, (f32)w);
	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_v4_add(DqnV4 a, DqnV4 b)
{
	DqnV4 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_v4_sub(DqnV4 a, DqnV4 b)
{
	DqnV4 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_v4_scale(DqnV4 a, f32 b)
{
	DqnV4 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_v4_hadamard(DqnV4 a, DqnV4 b)
{
	DqnV4 result;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE inline f32 dqn_v4_dot(DqnV4 a, DqnV4 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE inline bool dqn_v4_equals(DqnV4 a, DqnV4 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// 4D Matrix Mat4
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE inline DqnMat4 dqn_mat4_identity()
{
	DqnMat4 result    = {0};
	result.e[0][0] = 1;
	result.e[1][1] = 1;
	result.e[2][2] = 1;
	result.e[3][3] = 1;
	return result;
}

DQN_FILE_SCOPE inline DqnMat4
dqn_mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
	DqnMat4 result = dqn_mat4_identity();
	result.e[0][0] = +2.0f   / (right - left);
	result.e[1][1] = +2.0f   / (top   - bottom);
	result.e[2][2] = -2.0f   / (zFar  - zNear);

	result.e[3][0] = -(right + left)   / (right - left);
	result.e[3][1] = -(top   + bottom) / (top   - bottom);
	result.e[3][2] = -(zFar  + zNear)  / (zFar  - zNear);

	return result;
}

DQN_FILE_SCOPE inline DqnMat4 dqn_mat4_translate(f32 x, f32 y, f32 z)
{
	DqnMat4 result = dqn_mat4_identity();
	result.e[3][0] = x;
	result.e[3][1] = y;
	result.e[3][2] = z;
	return result;
}

DQN_FILE_SCOPE inline DqnMat4 dqn_mat4_rotate(f32 radians, f32 x, f32 y, f32 z)
{
	DqnMat4 result = dqn_mat4_identity();
	f32 sinVal = sinf(radians);
	f32 cosVal = cosf(radians);

	result.e[0][0] = (cosVal + (DQN_SQUARED(x) * (1.0f - cosVal)));
	result.e[0][1] = ((y * z * (1.0f - cosVal)) + (z * sinVal));
	result.e[0][2] = ((z * x * (1.0f - cosVal)) - (y * sinVal));

	result.e[1][0] = ((x * y * (1.0f - cosVal)) - (z * sinVal));
	result.e[1][1] = (cosVal + (DQN_SQUARED(y) * (1.0f - cosVal)));
	result.e[1][2] = ((z * y * (1.0f - cosVal)) + (x * sinVal));

	result.e[2][0] = ((x * z * (1.0f - cosVal)) + (y * sinVal));
	result.e[2][1] = ((y * z * (1.0f - cosVal)) - (x * sinVal));
	result.e[2][2] = (cosVal + (DQN_SQUARED(z) * (1.0f - cosVal)));

	result.e[3][3] = 1;

	return result;
}

DQN_FILE_SCOPE inline DqnMat4 dqn_mat4_scale(f32 x, f32 y, f32 z)
{
	DqnMat4 result = {0};
	result.e[0][0] = x;
	result.e[1][1] = y;
	result.e[2][2] = z;
	result.e[3][3] = 1;
	return result;
}

DQN_FILE_SCOPE inline DqnMat4 dqn_mat4_mul(DqnMat4 a, DqnMat4 b)
{
	DqnMat4 result = {0};
	for (i32 j = 0; j < 4; j++) {
		for (i32 i = 0; i < 4; i++) {
			result.e[j][i] = a.e[0][i] * b.e[j][0]
			               + a.e[1][i] * b.e[j][1]
			               + a.e[2][i] * b.e[j][2]
			               + a.e[3][i] * b.e[j][3];
		}
	}

	return result;
}

DQN_FILE_SCOPE inline DqnV4 dqn_mat4_mul_vec4(DqnMat4 a, DqnV4 b)
{
	DqnV4 result = {0};

	result.x = (a.e[0][0] * b.x) + (a.e[1][0] * b.y) + (a.e[2][0] * b.z) +
	           (a.e[3][0] * b.w);
	result.y = (a.e[0][1] * b.x) + (a.e[1][1] * b.y) + (a.e[2][1] * b.z) +
	           (a.e[3][1] * b.w);
	result.z = (a.e[0][2] * b.x) + (a.e[1][2] * b.y) + (a.e[2][2] * b.z) +
	           (a.e[3][2] * b.w);
	result.w = (a.e[0][3] * b.x) + (a.e[1][3] * b.y) + (a.e[2][3] * b.z) +
	           (a.e[3][3] * b.w);

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Rect
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE inline DqnRect dqn_rect(DqnV2 origin, DqnV2 size)
{
	DqnRect result = {};
	result.min      = origin;
	result.max      = dqn_v2_add(origin, size);

	return result;
}

DQN_FILE_SCOPE inline void dqn_rect_get_size_2f(DqnRect rect, f32 *width, f32 *height)
{
	*width  = DQN_ABS(rect.max.x - rect.min.x);
	*height = DQN_ABS(rect.max.y - rect.min.y);
}

DQN_FILE_SCOPE inline DqnV2 dqn_rect_get_size_v2(DqnRect rect)
{
	f32 width     = DQN_ABS(rect.max.x - rect.min.x);
	f32 height    = DQN_ABS(rect.max.y - rect.min.y);
	DqnV2 result = dqn_v2(width, height);
	return result;
}


DQN_FILE_SCOPE inline DqnV2 dqn_rect_get_centre(DqnRect rect)
{
	f32 sumX  = rect.min.x + rect.max.x;
	f32 sumY  = rect.min.y + rect.max.y;
	DqnV2 result = dqn_v2_scale(dqn_v2(sumX, sumY), 0.5f);
	return result;
}

DQN_FILE_SCOPE inline DqnRect dqn_rect_move(DqnRect rect, DqnV2 shift)
{
	DqnRect result = {0};
	result.min       = dqn_v2_add(rect.min, shift);
	result.max       = dqn_v2_add(rect.max, shift);

	return result;
}

DQN_FILE_SCOPE inline bool dqn_rect_contains_p(DqnRect rect, DqnV2 p)
{
	bool outsideOfRectX = false;
	if (p.x < rect.min.x || p.x > rect.max.w)
		outsideOfRectX = true;

	bool outsideOfRectY = false;
	if (p.y < rect.min.y || p.y > rect.max.h)
		outsideOfRectY = true;

	if (outsideOfRectX || outsideOfRectY) return false;

	return true;
}

////////////////////////////////////////////////////////////////////////////////
// String Operations
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool dqn_char_is_digit(char c)
{
	if (c >= '0' && c <= '9') return true;
	return false;
}

DQN_FILE_SCOPE bool dqn_char_is_alpha(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
	return false;
}

DQN_FILE_SCOPE bool dqn_char_is_alphanum(char c)
{
	if (dqn_char_is_alpha(c) || dqn_char_is_digit(c)) return true;
	return false;
}

DQN_FILE_SCOPE i32 dqn_strcmp(const char *a, const char *b)
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

DQN_FILE_SCOPE i32 dqn_strlen(const char *a)
{
	i32 result = 0;
	while (a && a[result]) result++;

	return result;
}

DQN_FILE_SCOPE char *dqn_strncpy(char *dest, const char *src, i32 numChars)
{
	if (!dest) return NULL;
	if (!src)  return dest;

	for (i32 i  = 0; i < numChars; i++)
		dest[i] = src[i];

	return dest;
}

DQN_FILE_SCOPE bool dqn_str_reverse(char *buf, const i32 bufSize)
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

DQN_FILE_SCOPE i32 dqn_str_to_i32(char *const buf, const i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	i32 index       = 0;
	bool isNegative = false;
	if (buf[index] == '-' || buf[index] == '+')
	{
		if (buf[index] == '-') isNegative = true;
		index++;
	}
	else if (!dqn_char_is_digit(buf[index]))
	{
		return 0;
	}

	i32 result = 0;
	for (i32 i = index; i < bufSize; i++)
	{
		if (dqn_char_is_digit(buf[i]))
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

DQN_FILE_SCOPE void dqn_i32_to_str(i32 value, char *buf, i32 bufSize)
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

	i32 val = DQN_ABS(value);
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
		dqn_str_reverse(buf + 1, charIndex - 1);
	}
	else
	{
		dqn_str_reverse(buf, charIndex);
	}
}

/*
	Encoding
	The following byte sequences are used to represent a character. The sequence
	to be used depends on the UCS code number of the character:

	The extra 1's are the headers used to identify the string as a UTF-8 string.
	UCS [0x00000000, 0x0000007F] -> UTF-8 0xxxxxxx
	UCS [0x00000080, 0x000007FF] -> UTF-8 110xxxxx 10xxxxxx
	UCS [0x00000800, 0x0000FFFF] -> UTF-8 1110xxxx 10xxxxxx 10xxxxxx
	UCS [0x00010000, 0x001FFFFF] -> UTF-8 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
	UCS [0x00200000, 0x03FFFFFF] -> N/A   111110xx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx
	UCS [0x04000000, 0x7FFFFFFF] -> N/A   1111110x 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx 10xxxxxx

	The xxx bit positions are filled with the bits of the character code number
	in binary representation. Only the shortest possible multibyte sequence
	which can represent the code number of the character can be used.

	The UCS code values 0xd800–0xdfff (UTF-16 surrogates) as well as 0xfffe and
	0xffff (UCS noncharacters) should not appear in conforming UTF-8 streams.
*/
DQN_FILE_SCOPE u32 dqn_ucs_to_utf8(u32 *dest, u32 character)
{
	if (!dest) return 0;

	u8 *bytePtr = (u8 *)dest;

	// Character is within ASCII range, so it's an ascii character
	// UTF Bit Arrangement: 0xxxxxxx
	// Character          : 0xxxxxxx
	if (character >= 0 && character < 0x80)
	{
		bytePtr[0] = (u8)character;
		return 1;
	}

	// UTF Header Bits    : 11000000 00xxxxxx
	// UTF Bit Arrangement: 000xxxxx 00xxxxxx
	// Character          : 00000xxx xxxxxxxx
	if (character < 0x800)
	{
		// Add the 2nd byte, 6 bits, OR the 0xC0 (11000000) header bits
		bytePtr[1] = (u8)((character >> 6) | 0xC0);

		// Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
		bytePtr[0] = (u8)((character & 0x3F) | 0x80);

		return 2;
	}

	// UTF Header Bits     : 11100000 10000000 10000000
	// UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
	// Character           : 00000000 xxxxxxxx xxxxxxxx
	if (character < 0x10000)
	{
		// Add the 3rd byte, 4 bits, OR the 0xE0 (11100000) header bits
		bytePtr[2] = (u8)((character >> 12) | 0xE0);

		// Add the 2nd byte, 6 bits, OR the 0x80 (10000000) header bits
		bytePtr[1] = (u8)((character >> 6) | 0x80);

		// Add the 1st byte, 6 bits, plus the 0x80 (10000000) header bits
		bytePtr[0] = (u8)((character & 0x3F) | 0x80);

		return 3;
	}

	// UTF Header Bits     : 11110000 10000000 10000000 10000000
	// UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
	// Character           : 00000000 00000xxx xxxxxxxx xxxxxxxx
	if (character < 0x110000)
	{
		// Add the 4th byte, 3 bits, OR the 0xF0 (11110000) header bits
		bytePtr[3] = (u8)((character >> 18) | 0xF0);

		// Add the 3rd byte, 6 bits, OR the 0x80 (10000000) header bits
		bytePtr[2] = (u8)(((character >> 12) & 0x3F) | 0x80);

		// Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
		bytePtr[1] = (u8)(((character >> 6) & 0x3F) | 0x80);

		// Add the 2nd byte, 6 bits, plus the 0x80 (10000000) header bits
		bytePtr[0] = (u8)((character & 0x3F) | 0x80);

		return 4;
	}

	return 0;
}

DQN_FILE_SCOPE u32 dqn_utf8_to_ucs(u32 *dest, u32 character)
{
	if (!dest) return 0;

	// UTF Header Bits     : 11110000 10000000 10000000 10000000
	// UTF Bit Arrangement : 00000xxx 00xxxxxx 00xxxxxx 00xxxxxx
	// UCS                 : 00000000 00000xxx xxxxxxxx xxxxxxxx
	const u32 headerBits4Bytes = 0xF0808080;
	if ((character & headerBits4Bytes) == headerBits4Bytes)
	{
		u32 utfWithoutHeader = headerBits4Bytes ^ character;

		u32 firstByte  = utfWithoutHeader & 0x3F;
		u32 secondByte = (utfWithoutHeader >> 8)  & 0x3F;
		u32 thirdByte  = (utfWithoutHeader >> 16) & 0x3F;
		u32 fourthByte = utfWithoutHeader >> 24;

		u32 result =
		    (fourthByte << 18 | thirdByte << 12 | secondByte << 6 | firstByte);
		*dest = result;

		return 4;
	}

	// UTF Header Bits     : 11100000 10000000 10000000
	// UTF Bit Arrangement : 0000xxxx 00xxxxxx 00xxxxxx
	// UCS                 : 00000000 xxxxxxxx xxxxxxxx
	const u32 headerBits3Bytes = 0xE08080;
	if ((character & headerBits3Bytes)  == headerBits3Bytes)
	{
		u32 utfWithoutHeader = headerBits3Bytes ^ character;

		u32 firstByte  = utfWithoutHeader & 0x3F;
		u32 secondByte = (utfWithoutHeader >> 8) & 0x3F;
		u32 thirdByte  = utfWithoutHeader >> 16;

		u32 result = (thirdByte << 12 | secondByte << 6 | firstByte);
		*dest = result;

		return 3;
	}

	// UTF Header Bits    : 11000000 00xxxxxx
	// UTF Bit Arrangement: 000xxxxx 00xxxxxx
	// UCS                : 00000xxx xxxxxxxx
	const u32 headerBits2Bytes = 0xC000;
	if ((character & headerBits2Bytes) == headerBits2Bytes)
	{
		u32 utfWithoutHeader = headerBits2Bytes ^ character;

		u32 firstByte  = utfWithoutHeader & 0x3F;
		u32 secondByte = utfWithoutHeader >> 8;

		u32 result = (secondByte << 6 | firstByte);
		*dest = result;

		return 2;
	}

	// Character is within ASCII range, so it's an ascii character
	// UTF Bit Arrangement: 0xxxxxxx
	// UCS                : 0xxxxxxx
	if (character >= 0x0 && character < 0x80)
	{
		u32 firstByte = (character & 0x3F);
		*dest         = firstByte;

		return 1;
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// File Operations
////////////////////////////////////////////////////////////////////////////////
#ifdef DQN_WIN32

DQN_FILE_SCOPE bool dqn_win32_utf8_to_wchar(char *in, wchar_t *out, i32 outLen)
{
	u32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, outLen-1);

	if (result == 0xFFFD || 0)
	{
		DQN_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}

DQN_FILE_SCOPE bool dqn_win32_wchar_to_utf8(wchar_t *in, char *out, i32 outLen)
{
	u32 result =
	    WideCharToMultiByte(CP_UTF8, 0, in, -1, out, outLen, NULL, NULL);

	if (result == 0xFFFD || 0)
	{
		DQN_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}
#endif

DQN_FILE_SCOPE bool dqn_file_open(char *const path, DqnFile *file)
{
	if (!file || !path) return false;

#ifdef DQN_WIN32
	wchar_t widePath[MAX_PATH] = {};
	dqn_win32_utf8_to_wchar(path, widePath, DQN_ARRAY_COUNT(widePath));

	HANDLE handle = CreateFileW(widePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
	                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		DQN_WIN32_ERROR_BOX("CreateFile() failed.", NULL);
		return false;
	}

	LARGE_INTEGER size;
	if (GetFileSizeEx(handle, &size) == 0)
	{
		DQN_WIN32_ERROR_BOX("GetFileSizeEx() failed.", NULL);
		return false;
	}

	file->handle = handle;
	file->size   = size.QuadPart;

#else
	return false;
#endif

	return true;
}

DQN_FILE_SCOPE u32 dqn_file_read(DqnFile file, u8 *buffer, u32 numBytesToRead)
{
	u32 numBytesRead = 0;
#ifdef DQN_WIN32
	if (file.handle && buffer)
	{
		DWORD bytesRead    = 0;
		HANDLE win32Handle = file.handle;

		BOOL result = ReadFile(win32Handle, (void *)buffer, numBytesToRead,
		                       &bytesRead, NULL);

		// TODO(doyle): 0 also means it is completing async, but still valid
		if (result == 0)
		{
			DQN_WIN32_ERROR_BOX("ReadFile() failed.", NULL);
		}

		numBytesRead = (u32)bytesRead;
	}
#endif

	return numBytesRead;
}

DQN_FILE_SCOPE inline void dqn_file_close(DqnFile *file)
{
#ifdef DQN_WIN32
	if (file && file->handle)
	{
		CloseHandle(file->handle);
		file->handle = NULL;
		file->size   = 0;
	}
#endif
}

DQN_FILE_SCOPE char **dqn_dir_read(char *dir, u32 *numFiles)
{
	if (!dir) return NULL;
#ifdef DQN_WIN32

	u32 currNumFiles = 0;
	wchar_t wideDir[MAX_PATH] = {};
	dqn_win32_utf8_to_wchar(dir, wideDir, DQN_ARRAY_COUNT(wideDir));

	// Enumerate number of files first
	{
		WIN32_FIND_DATAW findData = {};
		HANDLE findHandle = FindFirstFileW(wideDir, &findData);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			DQN_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
			return NULL;
		}

		while (FindNextFileW(findHandle, &findData) != 0)
			currNumFiles++;
		FindClose(findHandle);
	}

	if (currNumFiles == 0) return NULL;

	{
		WIN32_FIND_DATAW initFind = {};
		HANDLE findHandle = FindFirstFileW(wideDir, &initFind);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			DQN_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
			return NULL;
		}


		char **list = (char **)calloc(1, sizeof(*list) * (currNumFiles));
		if (!list)
		{
			DQN_WIN32_ERROR_BOX("calloc() failed.", NULL);
			return NULL;
		}

		for (u32 i = 0; i < currNumFiles; i++)
		{
			list[i] = (char *)calloc(1, sizeof(**list) * MAX_PATH);
			if (!list[i])
			{
				for (u32 j = 0; j < i; j++)
				{
					free(list[j]);
				}

				DQN_WIN32_ERROR_BOX("calloc() failed.", NULL);
				return NULL;
			}
		}

		i32 listIndex = 0;
		WIN32_FIND_DATAW findData = {};
		while (FindNextFileW(findHandle, &findData) != 0)
		{
			dqn_win32_wchar_to_utf8(findData.cFileName, list[listIndex++],
			                        MAX_PATH);
		}

		*numFiles = currNumFiles;
		FindClose(findHandle);

		return list;
	}
#endif
}

DQN_FILE_SCOPE inline void dqn_dir_read_free(char **fileList, u32 numFiles)
{
	if (fileList)
	{
		for (u32 i = 0; i < numFiles; i++)
		{
			if (fileList[i]) free(fileList[i]);
			fileList[i] = NULL;
		}

		free(fileList);
	}
}

////////////////////////////////////////////////////////////////////////////////
// Timer
////////////////////////////////////////////////////////////////////////////////
#ifdef DQN_WIN32
inline FILE_SCOPE f64 dqn_win32_query_perf_counter_time_in_s_internal()
{
	LOCAL_PERSIST LARGE_INTEGER queryPerformanceFrequency = {};
	if (queryPerformanceFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&queryPerformanceFrequency);
		DQN_ASSERT(queryPerformanceFrequency.QuadPart != 0);
	}

	LARGE_INTEGER qpcResult;
	QueryPerformanceCounter(&qpcResult);

	// Convert to ms
	f64 timestamp =
	    qpcResult.QuadPart / (f64)queryPerformanceFrequency.QuadPart;
	return timestamp;
}
#endif

f64 dqn_time_now_in_s()
{
#ifdef _WIN32
	return dqn_win32_query_perf_counter_time_in_s_internal();
#else
	DQN_ASSERT(DQN_INVALID_CODE_PATH);
	return 0;
#endif
};

f64 dqn_time_now_in_ms()
{
	return dqn_time_now_in_s() * 1000.0f;
}

////////////////////////////////////////////////////////////////////////////////
// PCG (Permuted Congruential Generator) Random Number Generator
////////////////////////////////////////////////////////////////////////////////
// Public Domain library with thanks to Mattias Gustavsson
// https://github.com/mattiasgustavsson/libs/blob/master/docs/rnd.md

// Convert a randomized u32 value to a float value x in the range 0.0f <= x
// < 1.0f. Contributed by Jonatan Hedborg
FILE_SCOPE f32 dqn_rnd_f32_normalized_from_u32_internal(u32 value)
{
	u32 exponent = 127;
	u32 mantissa = value >> 9;
	u32 result   = (exponent << 23) | mantissa;
	f32 fresult  = *(f32 *)(&result);
	return fresult - 1.0f;
}

FILE_SCOPE u64 dqn_rnd_murmur3_avalanche64_internal(u64 h)
{
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccd;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53;
	h ^= h >> 33;
	return h;
}

FILE_SCOPE u32 dqn_rnd_make_seed_internal()
{
#ifdef _WIN32
	__int64 numClockCycles = __rdtsc();
	return (u32)numClockCycles;
#else
	unsigned long long numClockCycles = rdtsc();
	return (u32)numClockCycles;
#endif
}

DQN_FILE_SCOPE void dqn_rnd_pcg_init_with_seed(DqnRandPCGState *pcg, u32 seed)
{
	u64 value     = (((u64)seed) << 1ULL) | 1ULL;
	value         = dqn_rnd_murmur3_avalanche64_internal(value);
	pcg->state[0] = 0U;
	pcg->state[1] = (value << 1ULL) | 1ULL;
	dqn_rnd_pcg_next(pcg);
	pcg->state[0] += dqn_rnd_murmur3_avalanche64_internal(value);
	dqn_rnd_pcg_next(pcg);
}

DQN_FILE_SCOPE void dqn_rnd_pcg_init(DqnRandPCGState *pcg)
{
	u32 seed = dqn_rnd_make_seed_internal();
	dqn_rnd_pcg_init_with_seed(pcg, seed);
}

DQN_FILE_SCOPE u32 dqn_rnd_pcg_next(DqnRandPCGState *pcg)
{
	u64 oldstate   = pcg->state[0];
	pcg->state[0]  = oldstate * 0x5851f42d4c957f2dULL + pcg->state[1];
	u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
	u32 rot        = (u32)(oldstate >> 59ULL);
	return (xorshifted >> rot) | (xorshifted << ((-(i32)rot) & 31));
}

DQN_FILE_SCOPE f32 dqn_rnd_pcg_nextf(DqnRandPCGState *pcg)
{
	return dqn_rnd_f32_normalized_from_u32_internal(dqn_rnd_pcg_next(pcg));
}

DQN_FILE_SCOPE i32 dqn_rnd_pcg_range(DqnRandPCGState *pcg, i32 min, i32 max)
{
	i32 const range = (max - min) + 1;
	if (range <= 0) return min;
	i32 const value = (i32)(dqn_rnd_pcg_nextf(pcg) * range);
	return min + value;
}
#endif /* DQN_IMPLEMENTATION */

////////////////////////////////////////////////////////////////////////////////
//
// STB_Sprintf
//
////////////////////////////////////////////////////////////////////////////////
// stb_sprintf - v1.02 - public domain snprintf() implementation
// originally by Jeff Roberts / RAD Game Tools, 2015/10/20
// http://github.com/nothings/stb
//
// allowed types:  sc uidBboXx p AaGgEef n
// lengths      :  h ll j z t I64 I32 I
//
// Contributors (bugfixes):
//    github:d26435
//    github:trex78
//    Jari Komppa (SI suffixes)
//
// LICENSE:
//
//   See end of file for license information.

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE

/*
Single file sprintf replacement.

Originally written by Jeff Roberts at RAD Game Tools - 2015/10/20.  Hereby
placed in public domain.

This is a full sprintf replacement that supports everything that the C runtime
sprintfs support, including float/double, 64-bit integers, hex floats, field
parameters (%*.*d stuff), length reads backs, etc.

Why would you need this if sprintf already exists?  Well, first off, it's *much*
faster (see below). It's also much smaller than the CRT versions
code-space-wise. We've also added some simple improvements that are super handy
(commas in thousands, callbacks at buffer full, for example). Finally, the
format strings for MSVC and GCC differ for 64-bit integers (among other small
things), so this lets you use the same format strings in cross platform code.

It uses the standard single file trick of being both the header file and the
source itself. If you just include it normally, you just get the header file
function definitions. To get the code, you include it from a C or C++ file and
define STB_SPRINTF_IMPLEMENTATION first.

It only uses va_args macros from the C runtime to do it's work. It does cast
doubles to S64s and shifts and divides U64s, which does drag in CRT code on most
platforms.

It compiles to roughly 8K with float support, and 4K without.  As a comparison,
when using MSVC static libs, calling sprintf drags in 16K.

API:
====
int stbsp_sprintf( char * buf, char const * fmt, ... )
int stbsp_snprintf( char * buf, int count, char const * fmt, ... )
  Convert an arg list into a buffer.
  stbsp_snprintf always returns a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintf( char * buf, char const * fmt, va_list va )
int stbsp_vsnprintf( char * buf, int count, char const * fmt, va_list va )
  Convert a va_list arg list into a buffer.  stbsp_vsnprintf always returns
  a zero-terminated string (unlike regular snprintf).

int stbsp_vsprintfcb( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
    typedef char * STBSP_SPRINTFCB( char const * buf, void * user, int len );
  Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
  Your callback can then copy the chars out, print them or whatever.
  This function is actually the workhorse for everything else.
  The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
    // you return the next buffer to use or 0 to stop converting

void stbsp_set_separators( char comma, char period )
  Set the comma and period characters to use.

FLOATS/DOUBLES:
===============
This code uses a internal float->ascii conversion method that uses doubles with
error correction (double-doubles, for ~105 bits of precision).  This conversion
is round-trip perfect - that is, an atof of the values output here will give you
the bit-exact double back.

One difference is that our insignificant digits will be different than with MSVC
or GCC (but they don't match each other either).  We also don't attempt to find
the minimum length matching float (pre-MSVC15 doesn't either).

If you don't need float or doubles at all, define STB_SPRINTF_NOFLOAT and you'll
save 4K of code space.

64-BIT INTS:
============
This library also supports 64-bit integers and you can use MSVC style or GCC
style indicators (%I64d or %lld).  It supports the C99 specifiers for size_t and
ptr_diff_t (%jd %zd) as well.

EXTRAS:
=======
Like some GCCs, for integers and floats, you can use a ' (single quote)
specifier and commas will be inserted on the thousands: "%'d" on 12345 would
print 12,345.

For integers and floats, you can use a "$" specifier and the number will be
converted to float and then divided to get kilo, mega, giga or tera and then
printed, so "%$d" 1000 is "1.0 k", "%$.2d" 2536000 is "2.53 M", etc. For byte
values, use two $:s, like "%$$d" to turn 2536000 to "2.42 Mi". If you prefer
JEDEC suffixes to SI ones, use three $:s: "%$$$d" -> "2.42 M". To remove the
space between the number and the suffix, add "_" specifier: "%_$d" -> "2.53M".

In addition to octal and hexadecimal conversions, you can print integers in
binary: "%b" for 256 would print 100.

PERFORMANCE vs MSVC 2008 32-/64-bit (GCC is even slower than MSVC):
===================================================================
"%d" across all 32-bit ints (4.8x/4.0x faster than 32-/64-bit MSVC)
"%24d" across all 32-bit ints (4.5x/4.2x faster)
"%x" across all 32-bit ints (4.5x/3.8x faster)
"%08x" across all 32-bit ints (4.3x/3.8x faster)
"%f" across e-10 to e+10 floats (7.3x/6.0x faster)
"%e" across e-10 to e+10 floats (8.1x/6.0x faster)
"%g" across e-10 to e+10 floats (10.0x/7.1x faster)
"%f" for values near e-300 (7.9x/6.5x faster)
"%f" for values near e+300 (10.0x/9.1x faster)
"%e" for values near e-300 (10.1x/7.0x faster)
"%e" for values near e+300 (9.2x/6.0x faster)
"%.320f" for values near e-300 (12.6x/11.2x faster)
"%a" for random values (8.6x/4.3x faster)
"%I64d" for 64-bits with 32-bit values (4.8x/3.4x faster)
"%I64d" for 64-bits > 32-bit values (4.9x/5.5x faster)
"%s%s%s" for 64 char strings (7.1x/7.3x faster)
"...512 char string..." ( 35.0x/32.5x faster!)
*/

#if defined(__has_feature)
 #if __has_feature(address_sanitizer)
  #define STBI__ASAN __attribute__((no_sanitize("address")))
 #endif
#endif
#ifndef STBI__ASAN
#define STBI__ASAN
#endif

#ifdef STB_SPRINTF_STATIC
#define STBSP__PUBLICDEC static
#define STBSP__PUBLICDEF static STBI__ASAN
#else
#ifdef __cplusplus
#define STBSP__PUBLICDEC extern "C"
#define STBSP__PUBLICDEF extern "C" STBI__ASAN
#else
#define STBSP__PUBLICDEC extern
#define STBSP__PUBLICDEF STBI__ASAN
#endif
#endif

#include <stdarg.h>  // for va_list()

#ifndef STB_SPRINTF_MIN
#define STB_SPRINTF_MIN 512 // how many characters per callback
#endif
typedef char * STBSP_SPRINTFCB( char * buf, void * user, int len );

#ifndef STB_SPRINTF_DECORATE
#define STB_SPRINTF_DECORATE(name) stbsp_##name  // define this before including if you want to change the names
#endif

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsprintf )( char * buf, char const * fmt, va_list va );
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va );
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( sprintf ) ( char * buf, char const * fmt, ... );
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( snprintf )( char * buf, int count, char const * fmt, ... );

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsprintfcb )( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va );
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE( set_separators )( char comma, char period );

#endif // STB_SPRINTF_H_INCLUDE

#ifdef STB_SPRINTF_IMPLEMENTATION

#include <stdlib.h>  // for va_arg()

#define stbsp__uint32 unsigned int
#define stbsp__int32 signed int

#ifdef _MSC_VER
#define stbsp__uint64 unsigned __int64
#define stbsp__int64 signed __int64
#else
#define stbsp__uint64 unsigned long long
#define stbsp__int64 signed long long
#endif
#define stbsp__uint16 unsigned short

#ifndef stbsp__uintptr 
#if defined(__ppc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE  // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER<1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED  // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str( char const * * start, stbsp__uint32 * len, char *out, stbsp__int32 * decimal_pos, double value, stbsp__uint32 frac_digits );
static stbsp__int32 stbsp__real_to_parts( stbsp__int64 * bits, stbsp__int32 * expo, double value );
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period='.';
static char stbsp__comma=',';
static char stbsp__digitpair[201]="00010203040506070809101112131415161718192021222324252627282930313233343536373839404142434445464748495051525354555657585960616263646566676869707172737475767778798081828384858687888990919293949596979899";

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE( set_separators )( char pcomma, char pperiod )
{
  stbsp__period=pperiod;
  stbsp__comma=pcomma;
}

#define STBSP__LEFTJUST 1
#define STBSP__LEADINGPLUS 2
#define STBSP__LEADINGSPACE 4
#define STBSP__LEADING_0X 8
#define STBSP__LEADINGZERO 16
#define STBSP__INTMAX 32
#define STBSP__TRIPLET_COMMA 64
#define STBSP__NEGATIVE 128
#define STBSP__METRIC_SUFFIX 256
#define STBSP__HALFWIDTH 512
#define STBSP__METRIC_NOSPACE 1024
#define STBSP__METRIC_1024 2048
#define STBSP__METRIC_JEDEC 4096

static void stbsp__lead_sign(stbsp__uint32 fl, char *sign)
{
  sign[0] = 0;
  if (fl&STBSP__NEGATIVE) {
    sign[0]=1;
    sign[1]='-';
  } else if (fl&STBSP__LEADINGSPACE) {
    sign[0]=1;
    sign[1]=' ';
  } else if (fl&STBSP__LEADINGPLUS) {
    sign[0]=1;
    sign[1]='+';
  }
}
  
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsprintfcb )( STBSP_SPRINTFCB * callback, void * user, char * buf, char const * fmt, va_list va )
{
  static char hex[]="0123456789abcdefxp";
  static char hexu[]="0123456789ABCDEFXP";
  char * bf;
  char const * f;
  int tlen = 0;

  bf = buf;
  f = fmt;
  for(;;)
  {
    stbsp__int32 fw,pr,tz; stbsp__uint32 fl;

    // macros for the callback buffer stuff
    #define stbsp__chk_cb_bufL(bytes) { int len = (int)(bf-buf); if ((len+(bytes))>=STB_SPRINTF_MIN) { tlen+=len; if (0==(bf=buf=callback(buf,user,len))) goto done; } }
    #define stbsp__chk_cb_buf(bytes) { if ( callback ) { stbsp__chk_cb_bufL(bytes); } }
    #define stbsp__flush_cb() { stbsp__chk_cb_bufL(STB_SPRINTF_MIN-1); } //flush if there is even one byte in the buffer
    #define stbsp__cb_buf_clamp(cl,v) cl = v; if ( callback ) { int lg = STB_SPRINTF_MIN-(int)(bf-buf); if (cl>lg) cl=lg; }

    // fast copy everything up to the next % (or end of string)
    for(;;)
    { 
      while (((stbsp__uintptr)f)&3)
      {
       schk1: if (f[0]=='%') goto scandd;
       schk2: if (f[0]==0) goto endfmt;
        stbsp__chk_cb_buf(1); *bf++=f[0]; ++f;
      } 
      for(;;)
      { 
        // Check if the next 4 bytes contain %(0x25) or end of string.
        // Using the 'hasless' trick:
        // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
        stbsp__uint32 v,c;
        v=*(stbsp__uint32*)f; c=(~v)&0x80808080;
        if (((v^0x25252525)-0x01010101)&c) goto schk1;
        if ((v-0x01010101)&c) goto schk2; 
        if (callback) if ((STB_SPRINTF_MIN-(int)(bf-buf))<4) goto schk1;
        *(stbsp__uint32*)bf=v; bf+=4; f+=4;
      }
    } scandd:

    ++f;

    // ok, we have a percent, read the modifiers first
    fw = 0; pr = -1; fl = 0; tz = 0;
    
    // flags
    for(;;)
    {
      switch(f[0])
      {
        // if we have left justify
        case '-': fl|=STBSP__LEFTJUST; ++f; continue;
        // if we have leading plus
        case '+': fl|=STBSP__LEADINGPLUS; ++f; continue; 
        // if we have leading space
        case ' ': fl|=STBSP__LEADINGSPACE; ++f; continue; 
        // if we have leading 0x
        case '#': fl|=STBSP__LEADING_0X; ++f; continue; 
        // if we have thousand commas
        case '\'': fl|=STBSP__TRIPLET_COMMA; ++f; continue; 
        // if we have kilo marker (none->kilo->kibi->jedec)
        case '$': 
            if (fl&STBSP__METRIC_SUFFIX)
            {
                if (fl&STBSP__METRIC_1024)
                {
                    fl|=STBSP__METRIC_JEDEC;
                }
                else
                {
                    fl|=STBSP__METRIC_1024;
                }
            }
            else
            {
                fl|=STBSP__METRIC_SUFFIX; 
            }
            ++f; continue; 
        // if we don't want space between metric suffix and number
        case '_': fl|=STBSP__METRIC_NOSPACE; ++f; continue;
        // if we have leading zero
        case '0': fl|=STBSP__LEADINGZERO; ++f; goto flags_done; 
        default: goto flags_done;
      }
    }
    flags_done:
   
    // get the field width
    if ( f[0] == '*' ) {fw = va_arg(va,stbsp__uint32); ++f;} else { while (( f[0] >= '0' ) && ( f[0] <= '9' )) { fw = fw * 10 + f[0] - '0'; f++; } }
    // get the precision
    if ( f[0]=='.' ) { ++f; if ( f[0] == '*' ) {pr = va_arg(va,stbsp__uint32); ++f;} else { pr = 0; while (( f[0] >= '0' ) && ( f[0] <= '9' )) { pr = pr * 10 + f[0] - '0'; f++; } } } 
    
    // handle integer size overrides
    switch(f[0])
    {
      // are we halfwidth?
      case 'h': fl|=STBSP__HALFWIDTH; ++f; break;
      // are we 64-bit (unix style)
      case 'l': ++f; if ( f[0]=='l') { fl|=STBSP__INTMAX; ++f; } break;
      // are we 64-bit on intmax? (c99)
      case 'j': fl|=STBSP__INTMAX; ++f; break; 
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z': case 't': fl|=((sizeof(char*)==8)?STBSP__INTMAX:0); ++f; break; 
      // are we 64-bit (msft style)
      case 'I': if ( ( f[1]=='6') && ( f[2]=='4') ) { fl|=STBSP__INTMAX; f+=3; }
                else if ( ( f[1]=='3') && ( f[2]=='2') ) { f+=3; }
                else { fl|=((sizeof(void*)==8)?STBSP__INTMAX:0); ++f; } break;
      default: break;
    }

    // handle each replacement
    switch( f[0] )
    {
      #define STBSP__NUMSZ   512 // big enough for e308 (with commas) or e-307 
      char num[STBSP__NUMSZ]; 
      char lead[8]; 
      char tail[8]; 
      char *s;
      char const *h;
      stbsp__uint32 l,n,cs;
      stbsp__uint64 n64;
      #ifndef STB_SPRINTF_NOFLOAT      
      double fv; 
      #endif
      stbsp__int32 dp; char const * sn;

      case 's':
        // get the string
        s = va_arg(va,char*); if (s==0) s = (char*)"null";
        // get the length
        sn = s;
        for(;;)
        { 
          if ((((stbsp__uintptr)sn)&3)==0) break;
         lchk:
          if (sn[0]==0) goto ld;
          ++sn;
        }
        n = 0xffffffff;
        if (pr>=0) { n=(stbsp__uint32)(sn-s); if (n>=(stbsp__uint32)pr) goto ld; n=((stbsp__uint32)(pr-n))>>2; }
        while(n) 
        { 
          stbsp__uint32 v=*(stbsp__uint32*)sn;
          if ((v-0x01010101)&(~v)&0x80808080UL) goto lchk; 
          sn+=4; 
          --n;
        }
        goto lchk;
       ld:

        l = (stbsp__uint32) ( sn - s );
        // clamp to precision
        if ( l > (stbsp__uint32)pr ) l = pr;
        lead[0]=0; tail[0]=0; pr = 0; dp = 0; cs = 0;
        // copy the string in
        goto scopy;

      case 'c': // char
        // get the character
        s = num + STBSP__NUMSZ -1; *s = (char)va_arg(va,int);
        l = 1;
        lead[0]=0; tail[0]=0; pr = 0; dp = 0; cs = 0;
        goto scopy;

      case 'n': // weird write-bytes specifier
        { int * d = va_arg(va,int*);
        *d = tlen + (int)( bf - buf ); }
        break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A': // float
      case 'a': // hex float
      case 'G': // float
      case 'g': // float
      case 'E': // float
      case 'e': // float
      case 'f': // float
        va_arg(va,double); // eat it
        s = (char*)"No float";
        l = 8;
        lead[0]=0; tail[0]=0; pr = 0; dp = 0; cs = 0;
        goto scopy;
#else
      case 'A': // float
        h=hexu;  
        goto hexfloat;

      case 'a': // hex float
        h=hex;
       hexfloat: 
        fv = va_arg(va,double);
        if (pr==-1) pr=6; // default is 6
        // read the double into a string
        if ( stbsp__real_to_parts( (stbsp__int64*)&n64, &dp, fv ) )
          fl |= STBSP__NEGATIVE;
  
        s = num+64;

        stbsp__lead_sign(fl, lead);

        if (dp==-1023) dp=(n64)?-1022:0; else n64|=(((stbsp__uint64)1)<<52);
        n64<<=(64-56);
        if (pr<15) n64+=((((stbsp__uint64)8)<<56)>>(pr*4));
        // add leading chars
        
        #ifdef STB_SPRINTF_MSVC_MODE
        *s++='0';*s++='x';
        #else
        lead[1+lead[0]]='0'; lead[2+lead[0]]='x'; lead[0]+=2;
        #endif
        *s++=h[(n64>>60)&15]; n64<<=4;
        if ( pr ) *s++=stbsp__period;
        sn = s;

        // print the bits
        n = pr; if (n>13) n = 13; if (pr>(stbsp__int32)n) tz=pr-n; pr = 0;
        while(n--) { *s++=h[(n64>>60)&15]; n64<<=4; }

        // print the expo
        tail[1]=h[17];
        if (dp<0) { tail[2]='-'; dp=-dp;} else tail[2]='+';
        n = (dp>=1000)?6:((dp>=100)?5:((dp>=10)?4:3));
        tail[0]=(char)n;
        for(;;) { tail[n]='0'+dp%10; if (n<=3) break; --n; dp/=10; }

        dp = (int)(s-sn);
        l = (int)(s-(num+64));
        s = num+64;
        cs = 1 + (3<<24);
        goto scopy;

      case 'G': // float
        h=hexu;
        goto dosmallfloat;

      case 'g': // float
        h=hex;
       dosmallfloat:   
        fv = va_arg(va,double);
        if (pr==-1) pr=6; else if (pr==0) pr = 1; // default is 6
        // read the double into a string
        if ( stbsp__real_to_str( &sn, &l, num, &dp, fv, (pr-1)|0x80000000 ) )
          fl |= STBSP__NEGATIVE;

        // clamp the precision and delete extra zeros after clamp
        n = pr;
        if ( l > (stbsp__uint32)pr ) l = pr; while ((l>1)&&(pr)&&(sn[l-1]=='0')) { --pr; --l; }

        // should we use %e
        if ((dp<=-4)||(dp>(stbsp__int32)n))
        {
          if ( pr > (stbsp__int32)l ) pr = l-1; else if ( pr ) --pr; // when using %e, there is one digit before the decimal
          goto doexpfromg;
        }
        // this is the insane action to get the pr to match %g sematics for %f
        if(dp>0) { pr=(dp<(stbsp__int32)l)?l-dp:0; } else { pr = -dp+((pr>(stbsp__int32)l)?l:pr); }
        goto dofloatfromg;

      case 'E': // float
        h=hexu;  
        goto doexp;

      case 'e': // float
        h=hex;
       doexp:   
        fv = va_arg(va,double);
        if (pr==-1) pr=6; // default is 6
        // read the double into a string
        if ( stbsp__real_to_str( &sn, &l, num, &dp, fv, pr|0x80000000 ) )
          fl |= STBSP__NEGATIVE;
       doexpfromg: 
        tail[0]=0; 
        stbsp__lead_sign(fl, lead);
        if ( dp == STBSP__SPECIAL ) { s=(char*)sn; cs=0; pr=0; goto scopy; }
        s=num+64; 
        // handle leading chars
        *s++=sn[0];

        if (pr) *s++=stbsp__period;

        // handle after decimal
        if ((l-1)>(stbsp__uint32)pr) l=pr+1;
        for(n=1;n<l;n++) *s++=sn[n];
        // trailing zeros
        tz = pr-(l-1); pr=0;
        // dump expo
        tail[1]=h[0xe];
        dp -= 1;
        if (dp<0) { tail[2]='-'; dp=-dp;} else tail[2]='+';
        #ifdef STB_SPRINTF_MSVC_MODE
        n = 5;
        #else
        n = (dp>=100)?5:4;
        #endif
        tail[0]=(char)n;
        for(;;) { tail[n]='0'+dp%10; if (n<=3) break; --n; dp/=10; }
        cs = 1 + (3<<24); // how many tens
        goto flt_lead;   

      case 'f': // float
        fv = va_arg(va,double);
       doafloat: 
        // do kilos
        if (fl&STBSP__METRIC_SUFFIX) 
        {
            double divisor;
            divisor=1000.0f;
            if (fl&STBSP__METRIC_1024) divisor = 1024.0;
            while(fl<0x4000000) { if ((fv<divisor) && (fv>-divisor)) break; fv/=divisor; fl+=0x1000000; }
        }
        if (pr==-1) pr=6; // default is 6
        // read the double into a string
        if ( stbsp__real_to_str( &sn, &l, num, &dp, fv, pr ) )
          fl |= STBSP__NEGATIVE;
        dofloatfromg:
        tail[0]=0;
        stbsp__lead_sign(fl, lead);
        if ( dp == STBSP__SPECIAL ) { s=(char*)sn; cs=0; pr=0; goto scopy; }
        s=num+64; 

        // handle the three decimal varieties
        if (dp<=0) 
        { 
          stbsp__int32 i;
          // handle 0.000*000xxxx
          *s++='0'; if (pr) *s++=stbsp__period; 
          n=-dp; if((stbsp__int32)n>pr) n=pr; i=n; while(i) { if ((((stbsp__uintptr)s)&3)==0) break; *s++='0'; --i; } while(i>=4) { *(stbsp__uint32*)s=0x30303030; s+=4; i-=4; } while(i) { *s++='0'; --i; }
          if ((stbsp__int32)(l+n)>pr) l=pr-n; i=l; while(i) { *s++=*sn++; --i; }
          tz = pr-(n+l);
          cs = 1 + (3<<24); // how many tens did we write (for commas below)
        }
        else
        {
          cs = (fl&STBSP__TRIPLET_COMMA)?((600-(stbsp__uint32)dp)%3):0;
          if ((stbsp__uint32)dp>=l)
          {
            // handle xxxx000*000.0
            n=0; for(;;) { if ((fl&STBSP__TRIPLET_COMMA) && (++cs==4)) { cs = 0; *s++=stbsp__comma; } else { *s++=sn[n]; ++n; if (n>=l) break; } }
            if (n<(stbsp__uint32)dp)
            {
              n = dp - n;
              if ((fl&STBSP__TRIPLET_COMMA)==0) { while(n) { if ((((stbsp__uintptr)s)&3)==0) break; *s++='0'; --n; }  while(n>=4) { *(stbsp__uint32*)s=0x30303030; s+=4; n-=4; } }
              while(n) { if ((fl&STBSP__TRIPLET_COMMA) && (++cs==4)) { cs = 0; *s++=stbsp__comma; } else { *s++='0'; --n; } }
            }
            cs = (int)(s-(num+64)) + (3<<24); // cs is how many tens
            if (pr) { *s++=stbsp__period; tz=pr;}
          }
          else
          {
            // handle xxxxx.xxxx000*000
            n=0; for(;;) { if ((fl&STBSP__TRIPLET_COMMA) && (++cs==4)) { cs = 0; *s++=stbsp__comma; } else { *s++=sn[n]; ++n; if (n>=(stbsp__uint32)dp) break; } }
            cs = (int)(s-(num+64)) + (3<<24); // cs is how many tens
            if (pr) *s++=stbsp__period;
            if ((l-dp)>(stbsp__uint32)pr) l=pr+dp;
            while(n<l) { *s++=sn[n]; ++n; }
            tz = pr-(l-dp);
          }
        }
        pr = 0;
        
        // handle k,m,g,t
        if (fl&STBSP__METRIC_SUFFIX) 
        { 
            char idx;
            idx=1;
            if (fl&STBSP__METRIC_NOSPACE)
                idx=0;
            tail[0]=idx; 
            tail[1]=' '; 
            { 
                if (fl>>24) 
                {   // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                    if (fl&STBSP__METRIC_1024)
                        tail[idx+1]="_KMGT"[fl>>24]; 
                    else
                        tail[idx+1]="_kMGT"[fl>>24]; 
                    idx++;
                    // If printing kibits and not in jedec, add the 'i'.
                    if (fl&STBSP__METRIC_1024&&!(fl&STBSP__METRIC_JEDEC))
                    {
                        tail[idx+1]='i';
                        idx++;
                    }
                    tail[0]=idx;
                } 
            } 
        };

        flt_lead:
        // get the length that we copied
        l = (stbsp__uint32) ( s-(num+64) );
        s=num+64; 
        goto scopy;
#endif

      case 'B': // upper binary
        h = hexu;
        goto binary;

      case 'b': // lower binary
        h = hex;
        binary:
        lead[0]=0;
        if (fl&STBSP__LEADING_0X) { lead[0]=2;lead[1]='0';lead[2]=h[0xb]; }
        l=(8<<4)|(1<<8);
        goto radixnum;

      case 'o': // octal
        h = hexu;
        lead[0]=0;
        if (fl&STBSP__LEADING_0X) { lead[0]=1;lead[1]='0'; }
        l=(3<<4)|(3<<8);
        goto radixnum;

      case 'p': // pointer
        fl |= (sizeof(void*)==8)?STBSP__INTMAX:0;
        pr = sizeof(void*)*2;
        fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
        // drop through to X
      
      case 'X': // upper binary
        h = hexu;
        goto dohexb;

      case 'x': // lower binary
        h = hex; dohexb:
        l=(4<<4)|(4<<8);
        lead[0]=0;
        if (fl&STBSP__LEADING_0X) { lead[0]=2;lead[1]='0';lead[2]=h[16]; }
       radixnum: 
        // get the number
        if ( fl&STBSP__INTMAX )
          n64 = va_arg(va,stbsp__uint64);
        else
          n64 = va_arg(va,stbsp__uint32);

        s = num + STBSP__NUMSZ; dp = 0;
        // clear tail, and clear leading if value is zero
        tail[0]=0; if (n64==0) { lead[0]=0; if (pr==0) { l=0; cs = ( ((l>>4)&15)) << 24; goto scopy; } }
        // convert to string
        for(;;) { *--s = h[n64&((1<<(l>>8))-1)]; n64>>=(l>>8); if ( ! ( (n64) || ((stbsp__int32) ( (num+STBSP__NUMSZ) - s ) < pr ) ) ) break; if ( fl&STBSP__TRIPLET_COMMA) { ++l; if ((l&15)==((l>>4)&15)) { l&=~15; *--s=stbsp__comma; } } };
        // get the tens and the comma pos
        cs = (stbsp__uint32) ( (num+STBSP__NUMSZ) - s ) + ( ( ((l>>4)&15)) << 24 );
        // get the length that we copied
        l = (stbsp__uint32) ( (num+STBSP__NUMSZ) - s );
        // copy it
        goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
        // get the integer and abs it
        if ( fl&STBSP__INTMAX )
        {
          stbsp__int64 i64 = va_arg(va,stbsp__int64); n64 = (stbsp__uint64)i64; if ((f[0]!='u') && (i64<0)) { n64=(stbsp__uint64)-i64; fl|=STBSP__NEGATIVE; }
        }
        else
        {
          stbsp__int32 i = va_arg(va,stbsp__int32); n64 = (stbsp__uint32)i; if ((f[0]!='u') && (i<0)) { n64=(stbsp__uint32)-i; fl|=STBSP__NEGATIVE; }
        }

        #ifndef STB_SPRINTF_NOFLOAT
        if (fl&STBSP__METRIC_SUFFIX) { if (n64<1024) pr=0; else if (pr==-1) pr=1; fv=(double)(stbsp__int64)n64; goto doafloat; } 
        #endif

        // convert to string
        s = num+STBSP__NUMSZ; l=0; 
        
        for(;;)
        {
          // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
          char * o=s-8;
          if (n64>=100000000) { n = (stbsp__uint32)( n64 % 100000000);  n64 /= 100000000; } else {n = (stbsp__uint32)n64; n64 = 0; }
          if((fl&STBSP__TRIPLET_COMMA)==0) { while(n) { s-=2; *(stbsp__uint16*)s=*(stbsp__uint16*)&stbsp__digitpair[(n%100)*2]; n/=100; } }
          while (n) { if ( ( fl&STBSP__TRIPLET_COMMA) && (l++==3) ) { l=0; *--s=stbsp__comma; --o; } else { *--s=(char)(n%10)+'0'; n/=10; } }
          if (n64==0) { if ((s[0]=='0') && (s!=(num+STBSP__NUMSZ))) ++s; break; }
          while (s!=o) if ( ( fl&STBSP__TRIPLET_COMMA) && (l++==3) ) { l=0; *--s=stbsp__comma; --o; } else { *--s='0'; }
        }

        tail[0]=0;
        stbsp__lead_sign(fl, lead);

        // get the length that we copied
        l = (stbsp__uint32) ( (num+STBSP__NUMSZ) - s ); if ( l == 0 ) { *--s='0'; l = 1; }
        cs = l + (3<<24);
        if (pr<0) pr = 0;

       scopy: 
        // get fw=leading/trailing space, pr=leading zeros
        if (pr<(stbsp__int32)l) pr = l;
        n = pr + lead[0] + tail[0] + tz;
        if (fw<(stbsp__int32)n) fw = n;
        fw -= n;
        pr -= l;

        // handle right justify and leading zeros
        if ( (fl&STBSP__LEFTJUST)==0 )
        {
          if (fl&STBSP__LEADINGZERO) // if leading zeros, everything is in pr
          { 
            pr = (fw>pr)?fw:pr;
            fw = 0;
          }
          else
          {
            fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
          }
        }

        // copy the spaces and/or zeros
        if (fw+pr)
        {
          stbsp__int32 i; stbsp__uint32 c; 

          // copy leading spaces (or when doing %8.4d stuff)
          if ( (fl&STBSP__LEFTJUST)==0 ) while(fw>0) { stbsp__cb_buf_clamp(i,fw); fw -= i; while(i) { if ((((stbsp__uintptr)bf)&3)==0) break; *bf++=' '; --i; } while(i>=4) { *(stbsp__uint32*)bf=0x20202020; bf+=4; i-=4; } while (i) {*bf++=' '; --i;} stbsp__chk_cb_buf(1); }
        
          // copy leader
          sn=lead+1; while(lead[0]) { stbsp__cb_buf_clamp(i,lead[0]); lead[0] -= (char)i; while (i) {*bf++=*sn++; --i;} stbsp__chk_cb_buf(1); }
          
          // copy leading zeros
          c = cs >> 24; cs &= 0xffffff;
          cs = (fl&STBSP__TRIPLET_COMMA)?((stbsp__uint32)(c-((pr+cs)%(c+1)))):0;
          while(pr>0) { stbsp__cb_buf_clamp(i,pr); pr -= i; if((fl&STBSP__TRIPLET_COMMA)==0) { while(i) { if ((((stbsp__uintptr)bf)&3)==0) break; *bf++='0'; --i; } while(i>=4) { *(stbsp__uint32*)bf=0x30303030; bf+=4; i-=4; } } while (i) { if((fl&STBSP__TRIPLET_COMMA) && (cs++==c)) { cs = 0; *bf++=stbsp__comma; } else *bf++='0'; --i; } stbsp__chk_cb_buf(1); }
        }

        // copy leader if there is still one
        sn=lead+1; while(lead[0]) { stbsp__int32 i; stbsp__cb_buf_clamp(i,lead[0]); lead[0] -= (char)i; while (i) {*bf++=*sn++; --i;} stbsp__chk_cb_buf(1); }

        // copy the string
        n = l; while (n) { stbsp__int32 i; stbsp__cb_buf_clamp(i,n); n-=i; STBSP__UNALIGNED( while(i>=4) { *(stbsp__uint32*)bf=*(stbsp__uint32*)s; bf+=4; s+=4; i-=4; } ) while (i) {*bf++=*s++; --i;} stbsp__chk_cb_buf(1); }

        // copy trailing zeros
        while(tz) { stbsp__int32 i; stbsp__cb_buf_clamp(i,tz); tz -= i; while(i) { if ((((stbsp__uintptr)bf)&3)==0) break; *bf++='0'; --i; } while(i>=4) { *(stbsp__uint32*)bf=0x30303030; bf+=4; i-=4; } while (i) {*bf++='0'; --i;} stbsp__chk_cb_buf(1); }

        // copy tail if there is one
        sn=tail+1; while(tail[0]) { stbsp__int32 i; stbsp__cb_buf_clamp(i,tail[0]); tail[0] -= (char)i; while (i) {*bf++=*sn++; --i;} stbsp__chk_cb_buf(1); }

        // handle the left justify
        if (fl&STBSP__LEFTJUST) if (fw>0) { while (fw) { stbsp__int32 i; stbsp__cb_buf_clamp(i,fw); fw-=i; while(i) { if ((((stbsp__uintptr)bf)&3)==0) break; *bf++=' '; --i; } while(i>=4) { *(stbsp__uint32*)bf=0x20202020; bf+=4; i-=4; } while (i--) *bf++=' '; stbsp__chk_cb_buf(1); } }
        break;

      default: // unknown, just copy code
        s = num + STBSP__NUMSZ -1; *s = f[0];
        l = 1;
        fw=pr=fl=0;
        lead[0]=0; tail[0]=0; pr = 0; dp = 0; cs = 0;
        goto scopy;
    }
    ++f;
  }
 endfmt:

  if (!callback) 
    *bf = 0;
  else
    stbsp__flush_cb();
 
 done:
  return tlen + (int)(bf-buf);
}

// cleanup
#undef STBSP__LEFTJUST
#undef STBSP__LEADINGPLUS
#undef STBSP__LEADINGSPACE
#undef STBSP__LEADING_0X
#undef STBSP__LEADINGZERO
#undef STBSP__INTMAX
#undef STBSP__TRIPLET_COMMA
#undef STBSP__NEGATIVE
#undef STBSP__METRIC_SUFFIX
#undef STBSP__NUMSZ
#undef stbsp__chk_cb_bufL
#undef stbsp__chk_cb_buf
#undef stbsp__flush_cb
#undef stbsp__cb_buf_clamp

// ============================================================================
//   wrapper functions

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( sprintf )( char * buf, char const * fmt, ... )
{
  int result;
  va_list va;
  va_start( va, fmt );
  result = STB_SPRINTF_DECORATE( vsprintfcb )( 0, 0, buf, fmt, va );
  va_end(va);
  return result;
}

typedef struct stbsp__context
{
  char * buf;
  int count;
  char tmp[ STB_SPRINTF_MIN ];
} stbsp__context;

static char * stbsp__clamp_callback( char * buf, void * user, int len )
{
  stbsp__context * c = (stbsp__context*)user;

  if ( len > c->count ) len = c->count;

  if (len)
  {
    if ( buf != c->buf )
    {
      char * s, * d, * se;
      d = c->buf; s = buf; se = buf+len;
      do{ *d++ = *s++; } while (s<se);
    }
    c->buf += len;
    c->count -= len;
  }
  
  if ( c->count <= 0 ) return 0;
  return ( c->count >= STB_SPRINTF_MIN ) ? c->buf : c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
  stbsp__context c;
  int l;

  if ( count == 0 )
    return 0;

  c.buf = buf;
  c.count = count;

  STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );
  
  // zero-terminate
  l = (int)( c.buf - buf );
  if ( l >= count ) // should never be greater, only equal (or less) than count
    l = count - 1;
  buf[l] = 0;

  return l;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( snprintf )( char * buf, int count, char const * fmt, ... )
{
  int result;
  va_list va;
  va_start( va, fmt );

  result = STB_SPRINTF_DECORATE( vsnprintf )( buf, count, fmt, va );
  va_end(va);

  return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsprintf )( char * buf, char const * fmt, va_list va )
{
  return STB_SPRINTF_DECORATE( vsprintfcb )( 0, 0, buf, fmt, va );
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest,src) { int cn; for(cn=0;cn<8;cn++) ((char*)&dest)[cn]=((char*)&src)[cn]; }
 
// get float info
static stbsp__int32 stbsp__real_to_parts( stbsp__int64 * bits, stbsp__int32 * expo, double value )
{
  double d;
  stbsp__int64 b = 0;

  // load value and round at the frac_digits
  d = value;

  STBSP__COPYFP( b, d );

  *bits = b & ((((stbsp__uint64)1)<<52)-1);
  *expo = (stbsp__int32) (((b >> 52) & 2047)-1023);
    
  return (stbsp__int32)(b >> 63);
}

static double const stbsp__bot[23]={1e+000,1e+001,1e+002,1e+003,1e+004,1e+005,1e+006,1e+007,1e+008,1e+009,1e+010,1e+011,1e+012,1e+013,1e+014,1e+015,1e+016,1e+017,1e+018,1e+019,1e+020,1e+021,1e+022};
static double const stbsp__negbot[22]={1e-001,1e-002,1e-003,1e-004,1e-005,1e-006,1e-007,1e-008,1e-009,1e-010,1e-011,1e-012,1e-013,1e-014,1e-015,1e-016,1e-017,1e-018,1e-019,1e-020,1e-021,1e-022};
static double const stbsp__negboterr[22]={-5.551115123125783e-018,-2.0816681711721684e-019,-2.0816681711721686e-020,-4.7921736023859299e-021,-8.1803053914031305e-022,4.5251888174113741e-023,4.5251888174113739e-024,-2.0922560830128471e-025,-6.2281591457779853e-026,-3.6432197315497743e-027,6.0503030718060191e-028,2.0113352370744385e-029,-3.0373745563400371e-030,1.1806906454401013e-032,-7.7705399876661076e-032,2.0902213275965398e-033,-7.1542424054621921e-034,-7.1542424054621926e-035,2.4754073164739869e-036,5.4846728545790429e-037,9.2462547772103625e-038,-4.8596774326570872e-039};
static double const stbsp__top[13]={1e+023,1e+046,1e+069,1e+092,1e+115,1e+138,1e+161,1e+184,1e+207,1e+230,1e+253,1e+276,1e+299};
static double const stbsp__negtop[13]={1e-023,1e-046,1e-069,1e-092,1e-115,1e-138,1e-161,1e-184,1e-207,1e-230,1e-253,1e-276,1e-299};
static double const stbsp__toperr[13]={8388608,6.8601809640529717e+028,-7.253143638152921e+052,-4.3377296974619174e+075,-1.5559416129466825e+098,-3.2841562489204913e+121,-3.7745893248228135e+144,-1.7356668416969134e+167,-3.8893577551088374e+190,-9.9566444326005119e+213,6.3641293062232429e+236,-5.2069140800249813e+259,-5.2504760255204387e+282};
static double const stbsp__negtoperr[13]={3.9565301985100693e-040,-2.299904345391321e-063,3.6506201437945798e-086,1.1875228833981544e-109,-5.0644902316928607e-132,-6.7156837247865426e-155,-2.812077463003139e-178,-5.7778912386589953e-201,7.4997100559334532e-224,-4.6439668915134491e-247,-6.3691100762962136e-270,-9.436808465446358e-293,8.0970921678014997e-317};

#if defined(_MSC_VER) && (_MSC_VER<=1200)                                                                                                                                                                                       
static stbsp__uint64 const stbsp__powten[20]={1,10,100,1000, 10000,100000,1000000,10000000, 100000000,1000000000,10000000000,100000000000,  1000000000000,10000000000000,100000000000000,1000000000000000,  10000000000000000,100000000000000000,1000000000000000000,10000000000000000000U };
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20]={1,10,100,1000, 10000,100000,1000000,10000000, 100000000,1000000000,10000000000ULL,100000000000ULL,  1000000000000ULL,10000000000000ULL,100000000000000ULL,1000000000000000ULL,  10000000000000000ULL,100000000000000000ULL,1000000000000000000ULL,10000000000000000000ULL };
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh,ol,xh,yh) \
{ \
  double ahi=0,alo,bhi=0,blo; \
  stbsp__int64 bt; \
  oh = xh * yh; \
  STBSP__COPYFP(bt,xh); bt&=((~(stbsp__uint64)0)<<27); STBSP__COPYFP(ahi,bt); alo = xh-ahi; \
  STBSP__COPYFP(bt,yh); bt&=((~(stbsp__uint64)0)<<27); STBSP__COPYFP(bhi,bt); blo = yh-bhi; \
  ol = ((ahi*bhi-oh)+ahi*blo+alo*bhi)+alo*blo; \
}

#define stbsp__ddtoS64(ob,xh,xl) \
{ \
  double ahi=0,alo,vh,t;\
  ob = (stbsp__int64)ph;\
  vh=(double)ob;\
  ahi = ( xh - vh );\
  t = ( ahi - xh );\
  alo = (xh-(ahi-t))-(vh+t);\
  ob += (stbsp__int64)(ahi+alo+xl);\
}


#define stbsp__ddrenorm(oh,ol) { double s; s=oh+ol; ol=ol-(s-oh); oh=s; }

#define stbsp__ddmultlo(oh,ol,xh,xl,yh,yl) \
  ol = ol + ( xh*yl + xl*yh ); \

#define stbsp__ddmultlos(oh,ol,xh,yl) \
  ol = ol + ( xh*yl ); \

static void stbsp__raise_to_power10( double *ohi, double *olo, double d, stbsp__int32 power )  // power can be -323 to +350
{
  double ph, pl;
  if ((power>=0) && (power<=22))
  {
    stbsp__ddmulthi(ph,pl,d,stbsp__bot[power]);
  }
  else
  {
    stbsp__int32 e,et,eb;
    double p2h,p2l;

    e=power; if (power<0) e=-e; 
    et = (e*0x2c9)>>14;/* %23 */ if (et>13) et=13; eb = e-(et*23);

    ph = d; pl = 0.0;
    if (power<0)
    {
      if (eb) { --eb; stbsp__ddmulthi(ph,pl,d,stbsp__negbot[eb]); stbsp__ddmultlos(ph,pl,d,stbsp__negboterr[eb]); }
      if (et)
      { 
        stbsp__ddrenorm(ph,pl);
        --et; stbsp__ddmulthi(p2h,p2l,ph,stbsp__negtop[et]); stbsp__ddmultlo(p2h,p2l,ph,pl,stbsp__negtop[et],stbsp__negtoperr[et]); ph=p2h;pl=p2l;
      }
    }
    else
    {
      if (eb) 
      { 
        e = eb; if (eb>22) eb=22; e -= eb;
        stbsp__ddmulthi(ph,pl,d,stbsp__bot[eb]); 
        if ( e ) { stbsp__ddrenorm(ph,pl); stbsp__ddmulthi(p2h,p2l,ph,stbsp__bot[e]); stbsp__ddmultlos(p2h,p2l,stbsp__bot[e],pl); ph=p2h;pl=p2l; }
      }
      if (et)
      {
        stbsp__ddrenorm(ph,pl);
        --et; stbsp__ddmulthi(p2h,p2l,ph,stbsp__top[et]); stbsp__ddmultlo(p2h,p2l,ph,pl,stbsp__top[et],stbsp__toperr[et]); ph=p2h;pl=p2l;
      }
    }
  }
  stbsp__ddrenorm(ph,pl);
  *ohi = ph; *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str( char const * * start, stbsp__uint32 * len, char *out, stbsp__int32 * decimal_pos, double value, stbsp__uint32 frac_digits )
{
  double d;
  stbsp__int64 bits = 0;
  stbsp__int32 expo, e, ng, tens;

  d = value;
  STBSP__COPYFP(bits,d);
  expo = (stbsp__int32) ((bits >> 52) & 2047);
  ng = (stbsp__int32)(bits >> 63);
  if (ng) d=-d;

  if ( expo == 2047 ) // is nan or inf?
  {
    *start = (bits&((((stbsp__uint64)1)<<52)-1)) ? "NaN" : "Inf";
    *decimal_pos =  STBSP__SPECIAL;
    *len = 3;
    return ng;
  } 

  if ( expo == 0 ) // is zero or denormal
  {
    if ((bits<<1)==0) // do zero
    {
      *decimal_pos = 1; 
      *start = out;
      out[0] = '0'; *len = 1;
      return ng;
    }
    // find the right expo for denormals
    {
      stbsp__int64 v = ((stbsp__uint64)1)<<51;
      while ((bits&v)==0) { --expo; v >>= 1; }
    }
  }

  // find the decimal exponent as well as the decimal bits of the value
  {
    double ph,pl;

    // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
    tens=expo-1023; tens = (tens<0)?((tens*617)/2048):(((tens*1233)/4096)+1);

    // move the significant bits into position and stick them into an int 
    stbsp__raise_to_power10( &ph, &pl, d, 18-tens );

    // get full as much precision from double-double as possible
    stbsp__ddtoS64( bits, ph,pl );

    // check if we undershot
    if ( ((stbsp__uint64)bits) >= stbsp__tento19th ) ++tens; 
  }

  // now do the rounding in integer land
  frac_digits = ( frac_digits & 0x80000000 ) ? ( (frac_digits&0x7ffffff) + 1 ) : ( tens + frac_digits );
  if ( ( frac_digits < 24 ) )
  {
    stbsp__uint32 dg = 1; if ((stbsp__uint64)bits >= stbsp__powten[9] ) dg=10; while( (stbsp__uint64)bits >= stbsp__powten[dg] ) { ++dg; if (dg==20) goto noround; }
    if ( frac_digits < dg )
    {
      stbsp__uint64 r;
      // add 0.5 at the right position and round
      e = dg - frac_digits;
      if ( (stbsp__uint32)e >= 24 ) goto noround;
      r = stbsp__powten[e];
      bits = bits + (r/2);
      if ( (stbsp__uint64)bits >= stbsp__powten[dg] ) ++tens;
      bits /= r;
    } 
    noround:;
  }

  // kill long trailing runs of zeros
  if ( bits )
  {
    stbsp__uint32 n;
    for(;;) { if ( bits<=0xffffffff ) break; if (bits%1000) goto donez; bits/=1000; }
    n = (stbsp__uint32)bits;
    while ((n%1000)==0) n/=1000;
    bits=n;
    donez:;
  }

  // convert to string
  out += 64;
  e = 0; 
  for(;;)
  {
    stbsp__uint32 n;
    char * o = out-8;
    // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
    if (bits>=100000000) { n = (stbsp__uint32)( bits % 100000000);  bits /= 100000000; } else {n = (stbsp__uint32)bits; bits = 0; }
    while(n) { out-=2; *(stbsp__uint16*)out=*(stbsp__uint16*)&stbsp__digitpair[(n%100)*2]; n/=100; e+=2; }
    if (bits==0) { if ((e) && (out[0]=='0')) { ++out; --e; } break; }
    while( out!=o ) { *--out ='0'; ++e; }
  }
  
  *decimal_pos = tens;
  *start = out;
  *len = e;
  return ng;
}

#undef stbsp__ddmulthi
#undef stbsp__ddrenorm
#undef stbsp__ddmultlo
#undef stbsp__ddmultlos
#undef STBSP__SPECIAL 
#undef STBSP__COPYFP
 
#endif // STB_SPRINTF_NOFLOAT

// clean up
#undef stbsp__uint16
#undef stbsp__uint32 
#undef stbsp__int32 
#undef stbsp__uint64
#undef stbsp__int64
#undef STBSP__UNALIGNED

#endif // STB_SPRINTF_IMPLEMENTATION


/*
------------------------------------------------------------------------------
This software is available under 2 licenses -- choose whichever you prefer.
------------------------------------------------------------------------------
ALTERNATIVE A - MIT License
Copyright (c) 2017 Sean Barrett
Permission is hereby granted, free of charge, to any person obtaining a copy of 
this software and associated documentation files (the "Software"), to deal in 
the Software without restriction, including without limitation the rights to 
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies 
of the Software, and to permit persons to whom the Software is furnished to do 
so, subject to the following conditions:
The above copyright notice and this permission notice shall be included in all 
copies or substantial portions of the Software.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, 
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE 
SOFTWARE.
------------------------------------------------------------------------------
ALTERNATIVE B - Public Domain (www.unlicense.org)
This is free and unencumbered software released into the public domain.
Anyone is free to copy, modify, publish, use, compile, sell, or distribute this 
software, either in source code form or as a compiled binary, for any purpose, 
commercial or non-commercial, and by any means.
In jurisdictions that recognize copyright laws, the author or authors of this 
software dedicate any and all copyright interest in the software to the public 
domain. We make this dedication for the benefit of the public at large and to 
the detriment of our heirs and successors. We intend this dedication to be an 
overt act of relinquishment in perpetuity of all present and future rights to 
this software under copyright law.
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN 
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION 
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
------------------------------------------------------------------------------
*/
