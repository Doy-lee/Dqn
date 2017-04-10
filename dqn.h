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
// The DArray stores metadata in the header and returns you a pointer straight
// to the data, to allow direct read/modify access. Adding elements should be
// done using the provided functions since it manages internal state.

/*
	Example Usage:

	uint32_t *uintArray = DQN_DARRAY_INIT(uint32_t, 16);
	uint32_t numberA = 48;
	uint32_t numberB = 52;
	DQN_DARRAY_PUSH(&uintArray, &numberA);
	DQN_DARRAY_PUSH(&uintArray, &numberB);

	for (uint32_t i = 0; i < dqn_darray_get_num_items(uintArray); i++)
	{
	    printf(%d\n", uintArray[i]);
	}

	dqn_darray_free(uintArray);
 */

// The init macro RETURNS a pointer to your type, you can index this as normal
// with array notation [].
// u32 type             - The data type to initiate a dynamic array with
// u32 startingCapacity - Initial number of available slots
#define DQN_DARRAY_INIT(type, startingCapacity)                               \
	(type *)dqn_darray_init_internal(sizeof(type), startingCapacity)

// Pass in the pointer returned by DQN_DARRAY_INIT. If the pointer is not
// a valid DArray pointer, this will return 0.
DQN_FILE_SCOPE u32 dqn_darray_get_capacity(void *array);
DQN_FILE_SCOPE u32 dqn_darray_get_num_items(void *array);

// void **array - the address of the pointer returned by DQN_DARRAY_INIT
// void *item   - a pointer to the object to insert
// The push macro RETURNS true/false if the push was successful or not.
#define DQN_DARRAY_PUSH(array, item)                                          \
	dqn_darray_push_internal((void **)array, (void *)item, sizeof(*item))

// Pass in the pointer returned by DQN_DARRAY_INIT. Returns if the free was
// successful. This will return false if the array is not a valid DArray and
// won't touch the pointer.
DQN_FILE_SCOPE bool dqn_darray_free(void *array);
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
DQN_FILE_SCOPE DqnV4 dqn_v4i(i32 x, i32 y, i32 z);
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

#ifdef _WIN32
	#define DQN_WIN32

	#include "Windows.h"
	#define WIN32_LEAN_AND_MEAN
#endif

////////////////////////////////////////////////////////////////////////////////
// DArray - Dynamic Array
////////////////////////////////////////////////////////////////////////////////
#define DQN_DARRAY_SIGNATURE_INTERNAL 0xAC83DB81
typedef struct DqnDArrayInternal
{
	u32  index;
	u32  itemSize;
	u32  capacity;
	u32  signature;

	void *data;
} DqnDArrayInternal;

FILE_SCOPE void *dqn_darray_init_internal(u32 itemSize, u32 startingCapacity)
{
	if (startingCapacity <= 0 || itemSize == 0) return NULL;

	u32 metadataSize = sizeof(DqnDArrayInternal);
	u32 storageSize  = itemSize * startingCapacity;

	void *memory = calloc(1, metadataSize + storageSize);
	if (!memory) return NULL;

	DqnDArrayInternal *array = (DqnDArrayInternal *)memory;
	array->signature  = DQN_DARRAY_SIGNATURE_INTERNAL;
	array->itemSize   = itemSize;
	array->capacity   = startingCapacity;
	array->data       = (u8 *)memory + metadataSize;

	return array->data;
}

FILE_SCOPE DqnDArrayInternal *dqn_darray_get_header_internal(void *array)
{
	if (!array) return NULL;

	DqnDArrayInternal *result = (DqnDArrayInternal *)((u8 *)array - sizeof(DqnDArrayInternal));
	if (result->signature != DQN_DARRAY_SIGNATURE_INTERNAL) return 0;

	return result;
}

DQN_FILE_SCOPE u32 dqn_darray_get_capacity(void *array)
{
	DqnDArrayInternal *header = dqn_darray_get_header_internal(array);
	if (!header) return 0;
	return header->capacity;
}

DQN_FILE_SCOPE u32 dqn_darray_get_num_items(void *array)
{
	DqnDArrayInternal *header = dqn_darray_get_header_internal(array);
	if (!header) return 0;
	return header->index;
}

bool dqn_darray_push_internal(void **array, void *element, u32 itemSize)
{
	if (!element || !array) return false;

	DqnDArrayInternal *header = dqn_darray_get_header_internal(*array);
	if (!header || header->itemSize != itemSize) return false;

	if (header->index >= header->capacity)
	{
		const f32 GROWTH_FACTOR = 1.2f;
		u32 newCapacity         = (i32)(header->capacity * GROWTH_FACTOR);
		if (newCapacity == header->capacity) newCapacity++;

		u32 metadataSize = sizeof(DqnDArrayInternal);
		u32 storageSize  = header->itemSize * newCapacity;
		void *newMem     = realloc(header, metadataSize + storageSize);
		if (newMem)
		{
			header           = (DqnDArrayInternal *)newMem;
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
	DQN_ASSERT(header->index <= header->capacity);
	u8 *dataPtr = (u8 *)header->data;

	void *dest = (void *)&dataPtr[arrayOffset];
	void *src  = element;
	memcpy(dest, src, header->itemSize);

	return true;
}

DQN_FILE_SCOPE inline bool dqn_darray_free(void *array)
{
	DqnDArrayInternal *header = dqn_darray_get_header_internal(array);
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
	#define DQN_WIN32_ERROR_BOX(text, title) MessageBoxA(NULL, text, title, MB_OK);

FILE_SCOPE bool dqn_win32_utf8_to_wchar_internal(char *in, wchar_t *out,
                                                  i32 outLen)
{
	u32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, outLen-1);

	if (result == 0xFFFD || 0)
	{
		DQN_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}

FILE_SCOPE bool dqn_win32_wchar_to_utf8_internal(wchar_t *in, char *out,
                                                  i32 outLen)
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
	dqn_win32_utf8_to_wchar_internal(path, widePath,
	                                  DQN_ARRAY_COUNT(widePath));

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
	dqn_win32_utf8_to_wchar_internal(dir, wideDir, DQN_ARRAY_COUNT(wideDir));

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
			dqn_win32_wchar_to_utf8_internal(
			    findData.cFileName, list[listIndex++], MAX_PATH);
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

	// Convert to seconds
	f64 timestamp =
	    (f64)(qpcResult.QuadPart / queryPerformanceFrequency.QuadPart);
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
