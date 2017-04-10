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
#include "math.h"

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

#define DQNT_PI 3.14159265359f
#define DQNT_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQNT_DEGREES_TO_RADIANS(x) ((x * (DQNT_PI / 180.0f)))
#define DQNT_RADIANS_TO_DEGREES(x) ((x * (180.0f / DQNT_PI)))
#define DQNT_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQNT_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQNT_SQUARED(x) ((x) * (x))

////////////////////////////////////////////////////////////////////////////////
// DArray - Dynamic Array
////////////////////////////////////////////////////////////////////////////////
// The DArray stores metadata in the header and returns you a pointer straight
// to the data, to allow direct read/modify access. Adding elements should be
// done using the provided functions since it manages internal state.

/*
	Example Usage:

	uint32_t *uintArray = DQNT_DARRAY_INIT(uint32_t, 16);
	uint32_t numberA = 48;
	uint32_t numberB = 52;
	DQNT_DARRAY_PUSH(&uintArray, &numberA);
	DQNT_DARRAY_PUSH(&uintArray, &numberB);

	for (uint32_t i = 0; i < dqnt_darray_get_num_items(uintArray); i++)
	{
	    printf(%d\n", uintArray[i]);
	}

	dqnt_darray_free(uintArray);
 */

// The init macro RETURNS a pointer to your type, you can index this as normal
// with array notation [].
// u32 type             - The data type to initiate a dynamic array with
// u32 startingCapacity - Initial number of available slots
#define DQNT_DARRAY_INIT(type, startingCapacity)                               \
	(type *)dqnt_darray_init_internal(sizeof(type), startingCapacity)

// Pass in the pointer returned by DQNT_DARRAY_INIT. If the pointer is not
// a valid DArray pointer, this will return 0.
DQNT_FILE_SCOPE u32 dqnt_darray_get_capacity(void *array);
DQNT_FILE_SCOPE u32 dqnt_darray_get_num_items(void *array);

// void **array - the address of the pointer returned by DQNT_DARRAY_INIT
// void *item   - a pointer to the object to insert
// The push macro RETURNS true/false if the push was successful or not.
#define DQNT_DARRAY_PUSH(array, item)                                          \
	dqnt_darray_push_internal((void **)array, (void *)item, sizeof(*item))

// Pass in the pointer returned by DQNT_DARRAY_INIT. Returns if the free was
// successful. This will return false if the array is not a valid DArray and
// won't touch the pointer.
DQNT_FILE_SCOPE bool dqnt_darray_free(void *array);
////////////////////////////////////////////////////////////////////////////////
// Math
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE f32 dqnt_math_lerp(f32 a, f32 t, f32 b);
DQNT_FILE_SCOPE f32 dqnt_math_sqrtf(f32 a);

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
typedef union DqntV2 {
	struct { f32 x, y; };
	struct { f32 w, h; };
	struct { f32 min, max; };
	f32 e[2];
} DqntV2;

// Create a vector using ints and typecast to floats
DQNT_FILE_SCOPE DqntV2 dqnt_v2i(i32 x, i32 y);
DQNT_FILE_SCOPE DqntV2 dqnt_v2 (f32 x, f32 y);

DQNT_FILE_SCOPE DqntV2 dqnt_v2_add     (DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE DqntV2 dqnt_v2_sub     (DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE DqntV2 dqnt_v2_scale   (DqntV2 a, f32 b);
DQNT_FILE_SCOPE DqntV2 dqnt_v2_hadamard(DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE f32    dqnt_v2_dot     (DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE bool   dqnt_v2_equals  (DqntV2 a, DqntV2 b);

DQNT_FILE_SCOPE f32    dqnt_v2_length_squared(DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE f32    dqnt_v2_length        (DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE DqntV2 dqnt_v2_normalise     (DqntV2 a);
DQNT_FILE_SCOPE bool   dqnt_v2_overlaps      (DqntV2 a, DqntV2 b);
DQNT_FILE_SCOPE DqntV2 dqnt_v2_perpendicular (DqntV2 a);

// Resize the dimension to fit the aspect ratio provided. Downscale only.
DQNT_FILE_SCOPE DqntV2 dqnt_v2_constrain_to_ratio(DqntV2 dim, DqntV2 ratio);

////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
typedef union DqntV3
{
	struct { f32 x, y, z; };
    struct { f32 r, g, b; };
	f32 e[3];
} DqntV3;

// Create a vector using ints and typecast to floats
DQNT_FILE_SCOPE DqntV3 dqnt_v3i(i32 x, i32 y, i32 z);
DQNT_FILE_SCOPE DqntV3 dqnt_v3 (f32 x, f32 y, f32 z);

DQNT_FILE_SCOPE DqntV3 dqnt_v3_add     (DqntV3 a, DqntV3 b);
DQNT_FILE_SCOPE DqntV3 dqnt_v3_sub     (DqntV3 a, DqntV3 b);
DQNT_FILE_SCOPE DqntV3 dqnt_v3_scale   (DqntV3 a, f32 b);
DQNT_FILE_SCOPE DqntV3 dqnt_v3_hadamard(DqntV3 a, DqntV3 b);
DQNT_FILE_SCOPE f32    dqnt_v3_dot     (DqntV3 a, DqntV3 b);
DQNT_FILE_SCOPE bool   dqnt_v3_equals  (DqntV3 a, DqntV3 b);

DQNT_FILE_SCOPE DqntV3 dqnt_v3_cross(DqntV3 a, DqntV3 b);

////////////////////////////////////////////////////////////////////////////////
// Vec4
////////////////////////////////////////////////////////////////////////////////
typedef union DqntV4
{
	struct { f32 x, y, z, w; };
	struct { f32 r, g, b, a; };
	f32 e[4];
	DqntV2 v2[2];
} DqntV4;

// Create a vector using ints and typecast to floats
DQNT_FILE_SCOPE DqntV4 dqnt_v4i(i32 x, i32 y, i32 z);
DQNT_FILE_SCOPE DqntV4 dqnt_v4 (f32 x, f32 y, f32 z, f32 w);

DQNT_FILE_SCOPE DqntV4 dqnt_v4_add     (DqntV4 a, DqntV4 b);
DQNT_FILE_SCOPE DqntV4 dqnt_v4_sub     (DqntV4 a, DqntV4 b);
DQNT_FILE_SCOPE DqntV4 dqnt_v4_scale   (DqntV4 a, f32 b);
DQNT_FILE_SCOPE DqntV4 dqnt_v4_hadamard(DqntV4 a, DqntV4 b);
DQNT_FILE_SCOPE f32    dqnt_v4_dot     (DqntV4 a, DqntV4 b);
DQNT_FILE_SCOPE bool   dqnt_v4_equals  (DqntV4 a, DqntV4 b);

////////////////////////////////////////////////////////////////////////////////
// 4D Matrix Mat4
////////////////////////////////////////////////////////////////////////////////
typedef union DqntMat4
{
	DqntV4 col[4];
	// Column/row
	f32 e[4][4];
} DqntMat4;

DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_identity ();
DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_ortho    (f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_translate(f32 x, f32 y, f32 z);
DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_rotate   (f32 radians, f32 x, f32 y, f32 z);
DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_scale    (f32 x, f32 y, f32 z);
DQNT_FILE_SCOPE DqntMat4 dqnt_mat4_mul      (DqntMat4 a, DqntMat4 b);
DQNT_FILE_SCOPE DqntV4   dqnt_mat4_mul_vec4 (DqntMat4 a, DqntV4 b);

////////////////////////////////////////////////////////////////////////////////
// Other Math
////////////////////////////////////////////////////////////////////////////////
typedef struct DqntRect
{
	DqntV2 min;
	DqntV2 max;
} DqntRect;

DQNT_FILE_SCOPE DqntRect dqnt_rect            (DqntV2 origin, DqntV2 size);
DQNT_FILE_SCOPE void     dqnt_rect_get_size_2f(DqntRect rect, f32 *width, f32 *height);
DQNT_FILE_SCOPE DqntV2   dqnt_rect_get_size_v2(DqntRect rect);
DQNT_FILE_SCOPE DqntV2   dqnt_rect_get_centre (DqntRect rect);
DQNT_FILE_SCOPE DqntRect dqnt_rect_move       (DqntRect rect, DqntV2 shift);
DQNT_FILE_SCOPE bool     dqnt_rect_contains_p (DqntRect rect, DqntV2 p);

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

// Both return the number of bytes read, return 0 if invalid codepoint or UTF8
DQNT_FILE_SCOPE u32 dqnt_ucs_to_utf8(u32 *dest, u32 character);
DQNT_FILE_SCOPE u32 dqnt_utf8_to_ucs(u32 *dest, u32 character);

////////////////////////////////////////////////////////////////////////////////
// File Operations
////////////////////////////////////////////////////////////////////////////////
typedef struct DqntFile
{
	void *handle;
	u64   size;
} DqntFile;

// Open a handle to the file
DQNT_FILE_SCOPE bool dqnt_file_open(char *const file, DqntFile *fileHandle);

// Return the number of bytes read
DQNT_FILE_SCOPE u32  dqnt_file_read (DqntFile file, void *buffer, u32 numBytesToRead);
DQNT_FILE_SCOPE void dqnt_file_close(DqntFile *file);

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
// DArray - Dynamic Array
////////////////////////////////////////////////////////////////////////////////
#define DQNT_DARRAY_SIGNATURE_INTERNAL 0xAC83DB81
typedef struct DqntDArrayInternal
{
	u32  index;
	u32  itemSize;
	u32  capacity;
	u32  signature;

	void *data;
} DqntDArrayInternal;

FILE_SCOPE void *dqnt_darray_init_internal(u32 itemSize, u32 startingCapacity)
{
	if (startingCapacity <= 0 || itemSize == 0) return NULL;

	u32 metadataSize = sizeof(DqntDArrayInternal);
	u32 storageSize  = itemSize * startingCapacity;

	void *memory = calloc(1, metadataSize + storageSize);
	if (!memory) return NULL;

	DqntDArrayInternal *array = (DqntDArrayInternal *)memory;
	array->signature  = DQNT_DARRAY_SIGNATURE_INTERNAL;
	array->itemSize   = itemSize;
	array->capacity   = startingCapacity;
	array->data       = (u8 *)memory + metadataSize;

	return array->data;
}

FILE_SCOPE DqntDArrayInternal *dqnt_darray_get_header_internal(void *array)
{
	if (!array) return NULL;

	DqntDArrayInternal *result = (DqntDArrayInternal *)((u8 *)array - sizeof(DqntDArrayInternal));
	if (result->signature != DQNT_DARRAY_SIGNATURE_INTERNAL) return 0;

	return result;
}

DQNT_FILE_SCOPE u32 dqnt_darray_get_capacity(void *array)
{
	DqntDArrayInternal *header = dqnt_darray_get_header_internal(array);
	if (!header) return 0;
	return header->capacity;
}

DQNT_FILE_SCOPE u32 dqnt_darray_get_num_items(void *array)
{
	DqntDArrayInternal *header = dqnt_darray_get_header_internal(array);
	if (!header) return 0;
	return header->index;
}

bool dqnt_darray_push_internal(void **array, void *element, u32 itemSize)
{
	if (!element || !array) return false;

	DqntDArrayInternal *header = dqnt_darray_get_header_internal(*array);
	if (!header || header->itemSize != itemSize) return false;

	if (header->index >= header->capacity)
	{
		const f32 GROWTH_FACTOR = 1.2f;
		u32 newCapacity         = (i32)(header->capacity * GROWTH_FACTOR);
		if (newCapacity == header->capacity) newCapacity++;

		u32 metadataSize = sizeof(DqntDArrayInternal);
		u32 storageSize  = header->itemSize * newCapacity;
		void *newMem     = realloc(header, metadataSize + storageSize);
		if (newMem)
		{
			header           = (DqntDArrayInternal *)newMem;
			header->capacity = newCapacity;
			header->data     = (u8 *)newMem + metadataSize;
			*array           = header->data;
		}
		else
		{
			return false;
		}
	}

	u32 arrayOffset = header->itemSize * header->index++;
	DQNT_ASSERT(header->index <= header->capacity);
	u8 *dataPtr = (u8 *)header->data;

	void *dest = (void *)&dataPtr[arrayOffset];
	void *src  = element;
	memcpy(dest, src, header->itemSize);

	return true;
}

DQNT_FILE_SCOPE inline bool dqnt_darray_free(void *array)
{
	DqntDArrayInternal *header = dqnt_darray_get_header_internal(array);
	if (header)
	{
		header->index     = 0;
		header->itemSize  = 0;
		header->capacity  = 0;
		header->signature = 0;

		free(header);
		return true;
	}
	else
	{
		return false;
	}
}

////////////////////////////////////////////////////////////////////////////////
// Math
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE f32 dqnt_math_lerp(f32 a, f32 t, f32 b)
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

DQNT_FILE_SCOPE f32 dqnt_math_sqrtf(f32 a)
{
	f32 result = sqrtf(a);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec2
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline DqntV2 dqnt_v2(f32 x, f32 y)
{
	DqntV2 result = {};
	result.x  = x;
	result.y  = y;

	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2i(i32 x, i32 y)
{
	DqntV2 result = dqnt_v2((f32)x, (f32)y);
	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_add(DqntV2 a, DqntV2 b)
{
	DqntV2 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_sub(DqntV2 a, DqntV2 b)
{
	DqntV2 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_scale(DqntV2 a, f32 b)
{
	DqntV2 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_hadamard(DqntV2 a, DqntV2 b)
{
	DqntV2 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline f32 dqnt_v2_dot(DqntV2 a, DqntV2 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQNT_FILE_SCOPE inline bool dqnt_v2_equals(DqntV2 a, DqntV2 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

DQNT_FILE_SCOPE inline f32 dqnt_v2_length_squared(DqntV2 a, DqntV2 b)
{
	f32 x      = b.x - a.x;
	f32 y      = b.y - a.y;
	f32 result = (DQNT_SQUARED(x) + DQNT_SQUARED(y));
	return result;
}

DQNT_FILE_SCOPE inline f32 dqnt_v2_length(DqntV2 a, DqntV2 b)
{
	f32 lengthSq = dqnt_v2_length_squared(a, b);
	f32 result   = dqnt_math_sqrtf(lengthSq);
	return result;
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_normalise(DqntV2 a)
{
	f32 magnitude = dqnt_v2_length(dqnt_v2(0, 0), a);
	DqntV2 result = dqnt_v2(a.x, a.y);
	result        = dqnt_v2_scale(a, 1 / magnitude);
	return result;
}

DQNT_FILE_SCOPE inline bool dqnt_v2_overlaps(DqntV2 a, DqntV2 b)
{
	bool result = false;
	
	f32 lenOfA = a.max - a.min;
	f32 lenOfB = b.max - b.min;

	if (lenOfA > lenOfB)
	{
		DqntV2 tmp = a;
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

DQNT_FILE_SCOPE inline DqntV2 dqnt_v2_perpendicular(DqntV2 a)
{
	DqntV2 result = {a.y, -a.x};
	return result;
}


DQNT_FILE_SCOPE DqntV2 dqnt_v2_constrain_to_ratio(DqntV2 dim, DqntV2 ratio)
{
	DqntV2 result                  = {};
	f32 numRatioIncrementsToWidth  = (f32)(dim.w / ratio.w);
	f32 numRatioIncrementsToHeight = (f32)(dim.h / ratio.h);

	f32 leastIncrementsToSide =
	    DQNT_MIN(numRatioIncrementsToHeight, numRatioIncrementsToWidth);

	result.w = (f32)(ratio.w * leastIncrementsToSide);
	result.h = (f32)(ratio.h * leastIncrementsToSide);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec3
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline DqntV3 dqnt_v3(f32 x, f32 y, f32 z)
{
	DqntV3 result = {};
	result.x       = x;
	result.y       = y;
	result.z       = z;
	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3i(i32 x, i32 y, i32 z)
{
	DqntV3 result = dqnt_v3((f32)x, (f32)y, (f32)z);
	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3_add(DqntV3 a, DqntV3 b)
{
	DqntV3 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3_sub(DqntV3 a, DqntV3 b)
{
	DqntV3 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3_scale(DqntV3 a, f32 b)
{
	DqntV3 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3_hadamard(DqntV3 a, DqntV3 b)
{
	DqntV3 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline f32 dqnt_v3_dot(DqntV3 a, DqntV3 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQNT_FILE_SCOPE inline bool dqnt_v3_equals(DqntV3 a, DqntV3 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

DQNT_FILE_SCOPE inline DqntV3 dqnt_v3_cross(DqntV3 a, DqntV3 b)
{
	/*
	   CROSS PRODUCT
	   Generate a perpendicular vector to the 2 vectors
	   |a|   |d|   |bf - ce|
	   |b| x |e| = |cd - af|
	   |c|   |f|   |ae - be|
	 */
	DqntV3 result = {};
	result.e[0] = (a.e[1] * b.e[2]) - (a.e[2] * b.e[1]);
	result.e[1] = (a.e[2] * b.e[0]) - (a.e[0] * b.e[2]);
	result.e[2] = (a.e[0] * b.e[1]) - (a.e[1] * b.e[0]);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// Vec4
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline DqntV4 dqnt_v4(f32 x, f32 y, f32 z, f32 w)
{
	DqntV4 result = {x, y, z, w};
	return result;
}

DQNT_FILE_SCOPE inline DqntV4 dqnt_v4i(i32 x, i32 y, i32 z, i32 w) {
	DqntV4 result = dqnt_v4((f32)x, (f32)y, (f32)z, (f32)w);
	return result;
}

DQNT_FILE_SCOPE inline DqntV4 dqnt_v4_add(DqntV4 a, DqntV4 b)
{
	DqntV4 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV4 dqnt_v4_sub(DqntV4 a, DqntV4 b)
{
	DqntV4 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline DqntV4 dqnt_v4_scale(DqntV4 a, f32 b)
{
	DqntV4 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQNT_FILE_SCOPE inline DqntV4 dqnt_v4_hadamard(DqntV4 a, DqntV4 b)
{
	DqntV4 result;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQNT_FILE_SCOPE inline f32 dqnt_v4_dot(DqntV4 a, DqntV4 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQNT_FILE_SCOPE inline bool dqnt_v4_equals(DqntV4 a, DqntV4 b)
{
	bool result = TRUE;
	for (i32 i = 0; i < DQNT_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = FALSE;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// 4D Matrix Mat4
////////////////////////////////////////////////////////////////////////////////
DQNT_FILE_SCOPE inline DqntMat4 dqnt_mat4_identity()
{
	DqntMat4 result    = {0};
	result.e[0][0] = 1;
	result.e[1][1] = 1;
	result.e[2][2] = 1;
	result.e[3][3] = 1;
	return result;
}

DQNT_FILE_SCOPE inline DqntMat4
dqnt_mat4_ortho(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar)
{
	DqntMat4 result = dqnt_mat4_identity();
	result.e[0][0] = +2.0f   / (right - left);
	result.e[1][1] = +2.0f   / (top   - bottom);
	result.e[2][2] = -2.0f   / (zFar  - zNear);

	result.e[3][0] = -(right + left)   / (right - left);
	result.e[3][1] = -(top   + bottom) / (top   - bottom);
	result.e[3][2] = -(zFar  + zNear)  / (zFar  - zNear);

	return result;
}

DQNT_FILE_SCOPE inline DqntMat4 dqnt_mat4_translate(f32 x, f32 y, f32 z)
{
	DqntMat4 result = dqnt_mat4_identity();
	result.e[3][0] = x;
	result.e[3][1] = y;
	result.e[3][2] = z;
	return result;
}

DQNT_FILE_SCOPE inline DqntMat4 dqnt_mat4_rotate(f32 radians, f32 x, f32 y, f32 z)
{
	DqntMat4 result = dqnt_mat4_identity();
	f32 sinVal = sinf(radians);
	f32 cosVal = cosf(radians);

	result.e[0][0] = (cosVal + (DQNT_SQUARED(x) * (1.0f - cosVal)));
	result.e[0][1] = ((y * z * (1.0f - cosVal)) + (z * sinVal));
	result.e[0][2] = ((z * x * (1.0f - cosVal)) - (y * sinVal));

	result.e[1][0] = ((x * y * (1.0f - cosVal)) - (z * sinVal));
	result.e[1][1] = (cosVal + (DQNT_SQUARED(y) * (1.0f - cosVal)));
	result.e[1][2] = ((z * y * (1.0f - cosVal)) + (x * sinVal));

	result.e[2][0] = ((x * z * (1.0f - cosVal)) + (y * sinVal));
	result.e[2][1] = ((y * z * (1.0f - cosVal)) - (x * sinVal));
	result.e[2][2] = (cosVal + (DQNT_SQUARED(z) * (1.0f - cosVal)));

	result.e[3][3] = 1;

	return result;
}

DQNT_FILE_SCOPE inline DqntMat4 dqnt_mat4_scale(f32 x, f32 y, f32 z)
{
	DqntMat4 result = {0};
	result.e[0][0] = x;
	result.e[1][1] = y;
	result.e[2][2] = z;
	result.e[3][3] = 1;
	return result;
}

DQNT_FILE_SCOPE inline DqntMat4 dqnt_mat4_mul(DqntMat4 a, DqntMat4 b)
{
	DqntMat4 result = {0};
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

DQNT_FILE_SCOPE inline DqntV4 dqnt_mat4_mul_vec4(DqntMat4 a, DqntV4 b)
{
	DqntV4 result = {0};

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
DQNT_FILE_SCOPE inline DqntRect dqnt_rect(DqntV2 origin, DqntV2 size)
{
	DqntRect result = {};
	result.min      = origin;
	result.max      = dqnt_v2_add(origin, size);

	return result;
}

DQNT_FILE_SCOPE inline void dqnt_rect_get_size_2f(DqntRect rect, f32 *width, f32 *height)
{
	*width  = DQNT_ABS(rect.max.x - rect.min.x);
	*height = DQNT_ABS(rect.max.y - rect.min.y);
}

DQNT_FILE_SCOPE inline DqntV2 dqnt_rect_get_size_v2(DqntRect rect)
{
	f32 width     = DQNT_ABS(rect.max.x - rect.min.x);
	f32 height    = DQNT_ABS(rect.max.y - rect.min.y);
	DqntV2 result = dqnt_v2(width, height);
	return result;
}


DQNT_FILE_SCOPE inline DqntV2 dqnt_rect_get_centre(DqntRect rect)
{
	f32 sumX  = rect.min.x + rect.max.x;
	f32 sumY  = rect.min.y + rect.max.y;
	DqntV2 result = dqnt_v2_scale(dqnt_v2(sumX, sumY), 0.5f);
	return result;
}

DQNT_FILE_SCOPE inline DqntRect dqnt_rect_move(DqntRect rect, DqntV2 shift)
{
	DqntRect result = {0};
	result.min       = dqnt_v2_add(rect.min, shift);
	result.max       = dqnt_v2_add(rect.max, shift);

	return result;
}

DQNT_FILE_SCOPE inline bool dqnt_rect_contains_p(DqntRect rect, DqntV2 p)
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

	i32 val = DQNT_ABS(value);
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
DQNT_FILE_SCOPE u32 dqnt_ucs_to_utf8(u32 *dest, u32 character)
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

DQNT_FILE_SCOPE u32 dqnt_utf8_to_ucs(u32 *dest, u32 character)
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
#ifdef DQNT_WIN32
	#define DQNT_WIN32_ERROR_BOX(text, title) MessageBoxA(NULL, text, title, MB_OK);

FILE_SCOPE bool dqnt_win32_utf8_to_wchar_internal(char *in, wchar_t *out,
                                                  i32 outLen)
{
	u32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, outLen-1);

	if (result == 0xFFFD || 0)
	{
		DQNT_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}

FILE_SCOPE bool dqnt_win32_wchar_to_utf8_internal(wchar_t *in, char *out,
                                                  i32 outLen)
{
	u32 result =
	    WideCharToMultiByte(CP_UTF8, 0, in, -1, out, outLen, NULL, NULL);

	if (result == 0xFFFD || 0)
	{
		DQNT_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}
#endif

DQNT_FILE_SCOPE bool dqnt_file_open(char *const path, DqntFile *file)
{
	if (!file || !path) return false;

#ifdef DQNT_WIN32
	wchar_t widePath[MAX_PATH] = {};
	dqnt_win32_utf8_to_wchar_internal(path, widePath,
	                                  DQNT_ARRAY_COUNT(widePath));

	HANDLE handle = CreateFileW(widePath, GENERIC_READ | GENERIC_WRITE, 0, NULL,
	                            OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		DQNT_WIN32_ERROR_BOX("CreateFile() failed.", NULL);
		return false;
	}

	LARGE_INTEGER size;
	if (GetFileSizeEx(handle, &size) == 0)
	{
		DQNT_WIN32_ERROR_BOX("GetFileSizeEx() failed.", NULL);
		return false;
	}

	file->handle = handle;
	file->size   = size.QuadPart;

#else
	return false;
#endif

	return true;
}

DQNT_FILE_SCOPE u32 dqnt_file_read(DqntFile file, u8 *buffer, u32 numBytesToRead)
{
	u32 numBytesRead = 0;
#ifdef DQNT_WIN32
	if (file.handle && buffer)
	{
		DWORD bytesRead    = 0;
		HANDLE win32Handle = file.handle;

		BOOL result = ReadFile(win32Handle, (void *)buffer, numBytesToRead,
		                       &bytesRead, NULL);

		// TODO(doyle): 0 also means it is completing async, but still valid
		if (result == 0)
		{
			DQNT_WIN32_ERROR_BOX("ReadFile() failed.", NULL);
		}

		numBytesRead = (u32)bytesRead;
	}
#endif

	return numBytesRead;
}

DQNT_FILE_SCOPE inline void dqnt_file_close(DqntFile *file)
{
#ifdef DQNT_WIN32
	if (file && file->handle)
	{
		CloseHandle(file->handle);
		file->handle = NULL;
		file->size   = 0;
	}
#endif
}

#include "stdio.h"
DQNT_FILE_SCOPE char **dqnt_dir_read(char *dir, u32 *numFiles)
{
	if (!dir) return NULL;
#ifdef DQNT_WIN32

	u32 currNumFiles = 0;
	wchar_t wideDir[MAX_PATH] = {};
	dqnt_win32_utf8_to_wchar_internal(dir, wideDir, DQNT_ARRAY_COUNT(wideDir));

	// Enumerate number of files first
	{
		WIN32_FIND_DATAW findData = {};
		HANDLE findHandle = FindFirstFileW(wideDir, &findData);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			DQNT_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
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
			DQNT_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
			return NULL;
		}


		char **list = (char **)calloc(1, sizeof(*list) * (currNumFiles));
		if (!list)
		{
			DQNT_WIN32_ERROR_BOX("calloc() failed.", NULL);
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

				DQNT_WIN32_ERROR_BOX("calloc() failed.", NULL);
				return NULL;
			}
		}

		i32 listIndex = 0;
		WIN32_FIND_DATAW findData = {};
		while (FindNextFileW(findHandle, &findData) != 0)
		{
			dqnt_win32_wchar_to_utf8_internal(
			    findData.cFileName, list[listIndex++], MAX_PATH);
		}

		*numFiles = currNumFiles;
		FindClose(findHandle);

		return list;
	}
#endif
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
