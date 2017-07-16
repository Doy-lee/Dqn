////////////////////////////////////////////////////////////////////////////////
// Dqn.h Usage
////////////////////////////////////////////////////////////////////////////////
/*
	#define DQN_IMPLEMENTATION       // Enable the implementation

	// Define this wherever you want access to DQN code that uses the platform.
	#define DQN_PLATFORM_HEADER      // Enable function prototypes for xplatform/platform code

	// NOTE: For platform code, it's one or the other or you will get compilation problems.
	// Define this in ONE and only ONE file to enable the implementation of platform code.
	#define DQN_WIN32_IMPLEMENTATION // Enable Win32 Code, but only if _WIN32 or _WIN64 is already defined. Also requires DQN_IMPLEMENTATION.
	#define DQN_UNIX_IMPLEMENTATION  // Enable Unix Code, but only if __linux__ is already defined. Also requires DQN_IMPLEMENTATION.

	#define DQN_MAKE_STATIC          // Make all functions be static
	#include "dqn.h"
 */

// Conventions
// All data structures fields are exposed by default, with exceptions here and there. The rationale
// is I rarely go into libraries and start changing values around in fields unless I know the
// implementation OR we're required to fill out a struct for some function.

// Just treat all struct fields to be internal and read-only unless explicitly stated otherwise.

////////////////////////////////////////////////////////////////////////////////
// Table Of Contents #TOC #TableOfContents
////////////////////////////////////////////////////////////////////////////////
// You can search by #<entry> to jump straight to the section.
// The first match is the public API, the next matche(s) are the implementation

// #Portable Code
// #DqnAssert    Assertions
// #DqnMem       Memory Allocation
// #DqnMemStack  Memory Allocator, Push, Pop Style
// #DqnMemAPI    Custom memory API for Dqn Data Structures
// #DqnArray     CPP Dynamic Array with Templates
// #DqnMath      Simple Math Helpers (Lerp etc.)
// #DqnV2        2D  Math Vectors
// #DqnV3        3D  Math Vectors
// #DqnV4        4D  Math Vectors
// #DqnMat4      4x4 Math Matrix
// #DqnRect      Rectangles
// #DqnChar      Char  Operations (IsDigit(), IsAlpha() etc)
// #DqnStr       Str   Operations (Str_Len(), Str_Copy() etc)
// #DqnWChar     WChar Operations (IsDigit(), IsAlpha() etc)
// #DqnWStr      WStr  Operations (WStr_Len() etc)
// #DqnRnd       Random Number Generator (ints and floats)
// #Dqn_*        Dqn_QuickSort

// #XPlatform (Win32 & Unix)
// #DqnFile      File I/O (Read, Write, Delete)
// #DqnDir       Directory Querying
// #DqnTimer     High Resolution Timer
// #DqnLock      Mutex Synchronisation
// #DqnJobQueue  Multithreaded Job Queue
// #DqnAtomic    Interlocks/Atomic Operations
// #DqnPlatform  Common Platform API helpers

// #Platform
// - #Win32Platform
//   - #DqnWin32     Common Win32 API Helpers

// #External Code
// #DqnIni       Simple INI Config File API (Public Domain lib by Mattias Gustavsson)
// #DqnSprintf   Cross-platform Sprintf Implementation (Public Domain lib stb_sprintf)

// TODO
// - DqnMemStack
//   - Allow 0 size memblock stack initialisation/block-less stack for situations where you don't
//     care about specifying a size upfront
//   - Default block size for when new blocks are required
//
// - Win32
//   - Get rid of reliance on MAX_PATH
//
// - Make lib compile and run on Linux with GCC using -03

////////////////////////////////////////////////////////////////////////////////
// Preprocessor Checks
////////////////////////////////////////////////////////////////////////////////
// This needs to be above the portable layer so  that, if the user requests
// a platform implementation, platform specific implementations in the portable
// layer will get activated.
#if (defined(_WIN32) || defined(_WIN64))
	#define DQN_IS_WIN32 1
#elif defined(__linux__)
	#define DQN_IS_UNIX 1
#endif

#if defined(DQN_IS_WIN32) && defined(DQN_WIN32_IMPLEMENTATION)
	#define DQN_XPLATFORM_LAYER 1
	#define DQN_WIN32_PLATFORM 1
#elif defined(DQN_IS_UNIX) && defined(DQN_UNIX_IMPLEMENTATION)
	#define DQN_XPLATFORM_LAYER 1
	#define DQN_UNIX_PLATFORM 1
#endif

#if defined(__cplusplus)
	#define DQN_CPP_MODE 1
#endif

////////////////////////////////////////////////////////////////////////////////
// #Portable Code
////////////////////////////////////////////////////////////////////////////////
#ifndef DQN_H
#define DQN_H

#ifdef DQN_MAKE_STATIC
	#define DQN_FILE_SCOPE static
#else
	#define DQN_FILE_SCOPE
#endif

#include <stdint.h> // For standard types
#include <stddef.h> // For standard types
#include <string.h> // memmove
#include <float.h>
#define LOCAL_PERSIST static
#define FILE_SCOPE    static

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t  u8;

typedef int64_t i64;
typedef int32_t i32;
typedef int16_t i16;

typedef double f64;
typedef float  f32;

#define DQN_F32_MIN -FLT_MAX

#define DQN_TERABYTE(val) (DQN_GIGABYTE(val) * 1024LL)
#define DQN_GIGABYTE(val) (DQN_MEGABYTE(val) * 1024LL)
#define DQN_MEGABYTE(val) (DQN_KILOBYTE(val) * 1024LL)
#define DQN_KILOBYTE(val) ((val) * 1024LL)

#define DQN_ALIGN_POW_N(val, align) ((((size_t)val) + ((size_t)align-1)) & (~(size_t)(align-1)))
#define DQN_ALIGN_POW_4(val)        DQN_ALIGN_POW_N(val, 4)

#define DQN_INVALID_CODE_PATH 0
#define DQN_ARRAY_COUNT(array) (sizeof(array) / sizeof(array[0]))

#define DQN_PI 3.14159265359f
#define DQN_SQUARED(x) ((x) * (x))
#define DQN_ABS(x) (((x) < 0) ? (-(x)) : (x))
#define DQN_DEGREES_TO_RADIANS(x) ((x * (DQN_PI / 180.0f)))
#define DQN_RADIANS_TO_DEGREES(x) ((x * (180.0f / DQN_PI)))

#define DQN_MAX(a, b) ((a) < (b) ? (b) : (a))
#define DQN_MIN(a, b) ((a) < (b) ? (a) : (b))
#define DQN_SWAP(type, a, b) do { type tmp = a; a = b; b = tmp; } while(0)

////////////////////////////////////////////////////////////////////////////////
// #DqnAssert Public API - Assertions
////////////////////////////////////////////////////////////////////////////////
// DQN_ASSERT() & DQN_ASSERT_MSG() will hard break the program but it can be
// disabled in DqnAssertInternal() for release whilst still allowing the assert
// expressions to be evaluated and instead send diagnostics to console.

// NOTE: "## __VA_ARGS__" is a GCC hack. Zero variadic arguments won't compile
// because there will be a trailing ',' at the end. Pasting it swallows it. MSVC
// implicitly swallows the trailing comma.

// returns: If the expr was true or not.
#define DQN_ASSERT(expr)               DqnAssertInternal(expr, __FILE__, __LINE__, #expr, NULL)
#define DQN_ASSERT_MSG(expr, msg, ...) DqnAssertInternal(expr, __FILE__, __LINE__, #expr, msg, ## __VA_ARGS__)

// Usage example. This protects code against asserts that should fire in release
// mode by still letting the expression evaluate and the ability to redirect
// the code flow to some recovery path.
#if 0
	int *mem = (int *)malloc(sizeof(*mem));
	if (DQN_ASSERT_MSG(mem, "Not enough memory for malloc")) {
	    // success
	} else {
	    // failed
	}
#endif

// Internal implementation should not be used as the macro above will handle it, but is required in
// header for visibility to external functions calling it.
// returns: If the expr was true or not.
DQN_FILE_SCOPE bool DqnAssertInternal(const bool result, const char *const file, const i32 lineNum,
                                      const char *const expr, const char *const msg, ...);

// Hard assert causes an immediate program break at point of assertion by trying
// to modify the 0th mem-address.
#define DQN_ASSERT_HARD(expr) if (!(expr)) { *((int *)0) = 0; }

// Assert at compile time by making a type that is invalid depending on the expression result
#define DQN_COMPILE_ASSERT(expr)                DQN_COMPILE_ASSERT_INTERNAL(expr, __COUNTER__)
#define DQN_COMPILE_ASSERT_INTERNAL(expr, guid) typedef char DqnCompileAssertInternal_##guid[((int)(expr)) * 2 - 1];

////////////////////////////////////////////////////////////////////////////////
// #DqnMem Public API - Memory Allocation
////////////////////////////////////////////////////////////////////////////////
// TODO(doyle): Use platform allocation, fallback to malloc if platform not defined
DQN_FILE_SCOPE void *DqnMem_Alloc  (const size_t size);
DQN_FILE_SCOPE void *DqnMem_Calloc (const size_t size);
DQN_FILE_SCOPE void  DqnMem_Clear  (void *const memory, const u8 clearValue, const size_t size);
DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, const size_t newSize);
DQN_FILE_SCOPE void  DqnMem_Free   (void *memory);

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStack Public API - Memory Allocator, Push, Pop Style
////////////////////////////////////////////////////////////////////////////////
// DqnMemStack is an memory allocator in a stack like, push-pop style. It pre-allocates a block of
// memory at init and sub-allocates from this block to take advantage of memory locality.

// When an allocation requires a larger amount of memory than available in the block then the
// MemStack will allocate a new block of sufficient size for you in DqnMemStack_Push(..). This DOES
// mean that there will be wasted spaceat the end of each block and is a tradeoff for memory
// locality against optimal space usage.

// How To Use:
// 1. Create a DqnMemStack struct and pass it into an initialisation function
//    - InitWithFixedMem() allows you to pass in your own memory which is
//      converted to a memory block. This disables dynamic allocation.
//      NOTE: Space is reserved in the given memory for MemStackBlock metadata.

//    - InitWithFixedSize() allows you to to disable dynamic allocations and
//      sub-allocate from the initial MemStack allocation size only.

// 2. Use DqnMemStack_Push(..) to allocate memory for use.
//    - "Freeing" memory is dealt by creating temporary MemStacks or using the
//      BeginTempRegion and EndTempRegion functions. Specifically freeing
//      individual items is typically not generalisable in this scheme.

enum DqnMemStackFlag
{
	DqnMemStackFlag_IsNotExpandable       = (1 << 0),

	// NOTE(doyle): Required to indicate we CAN'T free this memory when free is called.
	DqnMemStackFlag_IsFixedMemoryFromUser = (1 << 1),
};

typedef struct DqnMemStack
{
	// The memory block allocated for the stack
	struct DqnMemStackBlock *block;

	// Bits set from enum DqnMemStackFlag
	u32 flags;
	i32 tempRegionCount;

	// Allocations are address aligned to this value. Not to be modified as popping allocations uses this to realign size
	u32 byteAlign;

#if defined(DQN_CPP_MODE)
	// Initialisation API
	bool  InitWithFixedMem (u8 *const mem,     const size_t memSize, const u32 byteAlignment = 4);
	bool  InitWithFixedSize(const size_t size, const bool zeroClear, const u32 byteAlignment = 4);
	bool  Init             (const size_t size, const bool zeroClear, const u32 byteAlignment = 4);

	// Memory API
	void *Push(size_t size);
	void  Pop (void *const ptr, size_t size);
	void  Free();
	bool  FreeMemBlock  (DqnMemStackBlock *memBlock);
	bool  FreeLastBlock ();
	void  ClearCurrBlock(const bool zeroClear);

	// Temporary Regions API
	struct DqnMemStackTempRegion TempRegionBegin();
	void                         TempRegionEnd  (DqnMemStackTempRegion region);

	// Scoped Temporary Regions API
	struct DqnMemStackTempRegionGuard TempRegionGuard();

	// Advanced API
	DqnMemStackBlock *AllocateCompatibleBlock(size_t size, const bool zeroClear);
	bool              AttachBlock            (DqnMemStackBlock *const newBlock);
	bool              DetachBlock            (DqnMemStackBlock *const detachBlock);
    void              FreeDetachedBlock      (DqnMemStackBlock *memBlock);
#endif
} DqnMemStack;

////////////////////////////////////////////////////////////////////////////////
//  DqnMemStack Initialisation
////////////////////////////////////////////////////////////////////////////////
// Uses fixed memory given by user. All allocations from the MemStack will be suballocated from the
// given memory. Allocations fail after the MemStack becomes full.
// stack:     Pass in a pointer to a zero cleared DqnMemStack struct.
// mem:       Memory to use for the memory stack
// byteAlign: Set the alignment of memory addresses for all allocated items from the memory stack.
// return:    FALSE if args are invalid, or insufficient memSize.
DQN_FILE_SCOPE bool DqnMemStack_InitWithFixedMem (DqnMemStack *const stack, u8 *const mem, const size_t memSize, const u32 byteAlign = 4);

// The memory stack uses 1 initial allocation from the DqnMem_Alloc(). No further allocations are
// made. All allocations are suballocated from the first allocation.
// size: The amount of memory to allocate. Size gets aligned to the next "byteAlign"ed value.
DQN_FILE_SCOPE bool DqnMemStack_InitWithFixedSize(DqnMemStack *const stack, size_t size, const bool zeroClear, const u32 byteAlign = 4);

// Dynamically expandable stack. Akin to DqnMemStack_InitWithFixedSize() except if the MemStack does
// not have enough space for allocation it will automatically attach another MemBlock using
// DqnMem_Calloc().
DQN_FILE_SCOPE bool DqnMemStack_Init(DqnMemStack *const stack, size_t size, const bool zeroClear, const u32 byteAlign = 4);

////////////////////////////////////////////////////////////////////////////////
//  DqnMemStack Memory Operations
////////////////////////////////////////////////////////////////////////////////
// Allocate memory from the MemStack.
// size:   "size" gets aligned to the byte alignment of the stack. 
// return: NULL if out of space OR stack is using fixed memory/size OR stack full and platform malloc fails.
DQN_FILE_SCOPE void *DqnMemStack_Push(DqnMemStack *const stack, size_t size);

// Frees the given ptr. It MUST be the last allocated item in the stack, fails otherwise.
DQN_FILE_SCOPE bool  DqnMemStack_Pop(DqnMemStack *const stack, void *ptr, size_t size);

// Frees all blocks belonging to this stack.
DQN_FILE_SCOPE void  DqnMemStack_Free(DqnMemStack *const stack);

// Frees the specified block belonging to the stack.
// return: FALSE if block doesn't belong this into calls DqnMem_Free() or invalid args.
DQN_FILE_SCOPE bool  DqnMemStack_FreeMemBlock(DqnMemStack *const stack, DqnMemStackBlock *memBlock);

// Frees the last-most memory block. If last block, free that block making the MemStack blockless.
// Next allocate will attach a block.
DQN_FILE_SCOPE bool  DqnMemStack_FreeLastBlock(DqnMemStack *const stack);

// Reset the current memory block usage to 0.
DQN_FILE_SCOPE void  DqnMemStack_ClearCurrBlock(DqnMemStack *const stack, const bool zeroClear);

////////////////////////////////////////////////////////////////////////////////
//  DqnMemStack Temporary Regions
////////////////////////////////////////////////////////////////////////////////
// Lets the programmer mark a begin/end region in which all the memory allocated from the MemStack
// will be reverted upon reaching the DqnMemStackTempRegion_End() call. This includes the reverting
// of additional MemBlocks allocated in a dynamic stack and all other various MemStack configs.

// TODO(doyle): Look into a way of "preventing/guarding" against anual calls to free/clear in temp
// regions
typedef struct DqnMemStackTempRegion
{
	// The stack associated with this TempRegion
	DqnMemStack *stack;

	// Store memBlock state to revert back to on DqnMemStackTempRegion_End()
	struct DqnMemStackBlock *startingBlock;
	size_t used;
} DqnMemStackTempRegion;

// region: Takes pointer to a zero-cleared DqnMemStackTempRegion struct.
// return: FALSE if arguments are invalid.
DQN_FILE_SCOPE bool DqnMemStackTempRegion_Begin(DqnMemStackTempRegion *const region, DqnMemStack *const stack);
DQN_FILE_SCOPE void DqnMemStackTempRegion_End  (DqnMemStackTempRegion region);

#ifdef DQN_CPP_MODE
// Region guard automatically starts a region on construction and ends a region
// on destruction.
struct DqnMemStackTempRegionGuard
{
	// lock: Takes a pointer to a pre-existing and already initialised stack
	// bool: Pass in (optionally) a pointer to a bool which returns whether construction was successful.
	//       It can be FALSE if stack is NULL.
	DqnMemStackTempRegionGuard(DqnMemStack *const stack, bool *const succeeded);
	~DqnMemStackTempRegionGuard();

private:
	DqnMemStackTempRegion tempMemStack;
};
#endif

////////////////////////////////////////////////////////////////////////////////
// DqnMemStack Advanced API (OPTIONAL)
////////////////////////////////////////////////////////////////////////////////
// Blocks are freely modifiable if you want fine grained control. Size value and memory ptr should
// NOT be modified directly, only indirectly through the regular API.
typedef struct DqnMemStackBlock
{
	// The raw memory block, size and used count
	u8     *memory;
	size_t  size;
	size_t  used;

	// The allocator uses a linked list approach for additional blocks beyond capacity
	DqnMemStackBlock *prevBlock;
} DqnMemStackBlock;

// These are useful for forcing a new block to be used. AllocateCompatibleBlock() will fail if the
// supplied stack has flags set such that the stack is not allowed to have new blocks.
DQN_FILE_SCOPE DqnMemStackBlock *DqnMemStack_AllocateCompatibleBlock(const DqnMemStack *const stack, size_t size, const bool zeroClear);
DQN_FILE_SCOPE bool              DqnMemStack_AttachBlock            (DqnMemStack *const stack, DqnMemStackBlock *const newBlock);
DQN_FILE_SCOPE bool              DqnMemStack_DetachBlock            (DqnMemStack *const stack, DqnMemStackBlock *const detachBlock);

// (IMPORTANT) Should only be used to free blocks that haven't been attached! Attached blocks should
// be freed using DqnMemStack_FreeMemBlock().
DQN_FILE_SCOPE void DqnMemStack_FreeDetachedBlock(DqnMemStackBlock *memBlock);

////////////////////////////////////////////////////////////////////////////////
// #DqnMemAPI Public API - Custom memory API for Dqn Data Structures
////////////////////////////////////////////////////////////////////////////////
// You only need to care about this API if you want to use custom mem-alloc routines in the data
// structures! Otherwise it already has a default one to use.

// How To Use:
// 1. Implement the callback function, where DqnMemApiCallbackInfo will tell you the request.
//    - (NOTE) The callback should return the resulting data into DqnMemAPICallbackResult
// 2. Create a DqnMemAPI struct with a function ptr to your callback
//    - (OPTIONAL) Set the user context to your book-keeping/mem allocating service
// 3. Initialise any data structure that supports a DqnMemAPI with your struct.

// That's it! Done :) Of course, changing memAPI's after initialisation is invalid since the
// pointers belonging to your old routine may not be tracked in your new memAPI. So you're at your
// own discretion there.

enum DqnMemAPICallbackType
{
	DqnMemAPICallbackType_Invalid,
	DqnMemAPICallbackType_Alloc,
	DqnMemAPICallbackType_Realloc,
	DqnMemAPICallbackType_Free,
};

typedef struct DqnMemAPICallbackInfo
{
	void *userContext;
	enum DqnMemAPICallbackType type;
	union {
		 // DqnMemAPICallbackType_Alloc
		struct { size_t requestSize; };

		// DqnMemAPICallbackType_Free
		struct
		{
			void  *ptrToFree;
			size_t sizeToFree;
		};

		// DqnMemAPICallbackType_Realloc
		struct
		{
			size_t newRequestSize;
			void *oldMemPtr;
			size_t oldSize;
		};
	};
} DqnMemAPICallbackInfo;

typedef struct DqnMemAPICallbackResult
{
	// NOTE: CallbackResult on free has nothing to fill out for result.
	void *newMemPtr;
	enum DqnMemAPICallbackType type;
} DqnMemAPICallbackResult;

// Function prototype for implementing a DqnMemAPI_Callback. You must fill out the "result" structure.
// result: Is always guaranteed to be a valid pointer.
typedef void DqnMemAPI_Callback(DqnMemAPICallbackInfo info, DqnMemAPICallbackResult *result);

typedef struct DqnMemAPI
{
	DqnMemAPI_Callback *callback;
	void               *userContext;
} DqnMemAPI;

DQN_FILE_SCOPE DqnMemAPI DqnMemAPI_DefaultUseCalloc();
////////////////////////////////////////////////////////////////////////////////
// #DqnArray Public API - CPP Dynamic Array with Templates
////////////////////////////////////////////////////////////////////////////////
// Cplusplus mode only since it uses templates

#ifdef DQN_CPP_MODE
template <typename T>
struct DqnArray
{
	// Function pointers to custom allocators
	DqnMemAPI memAPI;

	// Array state
	u64 count;
	u64 capacity;
	T   *data;

	// API
	void  Init        (const size_t capacity, DqnMemAPI memAPI = DqnMemAPI_DefaultUseCalloc());
	bool  Free        ();
	bool  Grow        ();
	T    *Push        (const T item);
	void  Pop         ();
	T    *Get         (u64 index);
	bool  Clear       ();
	bool  Remove      (u64 index);
	bool  RemoveStable(u64 index);
};

FILE_SCOPE const char *const DQN_MEM_API_CALLBACK_RESULT_TYPE_INCORRECT =
    "DqnMemAPICallbackResult type is incorrect";

template <typename T>
bool DqnArray_Init(DqnArray<T> *const array, const size_t capacity,
                   const DqnMemAPI memAPI = DqnMemAPI_DefaultUseCalloc())
{
	if (!array) return false;
	if (array->data) DqnArray_Free(array);

	array->memAPI       = memAPI;
	size_t allocateSize = (size_t)capacity * sizeof(T);

	DqnMemAPICallbackResult memResult = {0};
	DqnMemAPICallbackInfo info = DqnMemAPIInternal_CallbackInfoAskAlloc(array->memAPI, allocateSize);
	array->memAPI.callback(info, &memResult);
	if (!DQN_ASSERT_MSG(memResult.type == DqnMemAPICallbackType_Alloc, DQN_MEM_API_CALLBACK_RESULT_TYPE_INCORRECT))
	{
		return false;
	}

	array->data = (T *)memResult.newMemPtr;
	if (!array->data) return false;

	array->count    = 0;
	array->capacity = capacity;
	return true;
}

// Implementation taken from Milton, developed by Serge at
// https://github.com/serge-rgb/milton#license
template <typename T>
bool DqnArray_Free(DqnArray<T> *const array)
{
	if (array && array->data)
	{
		// TODO(doyle): Right now we assume free always works, and it probably should?
		size_t sizeToFree = (size_t)array->capacity * sizeof(T);
		DqnMemAPICallbackInfo info = DqnMemAPIInternal_CallbackInfoAskFree(
		    array->memAPI, array->data, sizeToFree);
		array->memAPI.callback(info, NULL);
		array->data = NULL;

		array->count    = 0;
		array->capacity = 0;
		return true;
	}

	return false;
}

template <typename T>
bool DqnArray_Grow(DqnArray<T> *const array)
{
	if (!array || !array->data) return false;

	const f32 GROWTH_FACTOR = 1.2f;
	size_t newCapacity      = (size_t)(array->capacity * GROWTH_FACTOR);
	if (newCapacity == array->capacity) newCapacity++;

	size_t oldSize = (size_t)array->capacity * sizeof(T);
	size_t newSize = (size_t)newCapacity * sizeof(T);

	DqnMemAPICallbackResult memResult = {0};
	DqnMemAPICallbackInfo info = DqnMemAPIInternal_CallbackInfoAskRealloc(
	    array->memAPI, array->data, oldSize, newSize);

	array->memAPI.callback(info, &memResult);
	if (!DQN_ASSERT_MSG(memResult.type == DqnMemAPICallbackType_Realloc,
	                    DQN_MEM_API_CALLBACK_RESULT_TYPE_INCORRECT))
	{
		return false;
	}

	if (memResult.newMemPtr)
	{
		array->data     = (T *)memResult.newMemPtr;
		array->capacity = newCapacity;
		return true;
	}
	else
	{
		return false;
	}
}

template <typename T>
T *DqnArray_Push(DqnArray<T> *const array, const T item)
{
	if (!array) return NULL;

	if (array->count >= array->capacity)
	{
		if (!DqnArray_Grow(array)) return NULL;
	}

	DQN_ASSERT(array->count < array->capacity);
	array->data[array->count++] = item;

	return &array->data[array->count-1];
}

template <typename T>
void DqnArray_Pop(DqnArray<T> *array)
{
	if (!array) return;
	if (array->count == 0) return;
	array->count--;

	return;
}

template <typename T>
T *DqnArray_Get(DqnArray<T> *const array, const u64 index)
{
	T *result = NULL;
	if (index >= 0 && index <= array->count) result = &array->data[index];
	return result;
}

template <typename T>
bool DqnArray_Clear(DqnArray<T> *const array)
{
	if (array)
	{
		array->count = 0;
		return true;
	}

	return false;
}

template <typename T>
bool DqnArray_Remove(DqnArray<T> *const array, const u64 index)
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
bool DqnArray_RemoveStable(DqnArray<T> *const array, const u64 index)
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

template <typename T> void DqnArray<T>::Init (const size_t capacity, DqnMemAPI memAPI) { DqnArray_Init(this, capacity, memAPI); }
template <typename T> bool DqnArray<T>::Free ()                                        { return DqnArray_Free(this); }
template <typename T> bool DqnArray<T>::Grow ()                                        { return DqnArray_Grow(this);}
template <typename T> T*   DqnArray<T>::Push (const T item)                            { return DqnArray_Push(this, item); }
template <typename T> void DqnArray<T>::Pop  ()                                        { DqnArray_Pop(this); }
template <typename T> T*   DqnArray<T>::Get  (const u64 index)                         { return DqnArray_Get(this, index); }
template <typename T> bool DqnArray<T>::Clear()                                        { return DqnArray_Clear (this); }
template <typename T> bool DqnArray<T>::Remove      (const u64 index)                  { return DqnArray_Remove(this, index); }
template <typename T> bool DqnArray<T>::RemoveStable(const u64 index)                  { return DqnArray_RemoveStable(this, index); }

#endif // DQN_CPP_MODE

////////////////////////////////////////////////////////////////////////////////
// #DqnMath Public API - Simple Math Helpers
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f32 DqnMath_Lerp  (f32 a, f32 t, f32 b);
DQN_FILE_SCOPE f32 DqnMath_Sqrtf (f32 a);
DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max);

////////////////////////////////////////////////////////////////////////////////
// #DqnV2 Public API - 2D Math Vectors
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV2 {
	struct { f32 x, y; };
	struct { f32 w, h; };
	struct { f32 min, max; };
	f32 e[2];
} DqnV2;

typedef union DqnV2i {
	struct { i32 x, y; };
	struct { i32 w, h; };
	struct { i32 min, max; };
	i32 e[2];
} DqnV2i;

// DqnV2
DQN_FILE_SCOPE DqnV2 DqnV2_1f (f32 xy);
DQN_FILE_SCOPE DqnV2 DqnV2_2f (f32 x, f32 y);
DQN_FILE_SCOPE DqnV2 DqnV2_2i (i32 x, i32 y); // Typecasts 2 integers to 2 floats
DQN_FILE_SCOPE DqnV2 DqnV2_V2i(DqnV2i a);

DQN_FILE_SCOPE DqnV2 DqnV2_Add     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Sub     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalei  (DqnV2 a, i32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Scalef  (DqnV2 a, f32 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Dot     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE bool  DqnV2_Equals  (DqnV2 a, DqnV2 b);

DQN_FILE_SCOPE f32   DqnV2_LengthSquared(DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE f32   DqnV2_Length       (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Normalise    (DqnV2 a);
DQN_FILE_SCOPE bool  DqnV2_Overlaps     (DqnV2 a, DqnV2 b);
DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(DqnV2 a);

DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 srcSize, DqnV2 targetSize);
DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio     (DqnV2 dim, DqnV2 ratio); // Resize the dimension to fit the aspect ratio provided. Downscale only.

#ifdef DQN_CPP_MODE
DQN_FILE_SCOPE inline DqnV2  operator- (DqnV2  a, DqnV2 b) { return      DqnV2_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator+ (DqnV2  a, DqnV2 b) { return      DqnV2_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, DqnV2 b) { return      DqnV2_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, f32   b) { return      DqnV2_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2  operator* (DqnV2  a, i32   b) { return      DqnV2_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, f32   b) { return (a = DqnV2_Scalef  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator*=(DqnV2 &a, i32   b) { return (a = DqnV2_Scalei  (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator-=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2 &operator+=(DqnV2 &a, DqnV2 b) { return (a = DqnV2_Add     (a, b)); }
DQN_FILE_SCOPE inline bool   operator==(DqnV2  a, DqnV2 b) { return      DqnV2_Equals  (a, b);  }
#endif

// DqnV2i
DQN_FILE_SCOPE DqnV2i DqnV2i_2i(i32 x, i32 y);
DQN_FILE_SCOPE DqnV2i DqnV2i_2f(f32 x, f32 y); // Typecasts 2 floats to 2 integers
DQN_FILE_SCOPE DqnV2i DqnV2i_V2(DqnV2 a);

DQN_FILE_SCOPE DqnV2i DqnV2i_Add     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Sub     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei  (DqnV2i a, i32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef  (DqnV2i a, f32 b);
DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE f32    DqnV2i_Dot     (DqnV2i a, DqnV2i b);
DQN_FILE_SCOPE bool   DqnV2i_Equals  (DqnV2i a, DqnV2i b);

#ifdef DQN_CPP_MODE
DQN_FILE_SCOPE inline DqnV2i  operator- (DqnV2i  a, DqnV2i b) { return      DqnV2i_Sub     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator+ (DqnV2i  a, DqnV2i b) { return      DqnV2i_Add     (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, DqnV2i b) { return      DqnV2i_Hadamard(a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, f32    b) { return      DqnV2i_Scalef  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i  operator* (DqnV2i  a, i32    b) { return      DqnV2i_Scalei  (a, b);  }
DQN_FILE_SCOPE inline DqnV2i &operator*=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Hadamard(a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator-=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Sub     (a, b)); }
DQN_FILE_SCOPE inline DqnV2i &operator+=(DqnV2i &a, DqnV2i b) { return (a = DqnV2i_Add     (a, b)); }
DQN_FILE_SCOPE inline bool    operator==(DqnV2i  a, DqnV2i b) { return      DqnV2i_Equals  (a, b);  }
#endif

////////////////////////////////////////////////////////////////////////////////
// #DqnV3 Public API - 3D Math Vectors
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV3
{
	struct { f32 x, y, z; };
    DqnV2 xy;

    struct { f32 r, g, b; };
	f32 e[3];
} DqnV3;

typedef union DqnV3i
{
	struct { i32 x, y, z; };
    struct { i32 r, g, b; };
	i32 e[3];
} DqnV3i;

// DqnV3
DQN_FILE_SCOPE DqnV3 DqnV3_1f(f32 xyz);
DQN_FILE_SCOPE DqnV3 DqnV3_3f(f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnV3 DqnV3_3i(i32 x, i32 y, i32 z); // Create a vector using ints and typecast to floats

DQN_FILE_SCOPE DqnV3 DqnV3_Add     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Sub     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalei  (DqnV3 a, i32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Scalef  (DqnV3 a, f32 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_Dot     (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE bool  DqnV3_Equals  (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE DqnV3 DqnV3_Cross   (DqnV3 a, DqnV3 b);

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise    (DqnV3 a);
DQN_FILE_SCOPE f32   DqnV3_Length       (DqnV3 a, DqnV3 b);
DQN_FILE_SCOPE f32   DqnV3_LengthSquared(DqnV3 a, DqnV3 b);

#ifdef DQN_CPP_MODE
DQN_FILE_SCOPE inline DqnV3  operator- (DqnV3  a, DqnV3 b) { return      DqnV3_Sub     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, DqnV3 b) { return      DqnV3_Add     (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator+ (DqnV3  a, f32   b) { return      DqnV3_Add     (a, DqnV3_1f(b)); }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, DqnV3 b) { return      DqnV3_Hadamard(a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator* (DqnV3  a, i32   b) { return      DqnV3_Scalei  (a, b);           }
DQN_FILE_SCOPE inline DqnV3  operator/ (DqnV3  a, f32   b) { return      DqnV3_Scalef  (a, (1.0f/b));    }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Hadamard(a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, f32   b) { return (a = DqnV3_Scalef  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator*=(DqnV3 &a, i32   b) { return (a = DqnV3_Scalei  (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator-=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Sub     (a, b));          }
DQN_FILE_SCOPE inline DqnV3 &operator+=(DqnV3 &a, DqnV3 b) { return (a = DqnV3_Add     (a, b));          }
DQN_FILE_SCOPE inline bool   operator==(DqnV3  a, DqnV3 b) { return      DqnV3_Equals  (a, b);           }
#endif

// DqnV3i
DQN_FILE_SCOPE DqnV3i DqnV3i_3i(i32 x, i32 y, i32 z);
DQN_FILE_SCOPE DqnV3i DqnV3i_3f(f32 x, f32 y, f32 z);

////////////////////////////////////////////////////////////////////////////////
// #DqnV4 Public API - 4D Math Vectors
////////////////////////////////////////////////////////////////////////////////
typedef union DqnV4 {
	struct
	{
		f32 x, y, z, w;
	};
	DqnV3 xyz;
	DqnV2 xy;

	struct { f32 r, g, b, a; };
	DqnV3 rgb;

	f32    e[4];
	DqnV2 v2[2];
} DqnV4;

DQN_FILE_SCOPE DqnV4 DqnV4_1f(f32 xyzw);
DQN_FILE_SCOPE DqnV4 DqnV4_4f(f32 x, f32 y, f32 z, f32 w);
DQN_FILE_SCOPE DqnV4 DqnV4_4i(i32 x, i32 y, i32 z, f32 w); // Create a vector using ints and typecast to floats
DQN_FILE_SCOPE DqnV4 DqnV4_V3(DqnV3 a, f32 w);

DQN_FILE_SCOPE DqnV4 DqnV4_Add     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Sub     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalef  (DqnV4 a, f32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Scalei  (DqnV4 a, i32 b);
DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE f32   DqnV4_Dot     (DqnV4 a, DqnV4 b);
DQN_FILE_SCOPE bool  DqnV4_Equals  (DqnV4 a, DqnV4 b);

#ifdef DQN_CPP_MODE
DQN_FILE_SCOPE inline DqnV4  operator- (DqnV4  a, DqnV4 b) { return      DqnV4_Sub     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, DqnV4 b) { return      DqnV4_Add     (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator+ (DqnV4  a, f32   b) { return      DqnV4_Add     (a, DqnV4_1f(b));  }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, DqnV4 b) { return      DqnV4_Hadamard(a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, f32   b) { return      DqnV4_Scalef  (a, b);            }
DQN_FILE_SCOPE inline DqnV4  operator* (DqnV4  a, i32   b) { return      DqnV4_Scalei  (a, b);            }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Hadamard(a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, f32   b) { return (a = DqnV4_Scalef  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator*=(DqnV4 &a, i32   b) { return (a = DqnV4_Scalei  (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator-=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Sub     (a, b));           }
DQN_FILE_SCOPE inline DqnV4 &operator+=(DqnV4 &a, DqnV4 b) { return (a = DqnV4_Add     (a, b));           }
DQN_FILE_SCOPE inline bool   operator==(DqnV4  a, DqnV4 b) { return      DqnV4_Equals  (a, b);            }
#endif

////////////////////////////////////////////////////////////////////////////////
// #DqnMat4 Public API - 4x4 Math Matrix
////////////////////////////////////////////////////////////////////////////////
typedef union DqnMat4
{
	// TODO(doyle): Row/column instead? More cache friendly since multiplication
	// prefers rows.
	DqnV4 col[4];
	f32   e[4][4]; // Column/row
} DqnMat4;

DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity    ();

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear, f32 zFar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective (f32 fovYDegrees, f32 aspectRatio, f32 zNear, f32 zFar);
DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt      (DqnV3 eye, DqnV3 center, DqnV3 up);

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3 (DqnV3 vec);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate      (f32 radians, f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale       (f32 x, f32 y, f32 z);
DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3     (DqnV3 scale);
DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul         (DqnMat4 a, DqnMat4 b);
DQN_FILE_SCOPE DqnV4   DqnMat4_MulV4       (DqnMat4 a, DqnV4 b);

////////////////////////////////////////////////////////////////////////////////
// #DqnRect Public API - Rectangles
////////////////////////////////////////////////////////////////////////////////
typedef struct DqnRect
{
	DqnV2 min;
	DqnV2 max;
} DqnRect;

DQN_FILE_SCOPE DqnRect DqnRect_4f       (f32 minX, f32 minY, f32 maxX, f32 maxY);
DQN_FILE_SCOPE DqnRect DqnRect_4i       (i32 minX, i32 minY, i32 maxX, i32 maxY);
DQN_FILE_SCOPE DqnRect DqnRect_Init     (DqnV2 origin, DqnV2 size);

DQN_FILE_SCOPE void    DqnRect_GetSize2f(DqnRect rect, f32 *width, f32 *height);
DQN_FILE_SCOPE void    DqnRect_GetSize2i(DqnRect rect, i32 *width, i32 *height);
DQN_FILE_SCOPE DqnV2   DqnRect_GetSizeV2(DqnRect rect);
DQN_FILE_SCOPE DqnV2   DqnRect_GetCenter(DqnRect rect);
DQN_FILE_SCOPE DqnRect DqnRect_ClipRect (DqnRect rect, DqnRect clip);
DQN_FILE_SCOPE DqnRect DqnRect_Move     (DqnRect rect, DqnV2 shift);
DQN_FILE_SCOPE bool    DqnRect_ContainsP(DqnRect rect, DqnV2 p);

////////////////////////////////////////////////////////////////////////////////
// #DqnChar Public API - Char Operations
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE char DqnChar_ToLower   (char c);
DQN_FILE_SCOPE char DqnChar_ToUpper   (char c);
DQN_FILE_SCOPE bool DqnChar_IsDigit   (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlpha   (char c);
DQN_FILE_SCOPE bool DqnChar_IsAlphanum(char c);

////////////////////////////////////////////////////////////////////////////////
// #DqnStr Public API - Str Operations
////////////////////////////////////////////////////////////////////////////////
// return: 0 if equal. 0 < if a is before b, > 0 if a is after b
DQN_FILE_SCOPE i32 DqnStr_Cmp(const char *const a, const char *const b);

// return: String length not including the NULL terminator. 0 if invalid args.
DQN_FILE_SCOPE i32   DqnStr_Len           (const char *const a);

// Get the String length starting from a, up to and not including the first delimiter character.
DQN_FILE_SCOPE i32   DqnStr_LenDelimitWith(char *const a, const char delimiter);

// return: The dest argument, NULL if args invalid (i.e. NULL pointers or numChars < 0)
DQN_FILE_SCOPE char *DqnStr_Copy(char *const dest, const char *const src, const i32 numChars);

DQN_FILE_SCOPE bool DqnStr_Reverse           (char *const buf, const i32 bufSize);

// return: The offset into the src to first char of the found string. Returns -1 if not found
DQN_FILE_SCOPE i32  DqnStr_FindFirstOccurence(const char *const src, const i32 srcLen, const char *const find, const i32 findLen);
DQN_FILE_SCOPE bool DqnStr_HasSubstring      (const char *const src, const i32 srcLen, const char *const find, const i32 findLen);

#define DQN_32BIT_NUM_MAX_STR_SIZE 11
#define DQN_64BIT_NUM_MAX_STR_SIZE 21
// Return the len of the derived string. If buf is NULL and or bufSize is 0 the function returns the
// required string length for the integer
// TODO NOTE(doyle): Parsing stops when a non-digit is encountered, so numbers with ',' don't work
// atm.
DQN_FILE_SCOPE i32   Dqn_I64ToStr(const i64 value, char *const buf, const i32 bufSize);
DQN_FILE_SCOPE i64   Dqn_StrToI64(const char *const buf, const i32 bufSize);
// WARNING: Not robust, precision errors and whatnot but good enough!
DQN_FILE_SCOPE f32   Dqn_StrToF32(const char *const buf, const i32 bufSize);

// Both return the number of bytes read, return 0 if invalid codepoint or UTF8
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *const dest, const u32 character);
DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *const dest, const u32 character);

////////////////////////////////////////////////////////////////////////////////
// #DqnWChar Public API - WChar Operations
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool    DqnWChar_IsDigit(const wchar_t c);
DQN_FILE_SCOPE wchar_t DqnWChar_ToLower(const wchar_t c);

DQN_FILE_SCOPE i32 DqnWStr_Len(const wchar_t *const a);
DQN_FILE_SCOPE i32 DqnWStr_Cmp(const wchar_t *const a, const wchar_t *const b);

DQN_FILE_SCOPE bool Dqn_WStrReverse(wchar_t *buf, const i32 bufSize);
DQN_FILE_SCOPE i32  Dqn_WStrToI32(const wchar_t *const buf, const i32 bufSize);
DQN_FILE_SCOPE i32  Dqn_I32ToWStr(i32 value, wchar_t *buf, i32 bufSize);

////////////////////////////////////////////////////////////////////////////////
// #DqnRnd Public API - Random Number Generator
////////////////////////////////////////////////////////////////////////////////
// PCG (Permuted Congruential Generator) Random Number Generator
typedef struct DqnRandPCGState
{
	u64 state[2];
} DqnRandPCGState;

// pcg: Pass in a pointer to a zero-cleared DqnRandPCGState
DQN_FILE_SCOPE void DqnRnd_PCGInitWithSeed(DqnRandPCGState *pcg, u32 seed);
// Uses __rdtsc() to create a seed. TODO(doyle): This requires the platform layer.
DQN_FILE_SCOPE void DqnRnd_PCGInit(DqnRandPCGState *pcg);

// return: A random number N between [0, 0xFFFFFFFF]
DQN_FILE_SCOPE u32  DqnRnd_PCGNext (DqnRandPCGState *pcg);
// return: A random float N between [0.0, 1.0f]
DQN_FILE_SCOPE f32  DqnRnd_PCGNextf(DqnRandPCGState *pcg);
// return: A random integer N between [min, max]
DQN_FILE_SCOPE i32  DqnRnd_PCGRange(DqnRandPCGState *pcg, i32 min, i32 max);

////////////////////////////////////////////////////////////////////////////////
// #Dqn_* Public API
////////////////////////////////////////////////////////////////////////////////
typedef bool Dqn_QuickSortLessThanCallback(const void *const val1, const void *const val2);
typedef void Dqn_QuickSortSwapCallback    (void *const val1, void *const val2);
DQN_FILE_SCOPE void Dqn_QuickSortC(void *const array, const u32 itemSize, const u32 size,
                                   Dqn_QuickSortLessThanCallback *const IsLessThan,
                                   Dqn_QuickSortSwapCallback *const Swap);

template <typename T>
DQN_FILE_SCOPE void Dqn_QuickSort(T *const array, const u32 size,
                                  Dqn_QuickSortLessThanCallback *const IsLessThan)
{
	if (!array || size == 0 || size == 1 || !IsLessThan) return;

	// Insertion Sort, under 24->32 is an optimal amount
	const u32 QUICK_SORT_THRESHOLD = 24;
	if (size < QUICK_SORT_THRESHOLD)
	{
		u32 itemToInsertIndex = 1;
		while (itemToInsertIndex < size)
		{
			for (u32 checkIndex = 0; checkIndex < itemToInsertIndex; checkIndex++)
			{
				if (!IsLessThan(&array[checkIndex], &array[itemToInsertIndex]))
				{
					T itemToInsert = array[itemToInsertIndex];
					for (u32 i   = itemToInsertIndex; i > checkIndex; i--)
						array[i] = array[i - 1];

					array[checkIndex] = itemToInsert;
					break;
				}
			}
			itemToInsertIndex++;
		}

		return;
	}

	DqnRandPCGState state = {};
	DqnRnd_PCGInit(&state);

	u32 pivotIndex     = DqnRnd_PCGRange(&state, 0, size - 1);
	u32 partitionIndex = 0;
	u32 startIndex     = 0;

	// Swap pivot with last index, so pivot is always at the end of the array.
	// This makes logic much simpler.
	{
		u32 lastIndex = size - 1;
		DQN_SWAP(T, array[lastIndex], array[pivotIndex]);
		pivotIndex = lastIndex;
	}

	// 4^, 8, 7, 5, 2, 3, 6
	if (IsLessThan(&array[startIndex], &array[pivotIndex])) partitionIndex++;
	startIndex++;

	// 4, |8, 7, 5^, 2, 3, 6*
	// 4, 5, |7, 8, 2^, 3, 6*
	// 4, 5, 2, |8, 7, ^3, 6*
	// 4, 5, 2, 3, |7, 8, ^6*
	for (u32 checkIndex = startIndex; checkIndex < size; checkIndex++)
	{
		if (IsLessThan(&array[checkIndex], &array[pivotIndex]))
		{
			DQN_SWAP(T, array[partitionIndex], array[checkIndex]);
			partitionIndex++;
		}
	}

	// Move pivot to right of partition
	// 4, 5, 2, 3, |6, 8, ^7*
	DQN_SWAP(T, array[partitionIndex], array[pivotIndex]);
	Dqn_QuickSort(array, partitionIndex, IsLessThan);

	// Skip the value at partion index since that is guaranteed to be sorted.
	// 4, 5, 2, 3, (x), 8, 7
	u32 oneAfterPartitionIndex = partitionIndex + 1;
	Dqn_QuickSort(array + oneAfterPartitionIndex, (size - oneAfterPartitionIndex), IsLessThan);
}

#endif  /* DQN_H */

////////////////////////////////////////////////////////////////////////////////
// #XPlatform (Win32 & Unix) Public API
////////////////////////////////////////////////////////////////////////////////
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

// NOTE(doyle): DQN_PLATFORM_HEADER is enabled by the user to have the function prototypes be
// visible. DQN_PLATFORM_H is like a normal header guard that ensures singular declaration of
// functions.
#ifdef DQN_PLATFORM_HEADER

#ifndef DQN_PLATFORM_H
#define DQN_PLATFORM_H

#if defined(DQN_IS_WIN32)
	#define WIN32_LEAN_AND_MEAN 1
	#include <Windows.h>

#elif defined(DQN_IS_UNIX)
	#include <pthread.h>
	#include <semaphore.h>

#endif
////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnFile Public API - File I/O
////////////////////////////////////////////////////////////////////////////////
enum DqnFilePermissionFlag
{
	DqnFilePermissionFlag_Read    = (1 << 0),
	DqnFilePermissionFlag_Write   = (1 << 1),
	DqnFilePermissionFlag_Execute = (1 << 2),
	DqnFilePermissionFlag_All     = (1 << 3)
};

enum DqnFileAction
{
	// Only open file if it exists. Fails and returns false if file did not
	// exist or could not open.
	DqnFileAction_OpenOnly,

	// Try and create file. Return true if it was able to create. If it already
	// exists, this will fail.
	DqnFileAction_CreateIfNotExist,

	// Clear the file contents to zero if it exists. Fails and returns false if
	// file does not exist.
	DqnFileAction_ClearIfExist,
};

typedef struct DqnFile
{
	u32     permissionFlags;
	void   *handle;
	size_t  size;

#if defined(DQN_CPP_MODE)
	// If raiiCleanup is true, close() is called in the destructor on scope exit. Can be changed at
	// any point by user.
	bool    raiiCleanup;
	DqnFile (const bool raiiCleanup = false);
	~DqnFile();

	bool   Open (const char    *const path, const u32 permissionFlags_, const enum DqnFileAction action);
	bool   OpenW(const wchar_t *const path, const u32 permissionFlags_, const enum DqnFileAction action);
	size_t Write(u8 *const buffer, const size_t numBytesToWrite, const size_t fileOffset);
	size_t Read (u8 *const buffer, const size_t numBytesToRead);
	void   Close();
#endif
} DqnFile;

// NOTE: W(ide) versions of functions only work on Win32, since Unix is already UTF-8 compatible.

// Open a handle for file read and writing. Deleting files does not need a handle. Handles should be
// closed before deleting files otherwise the OS may not be able to delete the file.
// return: FALSE if invalid args or failed to get handle (i.e. insufficient permissions)
DQN_FILE_SCOPE bool DqnFile_Open (const char *const path, DqnFile *const file, const u32 permissionFlags, const enum DqnFileAction action);
DQN_FILE_SCOPE bool DqnFile_OpenW(const wchar_t *const path, DqnFile *const file, const u32 permissionFlags, const enum DqnFileAction action);

// fileOffset: The byte offset to starting writing from.
// return:     The number of bytes written. 0 if invalid args or it failed to write.
DQN_FILE_SCOPE size_t DqnFile_Write(const DqnFile *const file, u8 *const buffer, const size_t numBytesToWrite, const size_t fileOffset);

// return: The number of bytes read. 0 if invalid args or it failed to read.
DQN_FILE_SCOPE size_t DqnFile_Read (const DqnFile *const file, u8 *const buffer, const size_t numBytesToRead);

// Read the entire file at path into the given buffer. To determine the necessary buffer size, use
// DqnFile_GetFileSize()
// buffer:     The buffer to put data into.
// bufferSize: The size of the given buffer.
// bytesRead:  (Optionally) Pass in a pointer to a size_t to get how many bytes of the buffer was
//             used. Will be valid only if function returns true. This is basically the return value
//             of DqnFile_Read()
// return:     FALSE if insufficient bufferSize OR file access failure OR invalid args (i.e nullptr)
DQN_FILE_SCOPE bool DqnFile_ReadEntireFile(const char *const path, u8 *const buffer,
                                           const size_t bufferSize, size_t *const bytesRead);

// File close invalidates the handle after it is called.
DQN_FILE_SCOPE void DqnFile_Close(DqnFile *const file);

// size: Pass in a pointer to a size_t where the file size gets put into. It holds invalid data if
// function returns false.
DQN_FILE_SCOPE bool DqnFile_GetFileSize (const char *const path, size_t *const size);
DQN_FILE_SCOPE bool DqnFile_GetFileSizeW(const wchar_t *const path, size_t *const size);

// NOTE: You can't delete a file unless the handle has been closed to it on Win32.
DQN_FILE_SCOPE bool DqnFile_Delete (const char *const path);
DQN_FILE_SCOPE bool DqnFile_DeleteW(const wchar_t *const path);

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnDir Public API - Directory Querying
////////////////////////////////////////////////////////////////////////////////
// numFiles: Pass in a pointer to a u32. The function fills it out with the number of entries.
// return:   An array of strings of the files in the directory in UTF-8. The directory lisiting is
//           allocated with malloc and must be freed using free() or the helper function DqnDir_ReadFree()
DQN_FILE_SCOPE char **DqnDir_Read    (const char *const dir, u32 *const numFiles);
DQN_FILE_SCOPE void   DqnDir_ReadFree(char **fileList, u32 numFiles);

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnTimer Public API - High Resolution Timer
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f64  DqnTimer_NowInMs();
DQN_FILE_SCOPE f64  DqnTimer_NowInS ();

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnLock Public API - Mutex Synchronisation
////////////////////////////////////////////////////////////////////////////////
typedef struct DqnLock
{
#if defined(DQN_IS_WIN32)
	CRITICAL_SECTION win32Handle;

#elif defined(DQN_IS_UNIX)
	pthread_mutex_t unixHandle;

#else
	#error Unknown platform

#endif

	// Win32 only, when trying to acquire a locked lock, it is the number of spins permitted
	// spinlocking on the lock before being blocked. Set before init if you want a different value.
	u32 win32SpinCount = 16000;

#if defined(DQN_CPP_MODE)
	bool Init   ();
	void Acquire();
	void Release();
	void Delete ();

	// Create a lock guard on the lock this is invoked on.
	struct DqnLockGuard LockGuard();
#endif
} DqnLock;

#if defined(DQN_CPP_MODE)
// Lock guard automatically acquires a lock on construction and releases the associated lock on
// destruction. If the lock is unable to be acquired, the program blocks at construction until it
// can.
struct DqnLockGuard
{
	// lock: Takes a pointer to a pre-existing and already initialised lock
	// bool: Pass in (optionally) a pointer to a bool which returns whether a lock was successful.
	// FALSE if lock is NULL.
	 DqnLockGuard(DqnLock *const lock_, bool *const succeeded);
	~DqnLockGuard();

private:
	DqnLock *lock;
};
#endif

// lock: Pass in a pointer to a default DqnLock struct.
//       In Win32, you may optionally change the win32Spincount.
DQN_FILE_SCOPE bool DqnLock_Init   (DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Acquire(DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Release(DqnLock *const lock);
DQN_FILE_SCOPE void DqnLock_Delete (DqnLock *const lock);

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnJobQueue Public API - Multithreaded Job Queue
////////////////////////////////////////////////////////////////////////////////
// DqnJobQueue is a platform abstracted "lockless" multithreaded work queue. It will create threads
// and assign threads to complete the job via the job "callback" using the "userData" supplied.

// Usage
// 1. Prepare your callback function for threads to execute following the 'DqnJob_Callback' function
//    signature.
// 2. Create a job queue with DqnJobQueue_InitWithMem()
// 3. Add jobs with DqnJobQueue_AddJob() and threads will be dispatched automatically.

// When all jobs are sent you can also utilise the main executing thread to complete jobs whilst you
// wait for all jobs to complete using DqnJobQueue_TryExecuteNextJob() or spinlock on
// DqnJobQueue_AllJobsComplete(). Alternatively you can combine both for the main thread to help
// complete work and not move on until all tasks are complete.

typedef struct DqnJobQueue DqnJobQueue;

typedef void   DqnJob_Callback(DqnJobQueue *const queue, void *const userData);
typedef struct DqnJob
{
	DqnJob_Callback *callback;
	void            *userData;
} DqnJob;

typedef struct DqnJobQueue
{
	// JobList Circular Array, is setup in Init()
	DqnJob *jobList;
	u32     size;

	// NOTE(doyle): Modified by main+worker threads
	i32   volatile jobToExecuteIndex;
	i32   volatile numJobsToComplete;

#if defined(DQN_IS_WIN32)
	void *semaphore;

#elif defined(DQN_IS_UNIX)
	sem_t semaphore;

#else
	#error Unknown platform

#endif

	// NOTE: Modified by main thread ONLY
	i32 volatile jobInsertIndex;

#if defined(DQN_CPP_MODE)
	bool Init             (DqnJob *const jobList_, const u32 jobListSize, const u32 numThreads);
	bool AddJob           (const DqnJob job);

    void BlockAndCompleteAllJobs();
	bool TryExecuteNextJob();
	bool AllJobsComplete  ();
#endif
} DqnJobQueue;

// TODO(doyle): Queue delete, thread delete

// queue:       Pass a pointer to a zero cleared DqnJobQueue struct
// jobList:     Pass in a pointer to an array of DqnJob's
// jobListSize: The number of elements in the jobList array
// numThreads:  The number of threads the queue should request from the OS for working on the queue
// return:      FALSE if invalid args i.e. NULL ptrs or jobListSize & numThreads == 0
DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *const queue, const DqnJob *const jobList,
                                     const u32 jobListSize, const u32 numThreads);

// return: FALSE if the job is not able to be added, this occurs if the queue is full.
DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *const queue, const DqnJob job);

// Helper function that combines TryExecuteNextJob() and AllJobsComplete(), i.e.
// complete all work before moving on. Does nothing if queue is NULL.
DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *const queue);

// return: TRUE if there was a job to execute (the calling thread executes it). FALSE if it could
//         not get a job. It may return FALSE whilst there are still jobs, this means that another thread
//         has taken the job before the calling thread could and should NOT be used to determine if there
//         are any remaining jobs left. That can only be definitively known using
//         DqnJobQueue_AllJobsComplete(). This is typically combined like so ..
//         while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue));
//         Return FALSE also if queue is a NULL pointer.
DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *const queue);
DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete  (DqnJobQueue *const queue);

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnAtomic Public API - Interlocks/Atomic Operations
////////////////////////////////////////////////////////////////////////////////
// All atomic operations generate a full read/write barrier. This is implicitly enforced by the
// OS calls, not explicitly in my code.

// swapVal:    The value to put into "dest", IF at point of read, "dest" has the value of "compareVal"
// compareVal: The value to check in "dest"
// return:     Return the original value that was in "dest"
DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *const dest, const i32 swapVal, const i32 compareVal);

// Add "value" to src
// return: The new value at src
DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *const src, const i32 value);

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnPlatform Public API - Common Platform API Helpers
////////////////////////////////////////////////////////////////////////////////
// Uses a single call to DqnMem_Calloc() and DqnMem_Free(). Not completely platform "independent" for Unix.
// numCores: numThreadsPerCore: Can be NULL, the function will just skip it.
DQN_FILE_SCOPE void DqnPlatform_GetNumThreadsAndCores(u32 *const numCores, u32 *const numThreadsPerCore);

////////////////////////////////////////////////////////////////////////////////
// #Platform Public API
////////////////////////////////////////////////////////////////////////////////
// Functions here are only available for the #defined sections (i.e. all functions in
// DQN_WIN32_PLATFORM only have a valid implementation in Win32.

////////////////////////////////////////////////////////////////////////////////
// #Win32Platform Public API
////////////////////////////////////////////////////////////////////////////////
#if defined(DQN_IS_WIN32)
////////////////////////////////////////////////////////////////////////////////
// Platform > #DqnWin32 Public API - Common Win32 API Helpers
////////////////////////////////////////////////////////////////////////////////
#define DQN_WIN32_ERROR_BOX(text, title) MessageBoxA(NULL, text, title, MB_OK);

// out:    A pointer to the buffer to receive the characters.
// outLen: The length/capacity of the buffer "out"
DQN_FILE_SCOPE bool DqnWin32_UTF8ToWChar(const char    *const in, wchar_t *const out, const i32 outLen);
DQN_FILE_SCOPE bool DqnWin32_WCharToUTF8(const wchar_t *const in, char    *const out, const i32 outLen);

// "width" and "height" are optional and won't be used if not given by user.
// width & height: Pass in a pointer for function to fill out.
DQN_FILE_SCOPE void DqnWin32_GetClientDim     (const HWND window, LONG *const width, LONG *const height);
DQN_FILE_SCOPE void DqnWin32_GetRectDim       (const RECT rect,   LONG *const width, LONG *const height);

// Displays error in the format <errorPrefix>: <Win32 Error Message> in a Win32 Dialog Box.
// errorPrefix: The message before the Win32 error, can be NULL
DQN_FILE_SCOPE void DqnWin32_DisplayLastError (const char *const errorPrefix);

// Asimilar to DqnWin32_DisplayLastError() a particular error can be specified in a Win32 Dialog Box.
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode (const DWORD error, const char *const errorPrefix);

// Output text to the debug console. For visual studio this is the output window and not the console.
// ...: Variable args alike printf, powered by stb_sprintf
DQN_FILE_SCOPE void DqnWin32_OutputDebugString(const char *const formatStr, ...);

// Get the full path of to the current processes executable, and return the char offset in the
// string to the last backslash, i.e. the directory.
// buf:    Filled with the path to the executable file.
// return: The offset to the last backslash. -1 if bufLen was not large enough or buf is null.
DQN_FILE_SCOPE i32  DqnWin32_GetEXEDirectory(char *const buf, const u32 bufLen);
#endif // DQN_IS_WIN32
#endif // DQN_PLATFORM_H
#endif // DQN_PLATFORM_HEADER


////////////////////////////////////////////////////////////////////////////////
// #External Code
////////////////////////////////////////////////////////////////////////////////
#ifndef DQN_INI_H
#define DQN_INI_H
////////////////////////////////////////////////////////////////////////////////
// #DqnIni Public API - Simple INI Config File API v1.1
////////////////////////////////////////////////////////////////////////////////
/*
TODO(doyle): Make my own for fun?
Public Domain library with thanks to Mattias Gustavsson
https://github.com/mattiasgustavsson/libs/blob/master/docs/ini.md

Examples
Loading an ini file and retrieving values

#define DQN_INI_IMPLEMENTATION
#include "ini.h"
#include <stdio.h>
#include <stdlib.h>

int main()
{
	FILE *fp = fopen("test.ini", "r");
	fseek(fp, 0, SEEK_END);
	int size = ftell(fp);
	fseek(fp, 0, SEEK_SET);
	char *data = (char *)malloc(size + 1);
	fread(data, 1, size, fp);
	data[size] = '\0';
	fclose(fp);

	DqnIni *ini = DqnIni_Load(data);
	free(data);
	int second_index   = DqnIni_FindProperty(ini, DQN_INI_GLOBAL_SECTION, "SecondSetting");
	char const *second = DqnIni_PropertyValue(ini, DQN_INI_GLOBAL_SECTION, second_index);
	int section        = DqnIni_FindSection(ini, "MySection");
	int third_index    = DqnIni_FindProperty(ini, section, "ThirdSetting");
	char const *third  = DqnIni_PropertyValue(ini, section, third_index);
	DqnIni_Destroy(ini);

	return 0;
}

Creating a new ini file
#define DQN_INI_IMPLEMENTATION
#include "ini.h"

#include <stdio.h>
#include <stdlib.h>

int main()
{
	DqnIni *ini = DqnInit_Create();
	DqnIni_PropertyAdd(ini, DQN_INI_GLOBAL_SECTION, "FirstSetting", "Test");
	DqnIni_PropertyAdd(ini, DQN_INI_GLOBAL_SECTION, "SecondSetting", "2");
	int section = DqnIni_SectionAdd(ini, "MySection");
	DqnIni_PropertyAdd(ini, section, "ThirdSetting", "Three");

	int size   = DqnIni_Save(ini, NULL, 0); // Find the size needed
	char *data = (char *)malloc(size);
	size       = DqnIni_Save(ini, data, size); // Actually save the file
	DqnIni_Destroy(ini);

	FILE *fp = fopen("test.ini", "w");
	fwrite(data, 1, size, fp);
	fclose(fp);
	free(data);

	return 0;
}
*/

#define DQN_INI_GLOBAL_SECTION (0)
#define DQN_INI_NOT_FOUND (-1)

typedef struct DqnIni DqnIni;

DqnIni *DqnInit_Create(void *memctx);
DqnIni *DqnIni_Load  (char const *data, void *memctx);

int  DqnIni_Save   (DqnIni const *ini, char *data, int size);
void DqnIni_Destroy(DqnIni *ini);

int         DqnIni_SectionCount(DqnIni const *ini);
char const *DqnIni_SectionName (DqnIni const *ini, int section);

int         DqnIni_PropertyCount(DqnIni const *ini, int section);
char const *DqnIni_PropertyName (DqnIni const *ini, int section, int property);
char const *DqnIni_PropertyValue(DqnIni const *ini, int section, int property);

int DqnIni_FindSection (DqnIni const *ini, char const *name, int name_length);
int DqnIni_FindProperty(DqnIni const *ini, int section, char const *name, int name_length);

int  DqnIni_SectionAdd    (DqnIni *ini, char const *name, int length);
void DqnIni_PropertyAdd   (DqnIni *ini, int section, char const *name, int name_length, char const *value, int value_length);
void DqnIni_SectionRemove (DqnIni *ini, int section);
void DqnIni_PropertyRemove(DqnIni *ini, int section, int property);

void DqnIni_SectionNameSet  (DqnIni *ini, int section, char const *name, int length);
void DqnIni_PropertyNameSet (DqnIni *ini, int section, int property, char const *name, int length);
void DqnIni_PropertyValueSet(DqnIni *ini, int section, int property, char const *value, int length);

/**
Customization
-------------
### Custom memory allocators

To store the internal data structures, ini.h needs to do dynamic allocation by
calling `malloc`. Programs might want to keep track of allocations done, or use
custom defined pools to allocate memory from. ini.h allows for specifying custom
memory allocation functions for `malloc` and `free`.  This is done with the
following code:

    #define DQN_INI_IMPLEMENTATION
    #define DQN_INI_MALLOC( ctx, size ) ( my_custom_malloc( ctx, size ) )
    #define DQN_INI_FREE( ctx, ptr ) ( my_custom_free( ctx, ptr ) )
    #include "ini.h"

where `my_custom_malloc` and `my_custom_free` are your own memory
allocation/deallocation functions. The `ctx` parameter is an optional parameter
of type `void*`. When `DqnInit_Create` or `DqnIni_Load` is called, you can pass
in a `memctx` parameter, which can be a pointer to anything you like, and which
will be passed through as the `ctx` parameter to every
`DQN_INI_MALLOC`/`DQN_INI_FREE` call. For example, if you are doing memory
tracking, you can pass a pointer to your tracking data as `memctx`, and in your
custom allocation/deallocation function, you can cast the `ctx` param back to
the right type, and access the tracking data.

If no custom allocator is defined, ini.h will default to `malloc` and `free`
from the C runtime library.

### Custom C runtime function
The library makes use of three additional functions from the C runtime library,
and for full flexibility, it allows you to substitute them for your own.  Here's
an example:

    #define DQN_INI_IMPLEMENTATION
    #define DQN_INI_MEMCPY( dst, src, cnt ) ( my_memcpy_func( dst, src, cnt ) )
    #define DQN_INI_STRLEN( s ) ( my_strlen_func( s ) )
    #define DQN_INI_STRICMP( s1, s2 ) ( my_stricmp_func( s1, s2 ) )
    #include "ini.h"

If no custom function is defined, ini.h will default to the C runtime library equivalent.


DqnInit_Create
----------

    DqnIni* DqnInit_Create( void* memctx )

Instantiates a new, empty ini structure, which can be manipulated with other API
calls, to fill it with data. To save it out to an ini-file string, use
`DqnIni_Save`. When no longer needed, it can be destroyed by calling
`DqnIni_Destroy`.  `memctx` is a pointer to user defined data which will be
passed through to the custom DQN_INI_MALLOC/DQN_INI_FREE calls. It can be NULL
if no user defined data is needed.


DqnIni_Load
--------

    DqnIni* DqnIni_Load( char const* data, void* memctx )

Parse the zero-terminated string `data` containing an ini-file, and create a new
DqnIni instance containing the data.  The instance can be manipulated with
other API calls to enumerate sections/properties and retrieve values. When no
longer needed, it can be destroyed by calling `DqnIni_Destroy`. `memctx` is
a pointer to user defined data which will be passed through to the custom
DQN_INI_MALLOC/DQN_INI_FREE calls. It can be NULL if no user defined data is
needed.


DqnIni_Save
--------

    int DqnIni_Save( DqnIni const* ini, char* data, int size )

Saves an ini structure as a zero-terminated ini-file string, into the specified
buffer. Returns the number of bytes written, including the zero terminator. If
`data` is NULL, nothing is written, but `DqnIni_Save` still returns the number
of bytes it would have written. If the size of `data`, as specified in the
`size` parameter, is smaller than that required, only part of the ini-file
string will be written. `DqnIni_Save` still returns the number of bytes it
would have written had the buffer been large enough.

DqnIni_Destroy
-----------

    void DqnIni_Destroy( DqnIni* ini )

Destroy an `DqnIni` instance created by calling `DqnIni_Load` or
`DqnInit_Create`, releasing the memory allocated by it. No further API calls are
valid on an `DqnIni` instance after calling `DqnIni_Destroy` on it.


DqnIni_SectionCount
-----------------

    int DqnIni_SectionCount( DqnIni const* ini )

Returns the number of sections in an ini file. There's at least one section in
an ini file (the global section), but there can be many more, each specified in
the file by the section name wrapped in square brackets [ ].


DqnIni_SectionName
----------------

    char const* DqnIni_SectionName( DqnIni const* ini, int section )

Returns the name of the section with the specified index. `section` must be
non-negative and less than the value returned by `DqnIni_SectionCount`, or
`DqnIni_SectionName` will return NULL. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.


DqnIni_PropertyCount
------------------

    int DqnIni_PropertyCount( DqnIni const* ini, int section )

Returns the number of properties belonging to the section with the specified
index. `section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`, or `DqnIni_SectionName` will return 0. The defined
constant `DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.
Properties are declared in the ini-file on he format `name=value`.


DqnIni_PropertyName
-----------------

    char const* DqnIni_PropertyName( DqnIni const* ini, int section, int property )

Returns the name of the property with the specified index `property` in the
section with the specified index `section`.  `section` must be non-negative and
less than the value returned by `DqnIni_SectionCount`, and `property` must be
non-negative and less than the value returned by `DqnIni_PropertyCount`, or
`DqnIni_PropertyName` will return NULL. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.


DqnIni_PropertyValue
------------------

    char const* DqnIni_PropertyValue( DqnIni const* ini, int section, int property )

Returns the value of the property with the specified index `property` in the
section with the specified index `section`.  `section` must be non-negative and
less than the value returned by `DqnIni_SectionCount`, and `property` must be
non-negative and less than the value returned by `DqnIni_PropertyCount`, or
`DqnIni_PropertyValue` will return NULL. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.


DqnIni_FindSection
----------------

    int DqnIni_FindSection( DqnIni const* ini, char const* name, int name_length )

Finds the section with the specified name, and returns its index. `name_length`
specifies the number of characters in `name`, which does not have to be
zero-terminated. If `name_length` is zero, the length is determined
automatically, but in this case `name` has to be zero-terminated. If no section
with the specified name could be found, the value `DQN_INI_NOT_FOUND` is
returned.


DqnIni_FindProperty
-----------------

    int DqnIni_FindProperty( DqnIni const* ini, int section, char const* name, int name_length )

Finds the property with the specified name, within the section with the
specified index, and returns the index of the property. `name_length` specifies
the number of characters in `name`, which does not have to be zero-terminated.
If `name_length` is zero, the length is determined automatically, but in this
case `name` has to be zero-terminated. If no property with the specified name
could be found within the specified section, the value `DQN_INI_NOT_FOUND` is
returned.  `section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`, or `DqnIni_FindProperty` will return
`DQN_INI_NOT_FOUND`. The defined constant `DQN_INI_GLOBAL_SECTION` can be used
to indicate the global section.


DqnIni_SectionAdd
---------------

    int DqnIni_SectionAdd( DqnIni* ini, char const* name, int length )

Adds a section with the specified name, and returns the index it was added at.
There is no check done to see if a section with the specified name already
exists - multiple sections of the same name are allowed. `length` specifies the
number of characters in `name`, which does not have to be zero-terminated. If
`length` is zero, the length is determined automatically, but in this case
`name` has to be zero-terminated.


DqnIni_PropertyAdd
----------------
    
    void DqnIni_PropertyAdd( DqnIni* ini, int section, char const* name, int name_length, char const* value, int value_length )

Adds a property with the specified name and value to the specified section, and returns the index it was added at. There is no check done to see if a property
with the specified name already exists - multiple properties of the same name
are allowed. `name_length` and `value_length` specifies the number of characters
in `name` and `value`, which does not have to be zero-terminated. If
`name_length` or `value_length` is zero, the length is determined automatically,
but in this case `name`/`value` has to be zero-terminated. `section` must be
non-negative and less than the value returned by `DqnIni_SectionCount`, or the
property will not be added. The defined constant `DQN_INI_GLOBAL_SECTION` can be
used to indicate the global section.


DqnIni_SectionRemove
------------------

    void DqnIni_SectionRemove( DqnIni* ini, int section )

Removes the section with the specified index, and all properties within it.
`section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`. The defined constant `DQN_INI_GLOBAL_SECTION` can be
used to indicate the global section. Note that removing a section will shuffle
section indices, so that section indices you may have stored will no longer
indicate the same section as it did before the remove. Use the find functions to
update your indices.


DqnIni_PropertyRemove
-------------------

    void DqnIni_PropertyRemove( DqnIni* ini, int section, int property )

Removes the property with the specified index from the specified section.
`section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`, and `property` must be non-negative and less than the
value returned by `DqnIni_PropertyCount`. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section. Note that
removing a property will shuffle property indices within the specified section,
so that property indices you may have stored will no longer indicate the same
property as it did before the remove. Use the find functions to update your
indices.


DqnIni_SectionNameSet
--------------------

    void DqnIni_SectionNameSet( DqnIni* ini, int section, char const* name, int length )

Change the name of the section with the specified index. `section` must be
non-negative and less than the value returned by `DqnIni_SectionCount`. The
defined constant `DQN_INI_GLOBAL_SECTION` can be used to indicate the global
section. `length` specifies the number of characters in `name`, which does not
have to be zero-terminated. If `length` is zero, the length is determined
automatically, but in this case `name` has to be zero-terminated.


DqnIni_PropertyNameSet
---------------------

    void DqnIni_PropertyNameSet( DqnIni* ini, int section, int property, char const* name, int length )

Change the name of the property with the specified index in the specified
section. `section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`, and `property` must be non-negative and less than the
value returned by `DqnIni_PropertyCount`. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.  `length`
specifies the number of characters in `name`, which does not have to be
zero-terminated. If `length` is zero, the length is determined automatically,
	but in this case `name` has to be zero-terminated.


DqnIni_PropertyValueSet
----------------------

    void DqnIni_PropertyValueSet( DqnIni* ini, int section, int property, char const* value, int length  )

Change the value of the property with the specified index in the specified
section. `section` must be non-negative and less than the value returned by
`DqnIni_SectionCount`, and `property` must be non-negative and less than the
value returned by `DqnIni_PropertyCount`. The defined constant
`DQN_INI_GLOBAL_SECTION` can be used to indicate the global section.  `length`
specifies the number of characters in `value`, which does not have to be
zero-terminated. If `length` is zero, the length is determined automatically,
	but in this case `value` has to be zero-terminated.

**/
#endif // DQN_INI_H

#ifndef STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_H_INCLUDE
#define STB_SPRINTF_DECORATE(name) Dqn_##name

////////////////////////////////////////////////////////////////////////////////
// #DqnSprintf Public API - Cross-platform Sprintf Implementation
////////////////////////////////////////////////////////////////////////////////
/*
Public Domain library originally written by Jeff Roberts at RAD Game Tools
- 2015/10/20.  Hereby placed in public domain.
STB_Sprintf renamed to Dqn_Sprintf

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

#ifndef STB_SPRINTF_DECORATE
	#define STB_SPRINTF_DECORATE(name) stbsp_##name  // define this before including if you want to change the names
#endif

// Convert a va_list arg list into a buffer. vsnprintf always returns a zero-terminated string (unlike regular snprintf).
// return: The number of characters copied into the buffer
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf) (char *buf, char const *fmt, va_list va);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsnprintf)(char *buf, int count, char const *fmt, va_list va);

// Convert an arg list into a buffer. snprintf() always returns a zero-terminated string (unlike regular snprintf).
// return: The number of characters copied into the buffer
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf) (char *buf, char const *fmt, ...);
STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...);

// Convert into a buffer, calling back every STB_SPRINTF_MIN chars.
// Your callback can then copy the chars out, print them or whatever.
// This function is actually the workhorse for everything else.
// The buffer you pass in must hold at least STB_SPRINTF_MIN characters.
// You return the next buffer to use or 0 to stop converting
typedef char *STBSP_SPRINTFCB(char *buf, void *user, int len);
STBSP__PUBLICDEF int  STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB* callback, void *user, char *buf, char const *fmt, va_list va);

// Set the comma and period characters to use.
STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char comma, char period);

#endif // STB_SPRINTF_H_INCLUDE

#ifdef DQN_IMPLEMENTATION
////////////////////////////////////////////////////////////////////////////////
//
// DQN_IMPLEMENTATION
//
////////////////////////////////////////////////////////////////////////////////
#include <math.h>    // TODO(doyle): For trigonometry functions (for now)
#include <stdlib.h>  // For calloc, malloc, free
#include <stdio.h>   // For printf

// NOTE: STB_SPRINTF is included when DQN_IMPLEMENTATION defined
// #define STB_SPRINTF_IMPLEMENTATION

// NOTE: DQN_INI_IMPLEMENTATION modified to be included when DQN_IMPLEMENTATION defined
// #define DQN_INI_IMPLEMENTATION
#define DQN_INI_STRLEN(s) DqnStr_Len(s)

////////////////////////////////////////////////////////////////////////////////
// #DqnAssert Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnAssertInternal(const bool result, const char *const file, const i32 lineNum,
                                      const char *const expr, const char *const msg, ...)
{
	if (!(result))
	{
		const char *const formatStrNoMsg   = "DqnAssert() failed: %s|%d| (%s)\n";
		const char *const formatStrWithMsg = "DqnAssert() failed: %s|%d| (%s): %s\n";
		const char *const formatStr        = (msg) ? formatStrWithMsg : formatStrNoMsg;

		char userMsg[2048] = {};
		if (msg)
		{
			va_list argList;
			va_start(argList, msg);
			{
				u32 numCopied = Dqn_vsprintf(userMsg, msg, argList);
				DQN_ASSERT_HARD(numCopied < DQN_ARRAY_COUNT(userMsg));
			}
			va_end(argList);
		}

#ifdef DQN_WIN32_PLATFORM
		DqnWin32_OutputDebugString(formatStr, file, lineNum, expr, userMsg);
#else
		printf(formatStr, file, lineNum, expr, userMsg);
#endif

		(*((i32 *)0)) = 0;
	}
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemory Implementation
////////////////////////////////////////////////////////////////////////////////
// NOTE: All memory allocations in dqn.h go through these functions. So they can
// be rerouted fairly easily especially for platform specific mallocs.
DQN_FILE_SCOPE void *DqnMem_Alloc(const size_t size)
{
	void *result = malloc(size);
	return result;
}

DQN_FILE_SCOPE void *DqnMem_Calloc(const size_t size)
{
	void *result = calloc(1, size);
	return result;
}

DQN_FILE_SCOPE void DqnMem_Clear(void *const memory, const u8 clearValue, const size_t size)
{
	if (memory) memset(memory, clearValue, size);
}

DQN_FILE_SCOPE void *DqnMem_Realloc(void *memory, const size_t newSize)
{
	void *result = realloc(memory, newSize);
	return result;
}

DQN_FILE_SCOPE void DqnMem_Free(void *memory)
{
	if (memory) free(memory);
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStackInternal Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnMemStackBlock *
DqnMemStackInternal_AllocateBlock(u32 byteAlign, size_t size, const bool zeroClear)
{
	size_t alignedSize = DQN_ALIGN_POW_N(size, byteAlign);
	size_t totalSize   = alignedSize + sizeof(DqnMemStackBlock) + (byteAlign -1);

	// NOTE(doyle): Total size includes another (byteAlign-1) since we also want
	// to align the base pointer to memory that we receive.
	DqnMemStackBlock *result = NULL;
	if (zeroClear)
		result = (DqnMemStackBlock *)DqnMem_Calloc(totalSize);
	else
		result = (DqnMemStackBlock *)DqnMem_Alloc(totalSize);

	if (!result) return NULL;

	result->memory    = (u8 *)DQN_ALIGN_POW_N((u8 *)result + sizeof(*result), byteAlign);
	result->size      = alignedSize;
	result->used      = 0;
	result->prevBlock = NULL;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStack CPP Implementation
////////////////////////////////////////////////////////////////////////////////
#if defined(DQN_CPP_MODE)
bool DqnMemStack::InitWithFixedMem (u8 *const mem,     const size_t memSize, const u32 byteAlignment) { return DqnMemStack_InitWithFixedMem (this, mem, memSize, byteAlignment);    }
bool DqnMemStack::InitWithFixedSize(const size_t size, const bool zeroClear, const u32 byteAlignment) { return DqnMemStack_InitWithFixedSize(this, size, zeroClear, byteAlignment); }
bool DqnMemStack::Init             (const size_t size, const bool zeroClear, const u32 byteAlignment) { return DqnMemStack_Init             (this, size, zeroClear, byteAlignment); }

void *DqnMemStack::Push(size_t size)                        { return DqnMemStack_Push(this, size);                }
void  DqnMemStack::Pop (void *const ptr, size_t size)       {        DqnMemStack_Pop (this, ptr, size);           }
void  DqnMemStack::Free()                                   {        DqnMemStack_Free(this);                      }
bool  DqnMemStack::FreeMemBlock(DqnMemStackBlock *memBlock) { return DqnMemStack_FreeMemBlock(this, memBlock);       }
bool  DqnMemStack::FreeLastBlock()                          { return DqnMemStack_FreeLastBlock(this);             }
void  DqnMemStack::ClearCurrBlock(const bool zeroClear)     {        DqnMemStack_ClearCurrBlock(this, zeroClear); }

DqnMemStackTempRegion DqnMemStack::TempRegionBegin()
{
	// NOTE: Should always succeed since the stack is guaranteed to exist.
	DqnMemStackTempRegion result = {};
	bool succeeded               = DqnMemStackTempRegion_Begin(&result, this);
	DQN_ASSERT_HARD(succeeded);
	return result;
}
void DqnMemStack::TempRegionEnd(DqnMemStackTempRegion region) { DqnMemStackTempRegion_End(region); }

// NOTE: Guaranteed to always succeed. Fails when arguments are invalid, like a NULL ptr which is impossible here.
DqnMemStackTempRegionGuard DqnMemStack::TempRegionGuard() { return DqnMemStackTempRegionGuard(this, NULL); }

DqnMemStackBlock *DqnMemStack::AllocateCompatibleBlock(size_t size, const bool zeroClear)    { return DqnMemStack_AllocateCompatibleBlock(this, size, zeroClear); }
bool              DqnMemStack::AttachBlock            (DqnMemStackBlock *const newBlock)     { return DqnMemStack_AttachBlock            (this, newBlock);        }
bool              DqnMemStack::DetachBlock            (DqnMemStackBlock *const detachBlock)  { return DqnMemStack_DetachBlock            (this, detachBlock);     }
void              DqnMemStack::FreeDetachedBlock      (DqnMemStackBlock *memBlock)           {        DqnMemStack_FreeDetachedBlock      (memBlock);              }
#endif

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStack Initialisation Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnMemStack_InitWithFixedMem(DqnMemStack *const stack, u8 *const mem,
                                                 const size_t memSize, const u32 byteAlign)
{
	// TODO(doyle): Logging
	if (!stack || !mem) return false;

	if (!DQN_ASSERT_MSG(
	        memSize > sizeof(DqnMemStackBlock),
	        "memSize is insufficient to initialise a memstack, memSize: %d, requiredSize: %d",
	        memSize, sizeof(DqnMemStackBlock)))
	{
		return false;
	}

	stack->block            = (DqnMemStackBlock *)mem;
	stack->block->memory    = mem + sizeof(DqnMemStackBlock);
	stack->block->used      = 0;
	stack->block->size      = memSize - sizeof(DqnMemStackBlock);
	stack->block->prevBlock = NULL;
	stack->flags = (DqnMemStackFlag_IsFixedMemoryFromUser | DqnMemStackFlag_IsNotExpandable);

	const u32 DEFAULT_ALIGNMENT = 4;
	stack->tempRegionCount      = 0;
	stack->byteAlign = (byteAlign == 0) ? DEFAULT_ALIGNMENT : byteAlign;

	DQN_ASSERT(!stack->block->prevBlock);
	return true;
}

DQN_FILE_SCOPE bool DqnMemStack_InitWithFixedSize(DqnMemStack *const stack, size_t size,
                                                  const bool zeroClear, const u32 byteAlign)
{
	bool result = DqnMemStack_Init(stack, size, zeroClear, byteAlign);
	if (result)
	{
		stack->flags |= DqnMemStackFlag_IsNotExpandable;
		DQN_ASSERT(!stack->block->prevBlock);
		return true;
	}

	return false;
}

DQN_FILE_SCOPE bool DqnMemStack_Init(DqnMemStack *const stack, size_t size, const bool zeroClear,
                                     const u32 byteAlign)
{
	if (!stack || size <= 0) return false;
	if (!DQN_ASSERT_MSG(!stack->block, "MemStack has pre-existing block already attached"))
		return false;

	stack->block = DqnMemStackInternal_AllocateBlock(byteAlign, size, zeroClear);
	if (!DQN_ASSERT_MSG(stack->block, "MemStack failed to allocate block, not enough memory"))
		return false;

	stack->tempRegionCount = 0;
	stack->byteAlign       = byteAlign;
	stack->flags           = 0;
	DQN_ASSERT(!stack->block->prevBlock);
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStack Push/Pop/Free Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE void *DqnMemStack_Push(DqnMemStack *const stack, size_t size)
{
	if (!stack || size == 0) return NULL;

	size_t alignedSize = DQN_ALIGN_POW_N(size, stack->byteAlign);
	if (!stack->block ||
	    (stack->block->used + alignedSize) > stack->block->size)
	{
		size_t newBlockSize;
		// TODO(doyle): Allocate block size based on the aligned size or
		// a minimum block size? Not allocate based on the current block
		// size
		if (stack->block) newBlockSize = DQN_MAX(alignedSize, stack->block->size);
		else newBlockSize = alignedSize;

		DqnMemStackBlock *newBlock = DqnMemStack_AllocateCompatibleBlock(stack, newBlockSize, true);
		if (newBlock)
		{
			bool blockAttachResult = DqnMemStack_AttachBlock(stack, newBlock);
			// IMPORTANT(doyle): This should be impossible, considering that
			// AllocateCompatibleBlock checks the preconditions that the new
			// block should be able to be attached.

			// But if we somehow reach this, we need to free the block
			// otherwise memory is leaked.
			DQN_ASSERT_HARD(blockAttachResult);
		}
		else
		{
			// TODO: Better notifying to user, out of space in stack OR stack
			// is configured such that new blocks are not allowed.
			return NULL;
		}
	}

	u8 *currPointer        = stack->block->memory + stack->block->used;
	u8 *alignedResult      = (u8 *)DQN_ALIGN_POW_N(currPointer, stack->byteAlign);
	size_t alignmentOffset = (size_t)(alignedResult - currPointer);

	// NOTE(doyle): Since all stack can't change alignment once they've been
	// initialised and that the base memory ptr is already aligned, then all
	// subsequent allocations should also be aligned automatically.
	// TODO(doyle): In the future, do we want to allow arbitrary alignment PER
	// allocation, not per MemStack?
	DQN_ASSERT_HARD(alignmentOffset == 0);

	void *result = alignedResult;
	stack->block->used += (alignedSize + alignmentOffset);
	DQN_ASSERT_HARD(stack->block->used <= stack->block->size);
	return result;
}

DQN_FILE_SCOPE bool DqnMemStack_Pop(DqnMemStack *const stack, void *ptr, size_t size)
{
	if (!stack || !stack->block) return false;

	u8 *currPtr = stack->block->memory + stack->block->used;
	if (DQN_ASSERT_MSG((u8 *)ptr >= stack->block->memory && ptr < currPtr,
	                   "'ptr' to pop does not belong to current memStack attached block"))
	{
		size_t calcSize = (size_t)currPtr - (size_t)ptr;
		size_t sizeAligned = DQN_ALIGN_POW_N(size, stack->byteAlign);
		if (DQN_ASSERT_MSG(calcSize == sizeAligned, "'ptr' was not the last item allocated to memStack"))
		{
			stack->block->used -= sizeAligned;
			if (stack->block->used == 0 && stack->block->prevBlock)
			{
				return DQN_ASSERT(DqnMemStack_FreeLastBlock(stack));
			}
			return true;
		}
	}

	return false;
}

DQN_FILE_SCOPE void DqnMemStack_Free(DqnMemStack *stack)
{
	if (!stack) return;

	// NOTE(doyle): User is in charge of freeing this memory, so all we need to
	// do is clear the block.
	if (stack->flags & DqnMemStackFlag_IsFixedMemoryFromUser)
	{
		DQN_ASSERT_HARD(!stack->block->prevBlock);
		DqnMemStack_ClearCurrBlock(stack, false);
		return;
	}

	while (stack->block)
		DqnMemStack_FreeLastBlock(stack);

	// After a stack is free, we reset the not expandable flag so that if we
	// allocate on an empty stack it still works.
	stack->flags &= ~DqnMemStackFlag_IsNotExpandable;
}

DQN_FILE_SCOPE bool DqnMemStack_FreeMemBlock(DqnMemStack *const stack, DqnMemStackBlock *memBlock)
{
	if (!stack || !memBlock || !stack->block) return false;
	if (stack->flags & DqnMemStackFlag_IsFixedMemoryFromUser) return false;

	DqnMemStackBlock **blockPtr = &stack->block;

	while (*blockPtr && (*blockPtr) != memBlock)
		blockPtr = &((*blockPtr)->prevBlock);

	if (*blockPtr)
	{
		DqnMemStackBlock *blockToFree = *blockPtr;
		(*blockPtr)                    = blockToFree->prevBlock;
		DqnMem_Free(blockToFree);

		// No more blocks, then last block has been freed
		if (!stack->block) DQN_ASSERT_HARD(stack->tempRegionCount == 0);
		return true;
	}

	return false;
}

DQN_FILE_SCOPE bool DqnMemStack_FreeLastBlock(DqnMemStack *const stack)
{
	bool result = DqnMemStack_FreeMemBlock(stack, stack->block);
	return result;
}

DQN_FILE_SCOPE void DqnMemStack_ClearCurrBlock(DqnMemStack *const stack, const bool zeroClear)
{
	if (!stack) return;
	if (stack->block)
	{
		stack->block->used = 0;
		if (zeroClear)
		{
			DqnMem_Clear(stack->block->memory, 0, stack->block->size);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStackTempRegion Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnMemStackTempRegion_Begin(DqnMemStackTempRegion *const region,
                                                DqnMemStack *const stack)
{
	if (!region || !stack) return false;

	region->stack           = stack;
	region->startingBlock   = stack->block;
	region->used            = stack->block->used;

	stack->tempRegionCount++;
	return true;
}

DQN_FILE_SCOPE void DqnMemStackTempRegion_End(DqnMemStackTempRegion region)
{
	DqnMemStack *stack = region.stack;
	while (stack->block != region.startingBlock)
		DqnMemStack_FreeLastBlock(stack);

	if (stack->block)
	{
		DQN_ASSERT_HARD(stack->block->used >= region.used);
		stack->block->used = region.used;
	}

	stack->tempRegionCount--;
	DQN_ASSERT_HARD(stack->tempRegionCount >= 0);
}

#ifdef DQN_CPP_MODE
DqnMemStackTempRegionGuard::DqnMemStackTempRegionGuard(DqnMemStack *const stack, bool *const succeeded)
{
	if (stack)
	{
		DQN_ASSERT_HARD(DqnMemStackTempRegion_Begin(&this->tempMemStack, stack));
		if (succeeded) *succeeded = true;
	}
	else
	{
		if (succeeded) *succeeded = false;
	}
}

DqnMemStackTempRegionGuard::~DqnMemStackTempRegionGuard()
{
	DqnMemStackTempRegion_End(this->tempMemStack);
}
#endif

////////////////////////////////////////////////////////////////////////////////
// #DqnMemStack Advanced API Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnMemStackBlock *DqnMemStack_AllocateCompatibleBlock(const DqnMemStack *const stack,
                                                                     size_t size,
                                                                     const bool zeroClear)
{
	if (!stack) return NULL;
	if (stack->flags & DqnMemStackFlag_IsFixedMemoryFromUser) return NULL;
	if (stack->flags & DqnMemStackFlag_IsNotExpandable)       return NULL;

	DqnMemStackBlock *block =
	    DqnMemStackInternal_AllocateBlock(stack->byteAlign, size, zeroClear);
	return block;
}

DQN_FILE_SCOPE bool DqnMemStack_AttachBlock(DqnMemStack *const stack,
                                            DqnMemStackBlock *const newBlock)
{
	if (!stack || !newBlock) return false;
	if (stack->flags & DqnMemStackFlag_IsFixedMemoryFromUser) return false;
	if (stack->flags & DqnMemStackFlag_IsNotExpandable)       return false;

	newBlock->prevBlock = stack->block;
	stack->block        = newBlock;
	return true;
}

DQN_FILE_SCOPE bool DqnMemStack_DetachBlock(DqnMemStack *const stack,
                                            DqnMemStackBlock *const detachBlock)
{
	if (!stack || !detachBlock) return false;
	if (stack->flags & DqnMemStackFlag_IsFixedMemoryFromUser) return false;
	if (stack->flags & DqnMemStackFlag_IsNotExpandable)       return false;

	DqnMemStackBlock **blockPtr = &stack->block;
	while (*blockPtr && *blockPtr != detachBlock)
		blockPtr = &((*blockPtr)->prevBlock);

	if (*blockPtr)
	{
		*blockPtr = detachBlock->prevBlock;
		detachBlock->prevBlock = NULL;
	}
	else
	{
		return false;
	}

	return true;
}

DQN_FILE_SCOPE void DqnMemStack_FreeDetachedBlock(DqnMemStackBlock *memBlock)
{
	if (!memBlock) return;
	DqnMem_Free(memBlock);
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemAPIInternal Implementation
////////////////////////////////////////////////////////////////////////////////
FILE_SCOPE inline DqnMemAPICallbackInfo
DqnMemAPIInternal_CallbackInfoAskRealloc(const DqnMemAPI memAPI,
                                         void *const oldMemPtr,
                                         const size_t oldSize,
                                         const size_t newSize)
{
	DqnMemAPICallbackInfo info = {0};
	info.type           = DqnMemAPICallbackType_Realloc;
	info.userContext    = memAPI.userContext;
	info.newRequestSize = newSize;
	info.oldMemPtr      = oldMemPtr;
	info.oldSize        = oldSize;
	return info;
}

FILE_SCOPE inline DqnMemAPICallbackInfo
DqnMemAPIInternal_CallbackInfoAskAlloc(const DqnMemAPI memAPI,
                                       const size_t size)
{
	DqnMemAPICallbackInfo info = {0};
	info.type        = DqnMemAPICallbackType_Alloc;
	info.userContext = memAPI.userContext;
	info.requestSize = size;
	return info;
}

FILE_SCOPE DqnMemAPICallbackInfo DqnMemAPIInternal_CallbackInfoAskFree(
    const DqnMemAPI memAPI, void *const ptrToFree, const size_t sizeToFree)
{
	DqnMemAPICallbackInfo info = {0};
	info.type        = DqnMemAPICallbackType_Free;
	info.userContext = memAPI.userContext;
	info.ptrToFree   = ptrToFree;
	info.sizeToFree  = sizeToFree;
	return info;
}

FILE_SCOPE void DqnMemAPIInternal_ValidateCallbackInfo(DqnMemAPICallbackInfo info)
{
	DQN_ASSERT_HARD(info.type != DqnMemAPICallbackType_Invalid);

	switch(info.type)
	{
		case DqnMemAPICallbackType_Alloc:
		{
			DQN_ASSERT_HARD(info.requestSize > 0);
		}
		break;

		case DqnMemAPICallbackType_Realloc:
		{
			DQN_ASSERT_HARD(info.oldSize > 0);
			DQN_ASSERT_HARD(info.requestSize > 0);
			DQN_ASSERT_HARD(info.oldMemPtr);
		}
		break;

		case DqnMemAPICallbackType_Free:
		{
			// nothing to validate
		}
		break;

		default:
		{
			DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
		}
		break;
	}
}

FILE_SCOPE void DqnMemAPIInternal_DefaultUseCallocCallback(DqnMemAPICallbackInfo info,
                                                           DqnMemAPICallbackResult *result)
{
	DQN_ASSERT_HARD(!info.userContext);

	DqnMemAPIInternal_ValidateCallbackInfo(info);
	switch(info.type)
	{
		case DqnMemAPICallbackType_Alloc:
		{
			result->type = info.type;
			result->newMemPtr = DqnMem_Calloc(info.requestSize);
		}
		break;

		case DqnMemAPICallbackType_Realloc:
		{
			result->type = info.type;
			result->newMemPtr =
			    DqnMem_Realloc(info.oldMemPtr, info.newRequestSize);
		}
		break;

		case DqnMemAPICallbackType_Free:
		{
			// NOTE(doyle): We can pass in NULL as result if we're freeing since
			// there's nothing to return. But if the callback result has been
			// passed in, we can fill the type data out.
			if (result) result->type = info.type;
			DqnMem_Free(info.ptrToFree);
		}
		break;

		default:
		{
			DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
		}
		break;
	}
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMemAPI Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnMemAPI DqnMemAPI_DefaultUseCalloc()
{
	DqnMemAPI result   = {0};
	result.callback    = DqnMemAPIInternal_DefaultUseCallocCallback;
	result.userContext = NULL;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMath Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE f32 DqnMath_Lerp(f32 a, f32 t, f32 b)
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

DQN_FILE_SCOPE f32 DqnMath_Sqrtf(f32 a)
{
	f32 result = sqrtf(a);
	return result;
}

DQN_FILE_SCOPE f32 DqnMath_Clampf(f32 val, f32 min, f32 max)
{
	if (val < min) return min;
	if (val > max) return max;
	return val;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV2 Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV2 DqnV2_1f(f32 xy)
{
	DqnV2 result = {0};
	result.x  = xy;
	result.y  = xy;

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_2f(f32 x, f32 y)
{
	DqnV2 result = {0};
	result.x  = x;
	result.y  = y;

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_2i(i32 x, i32 y)
{
	DqnV2 result = DqnV2_2f((f32)x, (f32)y);
	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_V2i(DqnV2i a)
{
	DqnV2 result = {0};
	result.x = (f32)a.x;
	result.y = (f32)a.y;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV2 Arithmetic Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV2 DqnV2_Add(DqnV2 a, DqnV2 b)
{
	DqnV2 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Sub(DqnV2 a, DqnV2 b)
{
	DqnV2 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalei(DqnV2 a, i32 b)
{
	DqnV2 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Scalef(DqnV2 a, f32 b)
{
	DqnV2 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Hadamard(DqnV2 a, DqnV2 b)
{
	DqnV2 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE f32 DqnV2_Dot(DqnV2 a, DqnV2 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE bool DqnV2_Equals(DqnV2 a, DqnV2 b)
{
	bool result = true;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = false;
	return result;
}

DQN_FILE_SCOPE f32 DqnV2_LengthSquared(DqnV2 a, DqnV2 b)
{
	f32 x      = b.x - a.x;
	f32 y      = b.y - a.y;
	f32 result = (DQN_SQUARED(x) + DQN_SQUARED(y));
	return result;
}

DQN_FILE_SCOPE f32 DqnV2_Length(DqnV2 a, DqnV2 b)
{
	f32 lengthSq = DqnV2_LengthSquared(a, b);
	if (lengthSq == 0) return 0;

	f32 result = DqnMath_Sqrtf(lengthSq);
	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_Normalise(DqnV2 a)
{
	f32 magnitude = DqnV2_Length(DqnV2_2f(0, 0), a);
	if (magnitude == 0) return DqnV2_1f(0.0f);

	DqnV2 result = DqnV2_Scalef(a, 1 / magnitude);
	return result;
}

DQN_FILE_SCOPE bool DqnV2_Overlaps(DqnV2 a, DqnV2 b)
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

DQN_FILE_SCOPE DqnV2 DqnV2_Perpendicular(DqnV2 a)
{
	DqnV2 result = DqnV2_2f(a.y, -a.x);
	return result;
}


DQN_FILE_SCOPE DqnV2 DqnV2_ResizeKeepAspectRatio(DqnV2 srcSize, DqnV2 targetSize)
{
	f32 ratioA   = srcSize.w / targetSize.w;
	f32 ratioB   = srcSize.h / targetSize.h;
	f32 ratio    = DQN_MIN(ratioA, ratioB);
	DqnV2 result = DqnV2_Scalef(targetSize, ratio);
	return result;
}

DQN_FILE_SCOPE DqnV2 DqnV2_ConstrainToRatio(DqnV2 dim, DqnV2 ratio)
{
	DqnV2 result                  = {0};
	f32 numRatioIncrementsToWidth  = (f32)(dim.w / ratio.w);
	f32 numRatioIncrementsToHeight = (f32)(dim.h / ratio.h);

	f32 leastIncrementsToSide =
	    DQN_MIN(numRatioIncrementsToHeight, numRatioIncrementsToWidth);

	result.w = (f32)(ratio.w * leastIncrementsToSide);
	result.h = (f32)(ratio.h * leastIncrementsToSide);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV2i Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV2i DqnV2i_2i(i32 x, i32 y)
{
	DqnV2i result = {0};
	result.x      = x;
	result.y      = y;
	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_2f(f32 x, f32 y)
{
	DqnV2i result = DqnV2i_2i((i32)x, (i32)y);
	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_V2(DqnV2 a)
{
	DqnV2i result = {0};
	result.x      = (i32)a.x;
	result.y      = (i32)a.y;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV2i Arithmetic Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV2i DqnV2i_Add(DqnV2i a, DqnV2i b)
{
	DqnV2i result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Sub(DqnV2i a, DqnV2i b)
{
	DqnV2i result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalef(DqnV2i a, f32 b)
{
	DqnV2i result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = (i32)(a.e[i] * b);

	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Scalei(DqnV2i a, i32 b)
{
	DqnV2i result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV2i DqnV2i_Hadamard(DqnV2i a, DqnV2i b)
{
	DqnV2i result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE f32 DqnV2i_Dot(DqnV2i a, DqnV2i b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE bool DqnV2i_Equals(DqnV2i a, DqnV2i b)
{
	bool result = true;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = false;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV3 Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV3 DqnV3_1f(f32 xyz)
{
	DqnV3 result = {xyz, xyz, xyz};
	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_3f(f32 x, f32 y, f32 z)
{
	DqnV3 result = {x, y, z};
	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_3i(i32 x, i32 y, i32 z)
{
	DqnV3 result = {(f32)x, (f32)y, (f32)z};
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV3 Arithmetic Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV3 DqnV3_Add(DqnV3 a, DqnV3 b)
{
	DqnV3 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Sub(DqnV3 a, DqnV3 b)
{
	DqnV3 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalei(DqnV3 a, i32 b)
{
	DqnV3 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Scalef(DqnV3 a, f32 b)
{
	DqnV3 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Hadamard(DqnV3 a, DqnV3 b)
{
	DqnV3 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE f32 DqnV3_Dot(DqnV3 a, DqnV3 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE bool DqnV3_Equals(DqnV3 a, DqnV3 b)
{
	bool result = true;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = false;
	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Cross(DqnV3 a, DqnV3 b)
{
	/*
	   CROSS PRODUCT
	   Generate a perpendicular vector to the 2 vectors
	   |a|   |d|   |bf - ce|
	   |b| x |e| = |cd - af|
	   |c|   |f|   |ae - be|
	 */
	DqnV3 result = {0};
	result.e[0] = (a.e[1] * b.e[2]) - (a.e[2] * b.e[1]);
	result.e[1] = (a.e[2] * b.e[0]) - (a.e[0] * b.e[2]);
	result.e[2] = (a.e[0] * b.e[1]) - (a.e[1] * b.e[0]);
	return result;
}

DQN_FILE_SCOPE DqnV3 DqnV3_Normalise(DqnV3 a)
{
	f32 length    = DqnMath_Sqrtf(DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z));
	f32 invLength = 1 / length;
	DqnV3 result  = a * invLength;

	return result;
}

DQN_FILE_SCOPE f32 DqnV3_LengthSquared(DqnV3 a, DqnV3 b)
{
	f32 x      = b.x - a.x;
	f32 y      = b.y - a.y;
	f32 z      = b.z - a.z;
	f32 result = (DQN_SQUARED(x) + DQN_SQUARED(y) + DQN_SQUARED(z));
	return result;
}

DQN_FILE_SCOPE f32 DqnV3_Length(DqnV3 a, DqnV3 b)
{
	f32 lengthSq = DqnV3_LengthSquared(a, b);
	if (lengthSq == 0) return 0;

	f32 result = DqnMath_Sqrtf(lengthSq);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV3i Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV3i DqnV3i_3i(i32 x, i32 y, i32 z)
{
	DqnV3i result = {x, y, z};
	return result;
}

DQN_FILE_SCOPE DqnV3i DqnV3i_3f(f32 x, f32 y, f32 z)
{
	DqnV3i result = {(i32)x, (i32)y, (i32)z};
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV4 Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV4 DqnV4_1f(f32 xyzw)
{
	DqnV4 result = {xyzw, xyzw, xyzw, xyzw};
	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_4f(f32 x, f32 y, f32 z, f32 w)
{
	DqnV4 result = {x, y, z, w};
	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_4i(i32 x, i32 y, i32 z, i32 w)
{
	DqnV4 result = DqnV4_4f((f32)x, (f32)y, (f32)z, (f32)w);
	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_V3(DqnV3 a, f32 w)
{
	DqnV4 result;
	result.xyz = a;
	result.w   = w;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnV4 Arithmetic Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnV4 DqnV4_Add(DqnV4 a, DqnV4 b)
{
	DqnV4 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] + b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Sub(DqnV4 a, DqnV4 b)
{
	DqnV4 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] - b.e[i];

	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalei(DqnV4 a, i32 b)
{
	DqnV4 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Scalef(DqnV4 a, f32 b)
{
	DqnV4 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b;

	return result;
}

DQN_FILE_SCOPE DqnV4 DqnV4_Hadamard(DqnV4 a, DqnV4 b)
{
	DqnV4 result = {0};
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result.e[i] = a.e[i] * b.e[i];

	return result;
}

DQN_FILE_SCOPE f32 DqnV4_Dot(DqnV4 a, DqnV4 b)
{
	/*
	   DOT PRODUCT
	   Two vectors with dot product equals |a||b|cos(theta)
	   |a|   |d|
	   |b| . |e| = (ad + be + cf)
	   |c|   |f|
	 */
	f32 result = 0;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		result += (a.e[i] * b.e[i]);

	return result;
}

DQN_FILE_SCOPE bool DqnV4_Equals(DqnV4 a, DqnV4 b)
{
	bool result = true;
	for (u32 i = 0; i < DQN_ARRAY_COUNT(a.e); i++)
		if (a.e[i] != b.e[i]) result = false;
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnMat4 Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnMat4 DqnMat4_Identity()
{
	DqnMat4 result = {0};
	result.e[0][0] = 1;
	result.e[1][1] = 1;
	result.e[2][2] = 1;
	result.e[3][3] = 1;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Orthographic(f32 left, f32 right, f32 bottom, f32 top, f32 zNear,
                                            f32 zFar)
{
	DqnMat4 result = DqnMat4_Identity();
	result.e[0][0] = +2.0f / (right - left);
	result.e[1][1] = +2.0f / (top   - bottom);
	result.e[2][2] = -2.0f / (zFar  - zNear);

	result.e[3][0] = -(right + left)   / (right - left);
	result.e[3][1] = -(top   + bottom) / (top   - bottom);
	result.e[3][2] = -(zFar  + zNear)  / (zFar  - zNear);

	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Perspective(f32 fovYDegrees, f32 aspectRatio, f32 zNear, f32 zFar)
{
	f32 fovYRadians         = DQN_DEGREES_TO_RADIANS(fovYDegrees);
	f32 fovYRadiansOver2    = fovYRadians * 0.5f;
	f32 tanFovYRadiansOver2 = tanf(fovYRadiansOver2);
	f32 zNearSubZFar        = zNear - zFar;

	DqnMat4 result = DqnMat4_Identity();
	result.e[0][0] = 1.0f / (aspectRatio * tanFovYRadiansOver2);
	result.e[1][1] = 1.0f / tanFovYRadiansOver2;
	result.e[2][2] = (zNear + zFar) / zNearSubZFar;
	result.e[2][3] = -1.0f;
	result.e[3][2] = (2.0f * zNear * zFar) / zNearSubZFar;
	result.e[3][3] = 0.0f;

	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_LookAt(DqnV3 eye, DqnV3 center, DqnV3 up)
{
	DqnMat4 result = {0};

	DqnV3 f = DqnV3_Normalise(DqnV3_Sub(eye, center));
	DqnV3 s = DqnV3_Normalise(DqnV3_Cross(up, f));
	DqnV3 u = DqnV3_Cross(f, s);

	result.e[0][0] = s.x;
	result.e[0][1] = u.x;
	result.e[0][2] = -f.x;

	result.e[1][0] = s.y;
	result.e[1][1] = u.y;
	result.e[1][2] = -f.y;

	result.e[2][0] = s.z;
	result.e[2][1] = u.z;
	result.e[2][2] = -f.z;

	result.e[3][0] = -DqnV3_Dot(s, eye);
	result.e[3][1] = -DqnV3_Dot(u, eye);
	result.e[3][2] = DqnV3_Dot(f, eye);
	result.e[3][3] = 1.0f;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Translate3f(f32 x, f32 y, f32 z)
{
	DqnMat4 result = DqnMat4_Identity();
	result.e[3][0] = x;
	result.e[3][1] = y;
	result.e[3][2] = z;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_TranslateV3(DqnV3 vec)
{
	DqnMat4 result = DqnMat4_Identity();
	result.e[3][0] = vec.x;
	result.e[3][1] = vec.y;
	result.e[3][2] = vec.z;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Rotate(f32 radians, f32 x, f32 y, f32 z)
{
	DqnMat4 result     = DqnMat4_Identity();
	f32 sinVal         = sinf(radians);
	f32 cosVal         = cosf(radians);
	f32 oneMinusCosVal = 1 - cosVal;

	DqnV3 axis = DqnV3_Normalise(DqnV3_3f(x, y, z));

	result.e[0][0] = (axis.x * axis.x * oneMinusCosVal) + cosVal;
	result.e[0][1] = (axis.x * axis.y * oneMinusCosVal) + (axis.z * sinVal);
	result.e[0][2] = (axis.x * axis.z * oneMinusCosVal) - (axis.y * sinVal);

	result.e[1][0] = (axis.y * axis.x * oneMinusCosVal) - (axis.z * sinVal);
	result.e[1][1] = (axis.y * axis.y * oneMinusCosVal) + cosVal;
	result.e[1][2] = (axis.y * axis.z * oneMinusCosVal) + (axis.x * sinVal);

	result.e[2][0] = (axis.z * axis.x * oneMinusCosVal) + (axis.y * sinVal);
	result.e[2][1] = (axis.z * axis.y * oneMinusCosVal) - (axis.x * sinVal);
	result.e[2][2] = (axis.z * axis.z * oneMinusCosVal) + cosVal;

	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Scale(f32 x, f32 y, f32 z)
{
	DqnMat4 result = {0};
	result.e[0][0] = x;
	result.e[1][1] = y;
	result.e[2][2] = z;
	result.e[3][3] = 1;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_ScaleV3(DqnV3 scale)
{
	DqnMat4 result = {0};
	result.e[0][0] = scale.x;
	result.e[1][1] = scale.y;
	result.e[2][2] = scale.z;
	result.e[3][3] = 1;
	return result;
}

DQN_FILE_SCOPE DqnMat4 DqnMat4_Mul(DqnMat4 a, DqnMat4 b)
{
	DqnMat4 result = {0};
	for (u32 j = 0; j < 4; j++) {
		for (u32 i = 0; i < 4; i++)
		{
			result.e[j][i] = a.e[0][i] * b.e[j][0]
			               + a.e[1][i] * b.e[j][1]
			               + a.e[2][i] * b.e[j][2]
			               + a.e[3][i] * b.e[j][3];
		}
	}

	return result;
}

DQN_FILE_SCOPE DqnV4 DqnMat4_MulV4(DqnMat4 a, DqnV4 b)
{
	DqnV4 result = {0};
	result.x = (a.e[0][0] * b.x) + (a.e[1][0] * b.y) + (a.e[2][0] * b.z) + (a.e[3][0] * b.w);
	result.y = (a.e[0][1] * b.x) + (a.e[1][1] * b.y) + (a.e[2][1] * b.z) + (a.e[3][1] * b.w);
	result.z = (a.e[0][2] * b.x) + (a.e[1][2] * b.y) + (a.e[2][2] * b.z) + (a.e[3][2] * b.w);
	result.w = (a.e[0][3] * b.x) + (a.e[1][3] * b.y) + (a.e[2][3] * b.z) + (a.e[3][3] * b.w);

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnRect Init Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE DqnRect DqnRect_4f(f32 minX, f32 minY, f32 maxX, f32 maxY)
{
	DqnRect result = {0};
	result.min     = DqnV2_2f(minX, minY);
	result.max     = DqnV2_2f(maxX, maxY);

	return result;
}

DQN_FILE_SCOPE DqnRect DqnRect_4i(i32 minX, i32 minY, i32 maxX, i32 maxY)
{
	DqnRect result = {0};
	result.min     = DqnV2_2i(minX, minY);
	result.max     = DqnV2_2i(maxX, maxY);

	return result;
}

DQN_FILE_SCOPE DqnRect DqnRect_Init(DqnV2 origin, DqnV2 size)
{
	DqnRect result = {0};
	result.min      = origin;
	result.max      = DqnV2_Add(origin, size);

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnRect Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE void DqnRect_GetSize2f(DqnRect rect, f32 *width, f32 *height)
{
	*width  = rect.max.x - rect.min.x;
	*height = rect.max.y - rect.min.y;
}

DQN_FILE_SCOPE void DqnRect_GetSize2i(DqnRect rect, i32 *width, i32 *height)
{
	*width  = (i32)(rect.max.x - rect.min.x);
	*height = (i32)(rect.max.y - rect.min.y);
}

DQN_FILE_SCOPE DqnV2 DqnRect_GetSizeV2(DqnRect rect)
{
	f32 width     = rect.max.x - rect.min.x;
	f32 height    = rect.max.y - rect.min.y;
	DqnV2 result  = DqnV2_2f(width, height);
	return result;
}

DQN_FILE_SCOPE DqnV2 DqnRect_GetCentre(DqnRect rect)
{
	f32 sumX  = rect.min.x + rect.max.x;
	f32 sumY  = rect.min.y + rect.max.y;
	DqnV2 result = DqnV2_Scalef(DqnV2_2f(sumX, sumY), 0.5f);
	return result;
}

DQN_FILE_SCOPE DqnRect DqnRect_ClipRect(DqnRect rect, DqnRect clip)
{
	DqnRect result = {0};
	DqnV2 clipSize = DqnRect_GetSizeV2(clip);

	result.max.x = DQN_MIN(rect.max.x, clipSize.w);
	result.max.y = DQN_MIN(rect.max.y, clipSize.h);
	result.min.x = DQN_MAX(clip.min.x, rect.min.x);
	result.min.y = DQN_MAX(clip.min.y, rect.min.y);
	return result;
}

DQN_FILE_SCOPE DqnRect DqnRect_Move(DqnRect rect, DqnV2 shift)
{
	DqnRect result = {0};
	result.min       = DqnV2_Add(rect.min, shift);
	result.max       = DqnV2_Add(rect.max, shift);

	return result;
}

DQN_FILE_SCOPE bool DqnRect_ContainsP(DqnRect rect, DqnV2 p)
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
// #DqnChar Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE char DqnChar_ToLower(char c)
{
	if (c >= 'A' && c <= 'Z')
	{
		i32 shiftOffset = 'a' - 'A';
		return (c + (char)shiftOffset);
	}
	return c;
}

DQN_FILE_SCOPE char DqnChar_ToUpper(char c)
{
	if (c >= 'a' && c <= 'z')
	{
		i32 shiftOffset = 'a' - 'A';
		return (c - (char)shiftOffset);
	}
	return c;
}


DQN_FILE_SCOPE bool DqnChar_IsDigit(char c)
{
	if (c >= '0' && c <= '9') return true;
	return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlpha(char c)
{
	if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z')) return true;
	return false;
}

DQN_FILE_SCOPE bool DqnChar_IsAlphaNum(char c)
{
	if (DqnChar_IsAlpha(c) || DqnChar_IsDigit(c)) return true;
	return false;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnStr Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE i32 DqnStr_Cmp(const char *const a, const char *const b)
{
	if (!a && !b) return -1;
	if (!a) return -1;
	if (!b) return -1;

	char const *aPtr = a;
	char const *bPtr = b;

	while ((*aPtr) == (*bPtr))
	{
		if (!(*aPtr)) return 0;
		aPtr++;
		bPtr++;
	}

	return (((*aPtr) < (*bPtr)) ? -1 : 1);
}

DQN_FILE_SCOPE i32 DqnStr_Len(const char *const a)
{
	i32 result = 0;
	while (a && a[result]) result++;

	return result;
}

DQN_FILE_SCOPE i32 DqnStr_LenDelimitWith(const char *a, const char delimiter)
{
	i32 result = 0;
	while (a && a[result] && a[result] != delimiter) result++;
	return result;
}

DQN_FILE_SCOPE char *DqnStr_Copy(char *const dest, const char *const src, const i32 numChars)
{
	if (!dest) return NULL;
	if (!src)  return NULL;
	if (numChars < 0) return NULL;

	for (i32 i  = 0; i < numChars; i++)
		dest[i] = src[i];

	return dest;
}

DQN_FILE_SCOPE bool DqnStr_Reverse(char *const buf, const i32 bufSize)
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

DQN_FILE_SCOPE i32 DqnStr_FindFirstOccurence(const char *const src, const i32 srcLen,
                                             const char *const find, const i32 findLen)
{
	if (!src || !find)               return -1;
	if (srcLen == 0 || findLen == 0) return -1;
	if (srcLen < findLen)            return -1;

	for (i32 indexIntoSrc = 0; indexIntoSrc < srcLen; indexIntoSrc++)
	{
		// NOTE: As we scan through, if the src string we index into becomes
		// shorter than the substring we're checking then the substring is not
		// contained in the src string.
		i32 remainingLenInSrcStr = srcLen - indexIntoSrc;
		if (remainingLenInSrcStr < findLen) break;

		const char *srcSubStr = &src[indexIntoSrc];
		i32 index = 0;
		for (;;)
		{
			if (DqnChar_ToLower(srcSubStr[index]) ==
			    DqnChar_ToLower(find[index]))
			{
				index++;
				if (index >= findLen || !find[index])
				{
					return indexIntoSrc;
				}
			}
			else
			{
				break;
			}
		}
	}

	// NOTE(doyle): We have early exit, if we reach here, then the substring was
	// not found.
	return -1;
}

DQN_FILE_SCOPE bool DqnStr_HasSubstring(const char *const src, const i32 srcLen,
                                        const char *const find, const i32 findLen)
{
	if (DqnStr_FindFirstOccurence(src, srcLen, find, findLen) == -1)
		return false;

	return true;
}

DQN_FILE_SCOPE i32 Dqn_I64ToStr(const i64 value, char *const buf, const i32 bufSize)
{
	bool validBuffer = true;
	if (!buf || bufSize == 0) validBuffer = false;

	if (value == 0)
	{
		if (validBuffer) buf[0] = '0';
		return 1;
	}
	
	i32 charIndex = 0;
	bool negative           = false;
	if (value < 0) negative = true;

	if (negative)
	{
		if (validBuffer) buf[charIndex] = '-';
		charIndex++;
	}

	bool lastDigitDecremented = false;
	i64 val = DQN_ABS(value);
	if (val < 0)
	{
		// TODO(doyle): This will occur if we are checking the smallest number
		// possible in i64 since the range of negative numbers is one more than
		// it is for positives, so ABS will fail.
		lastDigitDecremented = true;
		val                  = DQN_ABS(val - 1);
		DQN_ASSERT_HARD(val >= 0);
	}

	if (validBuffer)
	{
		if (lastDigitDecremented)
		{
			i64 rem = (val % 10) + 1;
			buf[charIndex++] = (u8)rem + '0';
			val /= 10;
		}

		while (val != 0 && charIndex < bufSize)
		{
			i64 rem          = val % 10;
			buf[charIndex++] = (u8)rem + '0';
			val /= 10;
		}

		// NOTE(doyle): If string is negative, we only want to reverse starting
		// from the second character, so we don't put the negative sign at the
		// end
		if (negative)
		{
			DqnStr_Reverse(buf + 1, charIndex - 1);
		}
		else
		{
			DqnStr_Reverse(buf, charIndex);
		}
	}
	else
	{
		while (val != 0)
		{
			val /= 10;
			charIndex++;
		}
	}

	return charIndex;
}

DQN_FILE_SCOPE i64 Dqn_StrToI64(const char *const buf, const i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	i32 index = 0;
	while (buf[index] == ' ')
	{
		index++;
	}

	bool isNegative = false;
	if (buf[index] == '-' || buf[index] == '+')
	{
		if (buf[index] == '-') isNegative = true;
		index++;
	}
	else if (!DqnChar_IsDigit(buf[index]))
	{
		return 0;
	}

	i64 result = 0;
	for (i32 i = index; i < bufSize; i++)
	{
		if (DqnChar_IsDigit(buf[i]))
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

DQN_FILE_SCOPE f32 Dqn_StrToF32(const char *const buf, const i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	i32 index       = 0;
	bool isNegative = false;
	if (buf[index] == '-')
	{
		index++;
		isNegative = true;
	}

	bool isPastDecimal        = false;
	i32 numDigitsAfterDecimal = 0;
	i32 rawNumber             = 0;

	f32 digitShiftValue      = 1.0f;
	f32 digitShiftMultiplier = 0.1f;
	for (i32 i = index; i < bufSize; i++)
	{
		char ch = buf[i];
		if (ch == '.')
		{
			isPastDecimal = true;
			continue;
		}
		// Handle scientific notation
		else if (ch == 'e')
		{
			bool digitShiftIsPositive = true;
			if (i < bufSize)
			{
				if (buf[i + 1] == '-') digitShiftIsPositive = false;
				DQN_ASSERT_HARD(buf[i + 1] == '-' || buf[i + 1] == '+');
				i += 2;
			}

			i32 exponentPow         = 0;
			bool scientificNotation = false;
			while (i < bufSize)
			{
				scientificNotation = true;
				char exponentCh    = buf[i];
				if (DqnChar_IsDigit(exponentCh))
				{
					exponentPow *= 10;
					exponentPow += (buf[i] - '0');
				}
				else
				{
					i = bufSize;
				}

				i++;
			}

			// NOTE(doyle): If exponent not specified but this branch occurred,
			// the float string has a malformed scientific notation in the
			// string, i.e. "e" followed by no number.
			DQN_ASSERT_HARD(scientificNotation);

			if (digitShiftIsPositive)
			{
				numDigitsAfterDecimal -= exponentPow;
			}
			else
			{
				numDigitsAfterDecimal += exponentPow;
			}
		}
		else if (DqnChar_IsDigit(ch))
		{
			numDigitsAfterDecimal += (i32)isPastDecimal;
			rawNumber *= 10;
			rawNumber += (ch - '0');
		}
		else
		{
			break;
		}
	}

	for (i32 i = 0; i < numDigitsAfterDecimal; i++)
		digitShiftValue *= digitShiftMultiplier;

	f32 result = (f32)rawNumber;
	if (numDigitsAfterDecimal > 0) result *= digitShiftValue;
	if (isNegative) result *= -1;

	return result;
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
DQN_FILE_SCOPE u32 Dqn_UCSToUTF8(u32 *const dest, const u32 character)
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

DQN_FILE_SCOPE u32 Dqn_UTF8ToUCS(u32 *const dest, const u32 character)
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
// #DqnWChar Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnWChar_IsDigit(const wchar_t c)
{
	if (c >= L'0' && c <= L'9') return true;
	return false;
}

DQN_FILE_SCOPE wchar_t DqnWChar_ToLower(const wchar_t c)
{
	if (c >= L'A' && c <= L'Z')
	{
		i32 shiftOffset = L'a' - L'A';
		return (c + (wchar_t)shiftOffset);
	}

	return c;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnWStr Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE i32 DqnWStr_Len(const wchar_t *const a)
{
	i32 result = 0;
	while (a && a[result]) result++;
	return result;
}

DQN_FILE_SCOPE i32 DqnWStr_Cmp(const wchar_t *const a, const wchar_t *const b)
{
	if (!a && !b) return -1;
	if (!a) return -1;
	if (!b) return -1;

	const wchar_t *aPtr = a;
	const wchar_t *bPtr = b;

	while ((*aPtr) == (*bPtr))
	{
		if (!(*aPtr)) return 0;
		aPtr++;
		bPtr++;
	}

	return (((*aPtr) < (*bPtr)) ? -1 : 1);
}

DQN_FILE_SCOPE bool Dqn_WStrReverse(wchar_t *buf, const i32 bufSize)
{
	if (!buf) return false;
	i32 mid = bufSize / 2;

	for (i32 i = 0; i < mid; i++)
	{
		wchar_t tmp            = buf[i];
		buf[i]                 = buf[(bufSize - 1) - i];
		buf[(bufSize - 1) - i] = tmp;
	}

	return true;
}

DQN_FILE_SCOPE i32 Dqn_WStrToI32(const wchar_t *const buf, const i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	i32 index       = 0;
	bool isNegative = false;
	if (buf[index] == L'-' || buf[index] == L'+')
	{
		if (buf[index] == L'-') isNegative = true;
		index++;
	}
	else if (!DqnWChar_IsDigit(buf[index]))
	{
		return 0;
	}

	i32 result = 0;
	for (i32 i = index; i < bufSize; i++)
	{
		if (DqnWChar_IsDigit(buf[i]))
		{
			result *= 10;
			result += (buf[i] - L'0');
		}
		else
		{
			break;
		}
	}

	if (isNegative) result *= -1;

	return result;
}

DQN_FILE_SCOPE i32 Dqn_I32ToWstr(i32 value, wchar_t *buf, i32 bufSize)
{
	if (!buf || bufSize == 0) return 0;

	if (value == 0)
	{
		buf[0] = L'0';
		return 0;
	}
	
	// NOTE(doyle): Max 32bit integer (+-)2147483647
	i32 charIndex = 0;
	bool negative           = false;
	if (value < 0) negative = true;

	if (negative) buf[charIndex++] = L'-';

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
		Dqn_WStrReverse(buf + 1, charIndex - 1);
	}
	else
	{
		Dqn_WStrReverse(buf, charIndex);
	}

	return charIndex;
}

////////////////////////////////////////////////////////////////////////////////
// #DqnRnd Implementation
////////////////////////////////////////////////////////////////////////////////
// Public Domain library with thanks to Mattias Gustavsson
// https://github.com/mattiasgustavsson/libs/blob/master/docs/rnd.md

// Convert a randomized u32 value to a float value x in the range 0.0f <= x
// < 1.0f. Contributed by Jonatan Hedborg

// NOTE: This is to abide to strict aliasing rules.
union DqnRndInternal_U32F32
{
	u32 unsigned32;
	f32 float32;
};

FILE_SCOPE f32 DqnRnd_F32NormalizedFromU32Internal(u32 value)
{
	u32 exponent = 127;
	u32 mantissa = value >> 9;

	union DqnRndInternal_U32F32 uf;
	uf.unsigned32 = (exponent << 23 | mantissa);
	return uf.float32 - 1.0f;
}

FILE_SCOPE u64 DqnRnd_Murmur3Avalanche64Internal(u64 h)
{
	h ^= h >> 33;
	h *= 0xff51afd7ed558ccd;
	h ^= h >> 33;
	h *= 0xc4ceb9fe1a85ec53;
	h ^= h >> 33;
	return h;
}

#if defined(DQN_UNIX_PLATFORM)
	#include <x86intrin.h> // __rdtsc
#endif

FILE_SCOPE u32 DqnRnd_MakeSeedInternal()
{
#if defined(DQN_WIN32_PLATFORM) || defined(DQN_UNIX_PLATFORM)
	i64 numClockCycles = __rdtsc();
	return (u32)numClockCycles;
#elif __ANDROID__
	DQN_ASSERT_MSG(DQN_INVALID_CODE_PATH, "Android path not implemented yet");
	return 0;
#else
	DQN_ASSERT_MSG(DQN_INVALID_CODE_PATH, "Non Win32 path not implemented yet");
	return 0;
#endif
}

DQN_FILE_SCOPE void DqnRnd_PCGInitWithSeed(DqnRandPCGState *pcg, u32 seed)
{
	u64 value     = (((u64)seed) << 1ULL) | 1ULL;
	value         = DqnRnd_Murmur3Avalanche64Internal(value);
	pcg->state[0] = 0U;
	pcg->state[1] = (value << 1ULL) | 1ULL;
	DqnRnd_PCGNext(pcg);
	pcg->state[0] += DqnRnd_Murmur3Avalanche64Internal(value);
	DqnRnd_PCGNext(pcg);
}

DQN_FILE_SCOPE void DqnRnd_PCGInit(DqnRandPCGState *pcg)
{
	u32 seed = DqnRnd_MakeSeedInternal();
	DqnRnd_PCGInitWithSeed(pcg, seed);
}

DQN_FILE_SCOPE u32 DqnRnd_PCGNext(DqnRandPCGState *pcg)
{
	u64 oldstate   = pcg->state[0];
	pcg->state[0]  = oldstate * 0x5851f42d4c957f2dULL + pcg->state[1];
	u32 xorshifted = (u32)(((oldstate >> 18ULL) ^ oldstate) >> 27ULL);
	u32 rot        = (u32)(oldstate >> 59ULL);
	return (xorshifted >> rot) | (xorshifted << ((-(i32)rot) & 31));
}

DQN_FILE_SCOPE f32 DqnRnd_PCGNextf(DqnRandPCGState *pcg)
{
	return DqnRnd_F32NormalizedFromU32Internal(DqnRnd_PCGNext(pcg));
}

DQN_FILE_SCOPE i32 DqnRnd_PCGRange(DqnRandPCGState *pcg, i32 min, i32 max)
{
	i32 const range = (max - min) + 1;
	if (range <= 0) return min;
	i32 const value = (i32)(DqnRnd_PCGNextf(pcg) * range);
	return min + value;
}

////////////////////////////////////////////////////////////////////////////////
// #Dqn_* Implementation
////////////////////////////////////////////////////////////////////////////////
FILE_SCOPE void *DqnInternal_QuickSortGetArrayItemPtr(void *const array, const u32 index,
                                                      const u32 size)
{
	u8 *byteArray = (u8 *)array;
	void *result = (void *)(byteArray + (index * size));
	return result;
}

DQN_FILE_SCOPE void Dqn_QuickSortC(void *const array, const u32 itemSize, const u32 size,
                                   Dqn_QuickSortLessThanCallback *const IsLessThan,
                                   Dqn_QuickSortSwapCallback *const Swap)
{
	// NOTE: See <template> version for better implementation comments since here we need to use
	// void * for generics in C making it harder in general to comprehend.
	if (!array || size == 0 || size == 1) return;

	DqnRandPCGState state = {};
	DqnRnd_PCGInit(&state);

	u32 pivotIndex     = DqnRnd_PCGRange(&state, 0, size - 1);
	u32 partitionIndex = 0;
	u32 startIndex     = 0;

	{
		void *lastItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, size - 1, itemSize);
		void *pivotItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, pivotIndex, itemSize);
		Swap(lastItemPtr, pivotItemPtr);
		pivotIndex = size - 1;
	}

	void *startItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, startIndex, itemSize);
	void *pivotItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, pivotIndex, itemSize);
	if (IsLessThan(startItemPtr, pivotItemPtr)) partitionIndex++;
	startIndex++;

	for (u32 checkIndex = startIndex; checkIndex < size; checkIndex++)
	{
		void *checkItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, checkIndex, itemSize);
		if (IsLessThan(checkItemPtr, pivotItemPtr))
		{
			void *partitionItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, partitionIndex, itemSize);
			Swap(partitionItemPtr, checkItemPtr);
			partitionIndex++;
		}
	}

	void *partitionItemPtr = DqnInternal_QuickSortGetArrayItemPtr(array, partitionIndex, itemSize);
	Swap(partitionItemPtr, pivotItemPtr);

	Dqn_QuickSortC(array, itemSize, partitionIndex, IsLessThan, Swap);

	u32 oneAfterPartitionIndex = partitionIndex + 1;
	void *oneAfterPartionIndexPtr = DqnInternal_QuickSortGetArrayItemPtr(array, oneAfterPartitionIndex, itemSize);
	Dqn_QuickSortC(oneAfterPartionIndexPtr, itemSize, (size - oneAfterPartitionIndex), IsLessThan, Swap);
}

////////////////////////////////////////////////////////////////////////////////
// #External Code
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// #DqnSprintf Implementation - STB_Sprintf
////////////////////////////////////////////////////////////////////////////////
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

#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wmisleading-indentation"
	#pragma GCC diagnostic ignored "-Wstrict-aliasing"
#endif

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

#ifdef __GNUC__
	#pragma GCC diagnostic pop
#endif

////////////////////////////////////////////////////////////////////////////////
// #DqnIni Implementation - Simple ini-file reader for C/C++.
////////////////////////////////////////////////////////////////////////////////
#ifdef __GNUC__
	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wsign-compare"
#endif

#define INITIAL_CAPACITY (256)

#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#include <stddef.h>

#ifndef DQN_INI_MALLOC
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS
	#include <stdlib.h>
	#define DQN_INI_MALLOC(ctx, size) (malloc(size))
	#define DQN_INI_FREE(ctx, ptr) (free(ptr))
#endif

#ifndef DQN_INI_MEMCPY
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define DQN_INI_MEMCPY(dst, src, cnt) (memcpy(dst, src, cnt))
#endif

#ifndef DQN_INI_STRLEN
	#define _CRT_NONSTDC_NO_DEPRECATE
	#define _CRT_SECURE_NO_WARNINGS
	#include <string.h>
	#define DQN_INI_STRLEN(s) (strlen(s))
#endif

#ifndef DQN_INI_STRICMP
	#ifdef _WIN32
		#define _CRT_NONSTDC_NO_DEPRECATE
		#define _CRT_SECURE_NO_WARNINGS
		#include <string.h>
		#define DQN_INI_STRICMP(s1, s2) ( _stricmp(s1, s2))
	#else
		#include <string.h>
		#define DQN_INI_STRICMP(s1, s2) (strcasecmp(s1, s2))
	#endif
#endif

struct dqn_ini_internal_section_t
{
	char name[32];
	char *name_large;
};

struct dqn_ini_internal_property_t
{
	int section;
	char name[32];
	char *name_large;
	char value[64];
	char *value_large;
};

struct DqnIni
{
	struct dqn_ini_internal_section_t *sections;
	int section_capacity;
	int section_count;

	struct dqn_ini_internal_property_t *properties;
	int property_capacity;
	int property_count;

	void *memctx;
};

static int DqnIni_InternalPropertyIndex(DqnIni const *ini, int section,
                                           int property)
{
	int i;
	int p;

	if (ini && section >= 0 && section < ini->section_count)
	{
		p = 0;
		for (i = 0; i < ini->property_count; ++i)
		{
			if (ini->properties[i].section == section)
			{
				if (p == property) return i;
				++p;
			}
		}
	}

	return DQN_INI_NOT_FOUND;
}

DqnIni *DqnInit_Create(void *memctx)
{
	DqnIni *ini;

	ini           = (DqnIni *)DQN_INI_MALLOC(memctx, sizeof(DqnIni));
	ini->memctx   = memctx;
	ini->sections = (struct dqn_ini_internal_section_t *)DQN_INI_MALLOC(
	    ini->memctx, INITIAL_CAPACITY * sizeof(ini->sections[0]));
	ini->section_capacity       = INITIAL_CAPACITY;
	ini->section_count          = 1; /* global section */
	ini->sections[0].name[0]    = '\0';
	ini->sections[0].name_large = 0;
	ini->properties = (struct dqn_ini_internal_property_t *)DQN_INI_MALLOC(
	    ini->memctx, INITIAL_CAPACITY * sizeof(ini->properties[0]));
	ini->property_capacity = INITIAL_CAPACITY;
	ini->property_count    = 0;
	return ini;
}

DqnIni *DqnIni_Load(char const *data, void *memctx)
{
	DqnIni *ini;
	char const *ptr;
	int s;
	char const *start;
	char const *start2;
	int l;

	ini = DqnInit_Create(memctx);

	ptr = data;
	if (ptr)
	{
		s = 0;
		while (*ptr)
		{
			/* trim leading whitespace */
			while (*ptr && *ptr <= ' ')
				++ptr;

			/* done? */
			if (!*ptr) break;

			/* comment */
			else if (*ptr == ';')
			{
				while (*ptr && *ptr != '\n')
					++ptr;
			}
			/* section */
			else if (*ptr == '[')
			{
				++ptr;
				start = ptr;
				while (*ptr && *ptr != ']' && *ptr != '\n')
					++ptr;

				if (*ptr == ']')
				{
					s = DqnIni_SectionAdd(ini, start, (int)(ptr - start));
					++ptr;
				}
			}
			/* property */
			else
			{
				start = ptr;
				while (*ptr && *ptr != '=' && *ptr != '\n')
					++ptr;

				if (*ptr == '=')
				{
					l = (int)(ptr - start);
					++ptr;
					while (*ptr && *ptr <= ' ' && *ptr != '\n')
						ptr++;
					start2 = ptr;
					while (*ptr && *ptr != '\n')
						++ptr;
					while (*(--ptr) <= ' ')
						(void)ptr;
					ptr++;
					DqnIni_PropertyAdd(ini, s, start, l, start2,
					                     (int)(ptr - start2));
				}
			}
		}
	}

	return ini;
}

int DqnIni_Save(DqnIni const *ini, char *data, int size)
{
	int s;
	int p;
	int i;
	int l;
	char *n;
	int pos;

	if (ini)
	{
		pos = 0;
		for (s = 0; s < ini->section_count; ++s)
		{
			n = ini->sections[s].name_large ? ini->sections[s].name_large
			                                : ini->sections[s].name;
			l = (int)DQN_INI_STRLEN(n);
			if (l > 0)
			{
				if (data && pos < size) data[pos] = '[';
				++pos;
				for (i = 0; i < l; ++i)
				{
					if (data && pos < size) data[pos] = n[i];
					++pos;
				}
				if (data && pos < size) data[pos] = ']';
				++pos;
				if (data && pos < size) data[pos] = '\n';
				++pos;
			}

			for (p = 0; p < ini->property_count; ++p)
			{
				if (ini->properties[p].section == s)
				{
					n = ini->properties[p].name_large
					        ? ini->properties[p].name_large
					        : ini->properties[p].name;
					l = (int)DQN_INI_STRLEN(n);
					for (i = 0; i < l; ++i)
					{
						if (data && pos < size) data[pos] = n[i];
						++pos;
					}
					if (data && pos < size) data[pos] = '=';
					++pos;
					n = ini->properties[p].value_large
					        ? ini->properties[p].value_large
					        : ini->properties[p].value;
					l = (int)DQN_INI_STRLEN(n);
					for (i = 0; i < l; ++i)
					{
						if (data && pos < size) data[pos] = n[i];
						++pos;
					}
					if (data && pos < size) data[pos] = '\n';
					++pos;
				}
			}

			if (pos > 0)
			{
				if (data && pos < size) data[pos] = '\n';
				++pos;
			}
		}

		if (data && pos < size) data[pos] = '\0';
		++pos;

		return pos;
	}

	return 0;
}

void DqnIni_Destroy(DqnIni *ini)
{
	int i;

	if (ini)
	{
		for (i = 0; i < ini->property_count; ++i)
		{
			if (ini->properties[i].value_large)
				DQN_INI_FREE(ini->memctx, ini->properties[i].value_large);
			if (ini->properties[i].name_large)
				DQN_INI_FREE(ini->memctx, ini->properties[i].name_large);
		}
		for (i = 0; i < ini->section_count; ++i)
			if (ini->sections[i].name_large)
				DQN_INI_FREE(ini->memctx, ini->sections[i].name_large);
		DQN_INI_FREE(ini->memctx, ini->properties);
		DQN_INI_FREE(ini->memctx, ini->sections);
		DQN_INI_FREE(ini->memctx, ini);
	}
}

int DqnIni_SectionCount(DqnIni const *ini)
{
	if (ini) return ini->section_count;
	return 0;
}

char const *DqnIni_SectionName(DqnIni const *ini, int section)
{
	if (ini && section >= 0 && section < ini->section_count)
		return ini->sections[section].name_large
		           ? ini->sections[section].name_large
		           : ini->sections[section].name;

	return NULL;
}

int DqnIni_PropertyCount(DqnIni const *ini, int section)
{
	int i;
	int count;

	if (ini)
	{
		count = 0;
		for (i = 0; i < ini->property_count; ++i)
		{
			if (ini->properties[i].section == section) ++count;
		}
		return count;
	}

	return 0;
}

char const *DqnIni_PropertyName(DqnIni const *ini, int section, int property)
{
	int p;

	if (ini && section >= 0 && section < ini->section_count)
	{
		p = DqnIni_InternalPropertyIndex(ini, section, property);
		if (p != DQN_INI_NOT_FOUND)
			return ini->properties[p].name_large ? ini->properties[p].name_large
			                                     : ini->properties[p].name;
	}

	return NULL;
}

char const *DqnIni_PropertyValue(DqnIni const *ini, int section, int property)
{
	int p;

	if (ini && section >= 0 && section < ini->section_count)
	{
		p = DqnIni_InternalPropertyIndex(ini, section, property);
		if (p != DQN_INI_NOT_FOUND)
			return ini->properties[p].value_large
			           ? ini->properties[p].value_large
			           : ini->properties[p].value;
	}

	return NULL;
}

int DqnIni_FindSection(DqnIni const *ini, char const *name, int name_length)
{
	int i;

	if (ini && name)
	{
		if (name_length <= 0) name_length = (int)DQN_INI_STRLEN(name);
		for (i = 0; i < ini->section_count; ++i)
		{
			char const *const other = ini->sections[i].name_large
			                              ? ini->sections[i].name_large
			                              : ini->sections[i].name;
			if ((int)DQN_INI_STRLEN(other) == name_length &&
			    DQN_INI_STRICMP(name, other) == 0)
				return i;
		}
	}

	return DQN_INI_NOT_FOUND;
}

int DqnIni_FindProperty(DqnIni const *ini, int section, char const *name,
                          int name_length)
{
	int i;
	int c;

	if (ini && name && section >= 0 && section < ini->section_count)
	{
		if (name_length <= 0) name_length = (int)DQN_INI_STRLEN(name);
		c                                 = 0;
		for (i = 0; i < ini->property_capacity; ++i)
		{
			if (ini->properties[i].section == section)
			{
				char const *const other = ini->properties[i].name_large
				                              ? ini->properties[i].name_large
				                              : ini->properties[i].name;
				if ((int)DQN_INI_STRLEN(other) == name_length &&
				    DQN_INI_STRICMP(name, other) == 0)
					return c;
				++c;
			}
		}
	}

	return DQN_INI_NOT_FOUND;
}

int DqnIni_SectionAdd(DqnIni *ini, char const *name, int length)
{
	struct dqn_ini_internal_section_t *new_sections;

	if (ini && name)
	{
		if (length <= 0) length = (int)DQN_INI_STRLEN(name);
		if (ini->section_count >= ini->section_capacity)
		{
			ini->section_capacity *= 2;
			new_sections = (struct dqn_ini_internal_section_t *)DQN_INI_MALLOC(
			    ini->memctx, ini->section_capacity * sizeof(ini->sections[0]));
			DQN_INI_MEMCPY(new_sections, ini->sections,
			               ini->section_count * sizeof(ini->sections[0]));
			DQN_INI_FREE(ini->memctx, ini->sections);
			ini->sections = new_sections;
		}

		ini->sections[ini->section_count].name_large = 0;
		if (length + 1 >= sizeof(ini->sections[0].name))
		{
			ini->sections[ini->section_count].name_large =
			    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)length + 1);
			DQN_INI_MEMCPY(ini->sections[ini->section_count].name_large, name,
			               (size_t)length);
			ini->sections[ini->section_count].name_large[length] = '\0';
		}
		else
		{
			DQN_INI_MEMCPY(ini->sections[ini->section_count].name, name,
			               (size_t)length);
			ini->sections[ini->section_count].name[length] = '\0';
		}

		return ini->section_count++;
	}
	return DQN_INI_NOT_FOUND;
}

void DqnIni_PropertyAdd(DqnIni *ini, int section, char const *name,
                          int name_length, char const *value, int value_length)
{
	struct dqn_ini_internal_property_t *new_properties;

	if (ini && name && section >= 0 && section < ini->section_count)
	{
		if (name_length <= 0) name_length   = (int)DQN_INI_STRLEN(name);
		if (value_length <= 0) value_length = (int)DQN_INI_STRLEN(value);

		if (ini->property_count >= ini->property_capacity)
		{

			ini->property_capacity *= 2;
			new_properties =
			    (struct dqn_ini_internal_property_t *)DQN_INI_MALLOC(
			        ini->memctx,
			        ini->property_capacity * sizeof(ini->properties[0]));
			DQN_INI_MEMCPY(new_properties, ini->properties,
			               ini->property_count * sizeof(ini->properties[0]));
			DQN_INI_FREE(ini->memctx, ini->properties);
			ini->properties = new_properties;
		}

		ini->properties[ini->property_count].section     = section;
		ini->properties[ini->property_count].name_large  = 0;
		ini->properties[ini->property_count].value_large = 0;

		if (name_length + 1 >= sizeof(ini->properties[0].name))
		{
			ini->properties[ini->property_count].name_large =
			    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)name_length + 1);
			DQN_INI_MEMCPY(ini->properties[ini->property_count].name_large,
			               name, (size_t)name_length);
			ini->properties[ini->property_count].name_large[name_length] = '\0';
		}
		else
		{
			DQN_INI_MEMCPY(ini->properties[ini->property_count].name, name,
			               (size_t)name_length);
			ini->properties[ini->property_count].name[name_length] = '\0';
		}

		if (value_length + 1 >= sizeof(ini->properties[0].value))
		{
			ini->properties[ini->property_count].value_large =
			    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)value_length + 1);
			DQN_INI_MEMCPY(ini->properties[ini->property_count].value_large,
			               value, (size_t)value_length);
			ini->properties[ini->property_count].value_large[value_length] =
			    '\0';
		}
		else
		{
			DQN_INI_MEMCPY(ini->properties[ini->property_count].value, value,
			               (size_t)value_length);
			ini->properties[ini->property_count].value[value_length] = '\0';
		}

		++ini->property_count;
	}
}

void DqnIni_SectionRemove(DqnIni *ini, int section)
{
	int p;

	if (ini && section >= 0 && section < ini->section_count)
	{
		if (ini->sections[section].name_large)
			DQN_INI_FREE(ini->memctx, ini->sections[section].name_large);
		for (p = ini->property_count - 1; p >= 0; --p)
		{
			if (ini->properties[p].section == section)
			{
				if (ini->properties[p].value_large)
					DQN_INI_FREE(ini->memctx, ini->properties[p].value_large);
				if (ini->properties[p].name_large)
					DQN_INI_FREE(ini->memctx, ini->properties[p].name_large);
				ini->properties[p] = ini->properties[--ini->property_count];
			}
		}

		ini->sections[section] = ini->sections[--ini->section_count];

		for (p = 0; p < ini->property_count; ++p)
		{
			if (ini->properties[p].section == ini->section_count)
				ini->properties[p].section = section;
		}
	}
}

void DqnIni_PropertyRemove(DqnIni *ini, int section, int property)
{
	int p;

	if (ini && section >= 0 && section < ini->section_count)
	{
		p = DqnIni_InternalPropertyIndex(ini, section, property);
		if (p != DQN_INI_NOT_FOUND)
		{
			if (ini->properties[p].value_large)
				DQN_INI_FREE(ini->memctx, ini->properties[p].value_large);
			if (ini->properties[p].name_large)
				DQN_INI_FREE(ini->memctx, ini->properties[p].name_large);
			ini->properties[p] = ini->properties[--ini->property_count];
			return;
		}
	}
}

void DqnIni_SectionNameSet(DqnIni *ini, int section, char const *name,
                              int length)
{
	if (ini && name && section >= 0 && section < ini->section_count)
	{
		if (length <= 0) length = (int)DQN_INI_STRLEN(name);
		if (ini->sections[section].name_large)
			DQN_INI_FREE(ini->memctx, ini->sections[section].name_large);
		ini->sections[section].name_large = 0;

		if (length + 1 >= sizeof(ini->sections[0].name))
		{
			ini->sections[section].name_large =
			    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)length + 1);
			DQN_INI_MEMCPY(ini->sections[section].name_large, name,
			               (size_t)length);
			ini->sections[section].name_large[length] = '\0';
		}
		else
		{
			DQN_INI_MEMCPY(ini->sections[section].name, name, (size_t)length);
			ini->sections[section].name[length] = '\0';
		}
	}
}

void DqnIni_PropertyNameSet(DqnIni *ini, int section, int property,
                               char const *name, int length)
{
	int p;

	if (ini && name && section >= 0 && section < ini->section_count)
	{
		if (length <= 0) length = (int)DQN_INI_STRLEN(name);
		p = DqnIni_InternalPropertyIndex(ini, section, property);
		if (p != DQN_INI_NOT_FOUND)
		{
			if (ini->properties[p].name_large)
				DQN_INI_FREE(ini->memctx, ini->properties[p].name_large);
			ini->properties[ini->property_count].name_large = 0;

			if (length + 1 >= sizeof(ini->properties[0].name))
			{
				ini->properties[p].name_large =
				    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)length + 1);
				DQN_INI_MEMCPY(ini->properties[p].name_large, name,
				               (size_t)length);
				ini->properties[p].name_large[length] = '\0';
			}
			else
			{
				DQN_INI_MEMCPY(ini->properties[p].name, name, (size_t)length);
				ini->properties[p].name[length] = '\0';
			}
		}
	}
}

void DqnIni_PropertyValueSet(DqnIni *ini, int section, int property,
                                char const *value, int length)
{
	int p;

	if (ini && value && section >= 0 && section < ini->section_count)
	{
		if (length <= 0) length = (int)DQN_INI_STRLEN(value);
		p = DqnIni_InternalPropertyIndex(ini, section, property);
		if (p != DQN_INI_NOT_FOUND)
		{
			if (ini->properties[p].value_large)
				DQN_INI_FREE(ini->memctx, ini->properties[p].value_large);
			ini->properties[ini->property_count].value_large = 0;

			if (length + 1 >= sizeof(ini->properties[0].value))
			{
				ini->properties[p].value_large =
				    (char *)DQN_INI_MALLOC(ini->memctx, (size_t)length + 1);
				DQN_INI_MEMCPY(ini->properties[p].name_large, value,
				               (size_t)length);
				ini->properties[p].value_large[length] = '\0';
			}
			else
			{
				DQN_INI_MEMCPY(ini->properties[p].value, value, (size_t)length);
				ini->properties[p].name[length] = '\0';
			}
		}
	}
}

#ifdef __GNUC__
		#pragma GCC diagnostic pop // -Wsign-compare for DQN_INI
#endif
#endif // DQN_IMPLEMENTATION

#if defined(DQN_XPLATFORM_LAYER)
////////////////////////////////////////////////////////////////////////////////
// #XPlatform (Win32 & Unix) Implementation
////////////////////////////////////////////////////////////////////////////////
// Functions in the Cross Platform are guaranteed to be supported in both Unix
// and Win32

#ifdef DQN_UNIX_PLATFORM
	#include <stdio.h>    // Basic File I/O // TODO(doyle): Syscall versions

	#include <dirent.h>   // readdir()/opendir()/closedir()
	#include <sys/stat.h> // file size query
	#include <sys/time.h> // high resolution timer
	#include <time.h>     // timespec
	#include <unistd.h>   // unlink()
#endif

#ifdef DQN_CPP_MODE
////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnFile CPP Implementation
////////////////////////////////////////////////////////////////////////////////
// NOTE(doyle): This is necessary since we supply a default constructor, some uninitialised values
// were put in when using DqnFile file = {};
DqnFile::DqnFile(const bool raiiCleanup)
: permissionFlags(0)
, handle(0)
, size(0)
, raiiCleanup(raiiCleanup)
{
}

DqnFile::~DqnFile()
{
	if (this->raiiCleanup) this->Close();
}

bool DqnFile::Open(const char *const path, const u32 permissionFlags_,
                   const enum DqnFileAction action)
{
	return DqnFile_Open(path, this, permissionFlags_, action);
}

bool DqnFile::OpenW(const wchar_t *const path, const u32 permissionFlags_,
                    const enum DqnFileAction action)
{
	return DqnFile_OpenW(path, this, permissionFlags_, action);
}

size_t DqnFile::Write(u8 *const buffer, const size_t numBytesToWrite, const size_t fileOffset)
{
	return DqnFile_Write(this, buffer, numBytesToWrite, fileOffset);
}

size_t DqnFile::Read(u8 *const buffer, const size_t numBytesToRead)
{
	return DqnFile_Read(this, buffer, numBytesToRead);
}
void DqnFile::Close() { DqnFile_Close(this); }
#endif

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnFileInternal Implementation
////////////////////////////////////////////////////////////////////////////////
#ifdef DQN_WIN32_PLATFORM
FILE_SCOPE bool DqnFileInternal_Win32OpenW(const wchar_t *const path,
                                           DqnFile *const file,
                                           const u32 permissionFlags,
                                           const enum DqnFileAction action)
{
	if (!file || !path) return false;

	DWORD win32Permission = 0;
	if (permissionFlags & DqnFilePermissionFlag_All)
	{
		win32Permission = GENERIC_ALL;
	}
	else
	{
		if (permissionFlags & DqnFilePermissionFlag_Read)    win32Permission |= GENERIC_READ;
		if (permissionFlags & DqnFilePermissionFlag_Write)   win32Permission |= GENERIC_WRITE;
		if (permissionFlags & DqnFilePermissionFlag_Execute) win32Permission |= GENERIC_EXECUTE;
	}

	DWORD win32Action = 0;
	switch (action)
	{
		// Allow fall through
		default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
		case DqnFileAction_OpenOnly:         win32Action = OPEN_EXISTING; break;
		case DqnFileAction_ClearIfExist:     win32Action = TRUNCATE_EXISTING; break;
		case DqnFileAction_CreateIfNotExist: win32Action = CREATE_NEW; break;
	}

	HANDLE handle = CreateFileW(path, win32Permission, 0, NULL, win32Action,
	                            FILE_ATTRIBUTE_NORMAL, NULL);

	if (handle == INVALID_HANDLE_VALUE)
	{
		return false;
	}

	LARGE_INTEGER size;
	if (GetFileSizeEx(handle, &size) == 0)
	{
		DqnFile_Close(file);
		DqnWin32_DisplayLastError("GetFileSizeEx() failed");
		return false;
	}

	file->handle          = handle;
	file->size            = (size_t)size.QuadPart;
	file->permissionFlags = permissionFlags;
	return true;
}

DQN_FILE_SCOPE char **DqnDirInternal_PlatformRead(const char *const dir,
                                                  u32 *const numFiles)
{
	if (!dir || !numFiles) return NULL;

	u32 currNumFiles = 0;
	wchar_t wideDir[MAX_PATH] = {0};
	DqnWin32_UTF8ToWChar(dir, wideDir, DQN_ARRAY_COUNT(wideDir));

	// Enumerate number of files first
	{
		WIN32_FIND_DATAW findData = {0};
		HANDLE findHandle = FindFirstFileW(wideDir, &findData);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			DQN_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
			return NULL;
		}

		bool stayInLoop = true;
		while (stayInLoop)
		{
			BOOL result = FindNextFileW(findHandle, &findData);
			if (result == 0)
			{
				DWORD error = GetLastError();
				if (error != ERROR_NO_MORE_FILES)
				{
					DqnWin32_DisplayErrorCode(error,
					                             "FindNextFileW() failed");
				}

				stayInLoop = false;
			}
			else
			{
				currNumFiles++;
			}
		}
		FindClose(findHandle);
	}

	if (currNumFiles == 0)
	{
		*numFiles = 0;
		return NULL;
	}

	{
		WIN32_FIND_DATAW initFind = {0};
		HANDLE findHandle = FindFirstFileW(wideDir, &initFind);
		if (findHandle == INVALID_HANDLE_VALUE)
		{
			DQN_WIN32_ERROR_BOX("FindFirstFile() failed.", NULL);
			*numFiles = 0;
			return NULL;
		}

		char **list = (char **)DqnMem_Calloc(
		    sizeof(*list) * (currNumFiles));
		if (!list)
		{
			DQN_WIN32_ERROR_BOX("DqnMem_Calloc() failed.", NULL);
			*numFiles = 0;
			return NULL;
		}

		for (u32 i = 0; i < currNumFiles; i++)
		{
			list[i] = (char *)DqnMem_Calloc(sizeof(**list) * MAX_PATH);
			if (!list[i])
			{
				for (u32 j = 0; j < i; j++)
					DqnMem_Free(list[j]);

				DQN_WIN32_ERROR_BOX("DqnMem_Clloc() failed.", NULL);
				*numFiles = 0;
				return NULL;
			}
		}

		i32 listIndex = 0;
		WIN32_FIND_DATAW findData = {0};
		while (FindNextFileW(findHandle, &findData) != 0)
		{
			DqnWin32_WCharToUTF8(findData.cFileName, list[listIndex++],
			                        MAX_PATH);
		}

		*numFiles = currNumFiles;
		FindClose(findHandle);

		return list;
	}
}

#endif // DQN_WIN32_PLATFORM

#ifdef DQN_UNIX_PLATFORM
FILE_SCOPE bool DqnFileInternal_UnixGetFileSizeWithStat(const char *const path, size_t *const size)
{
	struct stat fileStat = {0};
	if (stat(path, &fileStat)) return false;
	*size = fileStat.st_size;

	return true;
}

FILE_SCOPE size_t DqnFileInternal_UnixGetFileSizeManual(FILE *const handle, const bool rewindHandle)
{
	u64 fileSizeInBytes = 0;

	DQN_ASSERT_HARD(handle);
	char c = fgetc(handle);
	while (c != EOF)
	{
		fileSizeInBytes++;
		c = fgetc(handle);
	}
	if (rewindHandle) rewind(handle);
	return fileSizeInBytes;
}

FILE_SCOPE bool DqnFileInternal_UnixOpen(const char *const path,
                                         DqnFile *const file,
                                         const u32 permissionFlags,
                                         const enum DqnFileAction action)
{
	char operation  = 0;
	bool updateFlag = false;

	if (permissionFlags & DqnFilePermissionFlag_Write)
	{
		updateFlag = true;
		switch (action)
		{
			default: DQN_ASSERT(DQN_INVALID_CODE_PATH);
			case DqnFileAction_OpenOnly:
			{
				operation   = 'r';
			}
			break;

			case DqnFileAction_CreateIfNotExist:
			case DqnFileAction_ClearIfExist:
			{
				operation   = 'w';
			}
			break;
		}
	}
	else if ((permissionFlags & DqnFilePermissionFlag_Read) ||
	         (permissionFlags & DqnFilePermissionFlag_Execute))
	{
		if (permissionFlags & DqnFilePermissionFlag_Execute)
		{
			// TODO(doyle): Logging, UNIX doesn't have execute param for file
			// handles. Execution goes through system()
		}
		operation = 'r';
	}
	DQN_ASSERT_HARD(operation != 0);

	// TODO(doyle): What about not reading as a binary file and appending to end
	// of file.
	u32 modeIndex     = 0;
	char mode[4]      = {};
	mode[modeIndex++] = operation;

	if (updateFlag) mode[modeIndex++] = '+';

	mode[modeIndex++] = 'b';
	DQN_ASSERT_HARD(modeIndex <= DQN_ARRAY_COUNT(mode) - 1);

	// TODO(doyle): Use open syscall
	// TODO(doyle): Query errno
	FILE *handle = fopen(path, mode);
	if (!handle) return false;

	if (!DqnFileInternal_UnixGetFileSizeWithStat(path, &file->size))
	{
		// TODO(doyle): Logging
		fclose(handle);
		return false;
	}

	file->handle          = (void *)handle;
	file->permissionFlags = permissionFlags;

	// NOTE: Can occur in some instances where files are generated on demand,
	//       i.e. /proc/cpuinfo. But there can also be zero-byte files, we can't
	//       be sure. So manual check by counting bytes
	if (file->size == 0)
		file->size = DqnFileInternal_UnixGetFileSizeManual((FILE *)file->handle, true);

	return true;
}

DQN_FILE_SCOPE char **DqnDirInternal_PlatformRead(const char *const dir,
                                                  u32 *const numFiles)
{
	if (!dir || !numFiles) return NULL;

	// Enumerate num files
	u32 currNumFiles = 0;
	{
		DIR *const dirHandle = opendir(dir);
		if (!dirHandle) return NULL;

		struct dirent *dirFile = readdir(dirHandle);
		while (dirFile)
		{
			currNumFiles++;
			dirFile = readdir(dirHandle);
		}
		closedir(dirHandle);
	}

	if (currNumFiles == 0)
	{
		*numFiles = 0;
		return NULL;
	}

	// Create file list
	{
		DIR *const dirHandle = opendir(dir);
		if (!dirHandle) return NULL;

		char **list = (char **)DqnMem_Calloc(sizeof(*list) * currNumFiles);
		if (!list)
		{
			// TODO(doyle): Logging
			DQN_ASSERT(DQN_INVALID_CODE_PATH);
			*numFiles = 0;
			return NULL;
		}

		struct dirent *dirFile = readdir(dirHandle);
		for (u32 i = 0; i < currNumFiles; i++)
		{
			list[i] = (char *)DqnMem_Calloc(sizeof(**list) *
			                                DQN_ARRAY_COUNT(dirFile->d_name));
			if (!list[i])
			{
				for (u32 j = 0; j < i; j++) DqnMem_Free(list[j]);

				// TODO(doyle): Logging
				*numFiles = 0;
				return NULL;
			}
		}

		u32 listIndex = 0;
		*numFiles     = currNumFiles;
		while (dirFile)
		{
			DqnStr_Copy(list[listIndex++], dirFile->d_name, DQN_ARRAY_COUNT(dirFile->d_name));
			dirFile = readdir(dirHandle);
		}
		closedir(dirHandle);

		return list;
	}
}

#endif // DQN_UNIX_PLATFORM

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnFile Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE
bool DqnFile_Open(const char *const path, DqnFile *const file,
                  const u32 permissionFlags, const enum DqnFileAction action)
{
	if (!file || !path) return false;

#if defined(DQN_WIN32_PLATFORM)
	// TODO(doyle): MAX PATH is baad
	wchar_t widePath[MAX_PATH] = {0};
	DqnWin32_UTF8ToWChar(path, widePath, DQN_ARRAY_COUNT(widePath));
	return DqnFileInternal_Win32OpenW(widePath, file, permissionFlags, action);

#elif defined(DQN_UNIX_PLATFORM)
	return DqnFileInternal_UnixOpen(path, file, permissionFlags, action);

#else
	DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
	return false;

#endif
}

DQN_FILE_SCOPE
bool DqnFile_OpenW(const wchar_t *const path, DqnFile *const file, const u32 permissionFlags,
                   const enum DqnFileAction action)
{
	if (!file || !path) return false;

#if defined(DQN_WIN32_PLATFORM)
	return DqnFileInternal_Win32OpenW(path, file, permissionFlags, action);
#else
	DQN_ASSERT(DQN_INVALID_CODE_PATH);
	return false;
#endif
}

DQN_FILE_SCOPE size_t DqnFile_Write(const DqnFile *const file,
                                    u8 *const buffer,
                                    const size_t numBytesToWrite,
                                    const size_t fileOffset)
{
	size_t numBytesWritten = 0;
	// TODO(doyle): Implement when it's needed
	if (!DQN_ASSERT_MSG(fileOffset == 0, "'fileOffset' not implemented yet")) return 0;
	if (!file || !buffer) return numBytesToWrite;

#if defined(DQN_WIN32_PLATFORM)
	DWORD bytesToWrite = (DWORD)numBytesToWrite;
	DWORD bytesWritten;
	BOOL result =
	    WriteFile(file->handle, buffer, bytesToWrite, &bytesWritten, NULL);

	numBytesWritten = (size_t)bytesWritten;
	// TODO(doyle): Better logging system
	if (result == 0)
	{
		DQN_WIN32_ERROR_BOX("ReadFile() failed.", NULL);
	}

#elif defined(DQN_UNIX_PLATFORM)
	const size_t ITEMS_TO_WRITE = 1;
	if (fwrite(buffer, numBytesToWrite, ITEMS_TO_WRITE, (FILE *)file->handle) ==
	    ITEMS_TO_WRITE)
	{
		rewind((FILE *)file->handle);
		numBytesWritten = ITEMS_TO_WRITE * numBytesToWrite;
	}
#endif

	return numBytesWritten;
}

DQN_FILE_SCOPE size_t DqnFile_Read(const DqnFile *const file, u8 *const buffer,
                                   const size_t numBytesToRead)
{
	size_t numBytesRead = 0;
	if (file && file->handle && buffer)
	{
#if defined(DQN_WIN32_PLATFORM)
		DWORD bytesToRead = (DWORD)numBytesToRead;
		DWORD bytesRead    = 0;
		HANDLE win32Handle = file->handle;

		BOOL result = ReadFile(win32Handle, (void *)buffer, bytesToRead,
		                       &bytesRead, NULL);

		numBytesRead = (size_t)bytesRead;
		// TODO(doyle): 0 also means it is completing async, but still valid
		if (result == 0)
		{
			DQN_WIN32_ERROR_BOX("ReadFile() failed.", NULL);
		}

#elif defined(DQN_UNIX_PLATFORM)
		// TODO(doyle): Syscall version
		const size_t ITEMS_TO_READ = 1;
		if (fread(buffer, numBytesToRead, ITEMS_TO_READ, (FILE *)file->handle) ==
		    ITEMS_TO_READ)
		{
			rewind((FILE *)file->handle);
			numBytesRead = ITEMS_TO_READ * numBytesToRead;
		}
		else
		{
			// TODO(doyle): Logging, failed read
		}
#else
	DQN_ASSERT_MSG(DQN_INVALID_CODE_PATH, "Non Win32/Unix path not implemented");
#endif
	}
	return numBytesRead;
}

DQN_FILE_SCOPE bool DqnFile_ReadEntireFile(const char *const path, u8 *const buffer,
                                           const size_t bufferSize, size_t *const bytesRead)
{
	if (!path || !buffer) return false;

	size_t bytesReadInternal;
	DqnFile file = {};
	bool result = DqnFile_Open(path, &file, DqnFilePermissionFlag_Read, DqnFileAction_OpenOnly);

	// TODO(doyle): Logging
	if (!result) goto cleanup;

	if (file.size > bufferSize)
	{
		result = false;
		goto cleanup;
	}

	bytesReadInternal = DqnFile_Read(&file, buffer, file.size);
	DQN_ASSERT_HARD(bytesReadInternal == file.size);

	if (bytesRead) *bytesRead = bytesReadInternal;

cleanup:
	DqnFile_Close(&file);
	return result;
}

DQN_FILE_SCOPE void DqnFile_Close(DqnFile *const file)
{
	if (file && file->handle)
	{
#if defined(DQN_WIN32_PLATFORM)
		CloseHandle(file->handle);
#elif defined(DQN_UNIX_PLATFORM)
		fclose((FILE *)file->handle);
#endif
		file->handle          = NULL;
		file->size            = 0;
		file->permissionFlags = 0;
	}
}

#if defined(DQN_WIN32_PLATFORM)
	DQN_COMPILE_ASSERT(sizeof(DWORD)  == sizeof(u32));
#endif

DQN_FILE_SCOPE bool DqnFile_GetFileSizeW(const wchar_t *const path, size_t *const size)
{

#if defined(DQN_WIN32_PLATFORM)
	WIN32_FILE_ATTRIBUTE_DATA attribData = {};
	if (GetFileAttributesExW(path, GetFileExInfoStandard, &attribData))
	{
		// TODO(doyle): What if size_t is < Quad.part?
		LARGE_INTEGER largeInt = {};
		largeInt.HighPart      = attribData.nFileSizeHigh;
		largeInt.LowPart       = attribData.nFileSizeLow;
		*size                  = (size_t)largeInt.QuadPart;
		return true;
	}

#elif defined(DQN_UNIX_PLATFORM)
	// NOTE: Not supported on unix
	DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);

#endif

	return false;
}

DQN_FILE_SCOPE bool DqnFile_GetFileSize(const char *const path, size_t *const size)
{
	if (!path || !size) return false;

	// TODO(doyle): Logging
#if defined(DQN_WIN32_PLATFORM)
	// TODO(doyle): MAX PATH is baad
	wchar_t widePath[MAX_PATH] = {0};
	DqnWin32_UTF8ToWChar(path, widePath, DQN_ARRAY_COUNT(widePath));
	return DqnFile_GetFileSizeW(widePath, size);

#elif defined(DQN_UNIX_PLATFORM)
	// TODO(doyle): Error logging
	if (!DqnFileInternal_UnixGetFileSizeWithStat(path, size)) return false;

	// NOTE: 0 size can occur in some instances where files are generated on demand,
	//       i.e. /proc/cpuinfo
	if (*size == 0)
	{
		// If stat fails, then do a manual byte count
		FILE *handle = fopen(path, "r");
		*size        = DqnFileInternal_UnixGetFileSizeManual(handle, false);
		fclose(handle);
	}

	return true;
#endif
}

DQN_FILE_SCOPE bool DqnFile_Delete(const char *const path)
{
	if (!path) return false;

// TODO(doyle): Logging
#if defined(DQN_WIN32_PLATFORM)
	return DeleteFileA(path);

#elif defined(DQN_UNIX_PLATFORM)
	i32 result = unlink(path);

	if (result == 0) return true;
	return false;

#endif
}

DQN_FILE_SCOPE bool DqnFile_DeleteW(const wchar_t *const path)
{
	if (!path) return false;

	// TODO(doyle): Logging
#if defined(DQN_WIN32_PLATFORM)
	return DeleteFileW(path);

#elif defined(DQN_UNIX_PLATFORM)
	DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
	return false;

#endif
}
////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnDir Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE char **DqnDir_Read(const char *const dir, u32 *const numFiles)
{
	char **result = DqnDirInternal_PlatformRead(dir, numFiles);
	return result;
}

DQN_FILE_SCOPE void DqnDir_ReadFree(char **fileList, u32 numFiles)
{
	if (fileList)
	{
		for (u32 i = 0; i < numFiles; i++)
		{
			if (fileList[i]) DqnMem_Free(fileList[i]);
			fileList[i] = NULL;
		}

		DqnMem_Free(fileList);
	}
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnTimer Implementation
////////////////////////////////////////////////////////////////////////////////
#if defined (DQN_WIN32_PLATFORM)
FILE_SCOPE f64 DqnTimerInternal_Win32QueryPerfCounterTimeInMs()
{
	LOCAL_PERSIST LARGE_INTEGER queryPerformanceFrequency = {0};
	if (queryPerformanceFrequency.QuadPart == 0)
	{
		QueryPerformanceFrequency(&queryPerformanceFrequency);
		DQN_ASSERT_HARD(queryPerformanceFrequency.QuadPart != 0);
	}

	LARGE_INTEGER qpcResult;
	QueryPerformanceCounter(&qpcResult);

	// Convert to microseconds first then divide by ticks per second then to milliseconds
	qpcResult.QuadPart *= 1000000;
	f64 timestamp = qpcResult.QuadPart / (f64)queryPerformanceFrequency.QuadPart;
	timestamp /= 1000.0f;
	return timestamp;
}
#endif

DQN_FILE_SCOPE f64 DqnTimer_NowInMs()
{
	f64 result = 0;
#if defined(DQN_WIN32_PLATFORM)
	result = DQN_MAX(DqnTimerInternal_Win32QueryPerfCounterTimeInMs(), 0);

#elif defined(DQN_UNIX_PLATFORM)
	struct timespec timeSpec = {0};
	if (clock_gettime(CLOCK_MONOTONIC, &timeSpec))
	{
		// TODO(doyle): Failed logging
		DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
	}
	else
	{
		result = (f64)((timeSpec.tv_sec * 1000.0f) + (timeSpec.tv_nsec / 1000000.0f));
	}

#else
	DQN_ASSERT_MSG(DQN_INVALID_CODE_PATH, "Non Unix/Win32 path not implemented yet");

#endif
	return result;
};

DQN_FILE_SCOPE f64 DqnTimer_NowInS() { return DqnTimer_NowInMs() / 1000.0f; }

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnLock Implementation
////////////////////////////////////////////////////////////////////////////////
bool DqnLock_Init(DqnLock *const lock)
{
	if (!lock) return false;

#if defined(DQN_WIN32_PLATFORM)
	if (InitializeCriticalSectionEx(&lock->win32Handle, lock->win32SpinCount, 0))
		return true;

#elif defined(DQN_UNIX_PLATFORM)
	// NOTE: Static initialise, pre-empt a lock so that it gets initialised as per documentation
	lock->unixHandle = PTHREAD_ERRORCHECK_MUTEX_INITIALIZER_NP;
	DqnLock_Acquire(lock);
	DqnLock_Release(lock);
	return true;

#else
	#error Unsupported platform

#endif

	return false;
}

void DqnLock_Acquire(DqnLock *const lock)
{
	if (!lock) return;

#if defined(DQN_WIN32_PLATFORM)
	EnterCriticalSection(&lock->win32Handle);

#elif defined(DQN_UNIX_PLATFORM)
	// TODO(doyle): Better error handling
	i32 error = pthread_mutex_lock(&lock->unixHandle);
	DQN_ASSERT_HARD(error == 0);

#else
	#error Unsupported platform

#endif
}

void DqnLock_Release(DqnLock *const lock)
{
	if (!lock) return;

#if defined(DQN_WIN32_PLATFORM)
	LeaveCriticalSection(&lock->win32Handle);

#elif defined (DQN_UNIX_PLATFORM)
	// TODO(doyle): better error handling
	i32 error = pthread_mutex_unlock(&lock->unixHandle);
	DQN_ASSERT_HARD(error == 0);

#else
	#error Unsupported platform

#endif
}

void DqnLock_Delete(DqnLock *const lock)
{
	if (!lock) return;

#if defined(DQN_WIN32_PLATFORM)
	DeleteCriticalSection(&lock->win32Handle);

#elif defined(DQN_UNIX_PLATFORM)
	i32 error = pthread_mutex_destroy(&lock->unixHandle);
	DQN_ASSERT_HARD(error == 0);

#else
	#error Unsupported platform

#endif
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnLock CPP Implementation
////////////////////////////////////////////////////////////////////////////////
bool DqnLock::Init()    { return DqnLock_Init   (this); }
void DqnLock::Acquire() {        DqnLock_Acquire(this); }
void DqnLock::Release() {        DqnLock_Release(this); }
void DqnLock::Delete()  {        DqnLock_Delete (this); }

DqnLockGuard DqnLock::LockGuard()
{
	return DqnLockGuard(this, NULL);
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnLockGuard CPP Implementation
////////////////////////////////////////////////////////////////////////////////
DqnLockGuard::DqnLockGuard(DqnLock *const lock_, bool *const succeeded)
{
	if (lock_)
	{
		this->lock = lock_;
		this->lock->Acquire();
		if (succeeded) *succeeded = true;
	}
	else
	{
		if (succeeded) *succeeded = false;
	}
}

DqnLockGuard::~DqnLockGuard()
{
	if (this->lock) this->lock->Release();
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnJobQueueInternal Implementation
////////////////////////////////////////////////////////////////////////////////

typedef void *DqnThreadCallbackInternal(void *threadParam);
size_t DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE = 0;

FILE_SCOPE u32 DqnJobQueueInternal_ThreadCreate(const size_t stackSize,
                                                DqnThreadCallbackInternal *const threadCallback,
                                                void *const threadParam, const u32 numThreads)
{
	u32 numThreadsCreated = 0;

#if defined(DQN_WIN32_PLATFORM)
	DQN_ASSERT_HARD(stackSize == 0 || !threadCallback);
	for (u32 i = 0; i < numThreads; i++)
	{
		HANDLE handle = CreateThread(NULL, stackSize, (LPTHREAD_START_ROUTINE)threadCallback,
		                             threadParam, 0, NULL);
		CloseHandle(handle);
		numThreadsCreated++;
	}

#elif defined(DQN_UNIX_PLATFORM)
	// TODO(doyle): Better error handling
	pthread_attr_t attribute = {};
	DQN_ASSERT(pthread_attr_init(&attribute) == 0);

	// Allows us to use pthread_join() which lets us wait till a thread finishes execution
	DQN_ASSERT(pthread_attr_setdetachstate(&attribute, PTHREAD_CREATE_JOINABLE) == 0);

	// TODO(doyle): Persist thread handles
	for (u32 i = 0; i < numThreads; i++)
	{
		pthread_t thread = {};
		pthread_create(&thread, &attribute, threadCallback, threadParam);
		numThreadsCreated++;
	}

	DQN_ASSERT(pthread_attr_destroy(&attribute) == 0);

#else
	#error Unsupported platform

#endif

	DQN_ASSERT(numThreadsCreated == numThreads);
	return numThreadsCreated;
}


FILE_SCOPE void *DqnJobQueueInternal_ThreadCallback(void *threadParam)
{
	DqnJobQueue *queue = (DqnJobQueue *)threadParam;
	for (;;)
	{
		if (!DqnJobQueue_TryExecuteNextJob(queue))
		{
#if defined(DQN_WIN32_PLATFORM)
			WaitForSingleObjectEx(queue->semaphore, INFINITE, false);

#elif defined(DQN_UNIX_PLATFORM)
			sem_wait(&queue->semaphore);

#else
	#error Unsupported platform

#endif
		}
	}
}

FILE_SCOPE bool DqnJobQueueInternal_CreateSemaphore(DqnJobQueue *const queue, const u32 initSignalCount, const u32 maxSignalCount)
{
	if (!queue) return false;

#if defined(DQN_WIN32_PLATFORM)
	queue->semaphore = (void *)CreateSemaphore(NULL, initSignalCount, maxSignalCount, NULL);
	DQN_ASSERT_HARD(queue->semaphore);

#elif defined(DQN_UNIX_PLATFORM)
	// TODO(doyle): Use max count for unix
	// TODO(doyle): Error handling
	const u32 UNIX_DONT_SHARE_BETWEEN_PROCESSES = 0;
	i32 error = sem_init(&queue->semaphore, UNIX_DONT_SHARE_BETWEEN_PROCESSES, 0);
	DQN_ASSERT_HARD(error == 0);

	for (u32 i = 0; i < initSignalCount; i++)
		DQN_ASSERT_HARD(sem_post(&queue->semaphore) == 0);

#else
	#error Unknown platform

#endif

	return true;
}

FILE_SCOPE bool DqnJobQueueInternal_ReleaseSemaphore(DqnJobQueue *const queue)
{
	DQN_ASSERT(queue);

#if defined(DQN_WIN32_PLATFORM)
	DQN_ASSERT(queue->semaphore);
	ReleaseSemaphore(queue->semaphore, 1, NULL);

#elif defined(DQN_UNIX_PLATFORM)
	// TODO(doyle): Error handling
	DQN_ASSERT_HARD(sem_post(&queue->semaphore) == 0);

#else
	#error Unknown platform

#endif
	
	return true;
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnJobQueue Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnJobQueue_Init(DqnJobQueue *const queue, DqnJob *const jobList,
                                     const u32 jobListSize, const u32 numThreads)
{
	if (!queue || !jobList || jobListSize == 0 || numThreads == 0) return false;
	queue->jobList = jobList;
	queue->size    = jobListSize;

	DQN_ASSERT(DqnJobQueueInternal_CreateSemaphore(queue, 0, numThreads));

	// Create threads
	u32 numThreadsCreated = DqnJobQueueInternal_ThreadCreate(
	    DQN_JOB_QUEUE_INTERNAL_THREAD_DEFAULT_STACK_SIZE, DqnJobQueueInternal_ThreadCallback,
	    (void *)queue, numThreads);
	DQN_ASSERT_HARD(numThreads == numThreadsCreated);

	return true;
}

DQN_FILE_SCOPE bool DqnJobQueue_AddJob(DqnJobQueue *const queue, const DqnJob job)
{
	i32 newJobInsertIndex = (queue->jobInsertIndex + 1) % queue->size;
	if (newJobInsertIndex == queue->jobToExecuteIndex) return false;

	queue->jobList[queue->jobInsertIndex] = job;

	DqnAtomic_Add32(&queue->numJobsToComplete, 1);
	DQN_ASSERT(DqnJobQueueInternal_ReleaseSemaphore(queue));

	queue->jobInsertIndex = newJobInsertIndex;
	return true;
}

DQN_FILE_SCOPE void DqnJobQueue_BlockAndCompleteAllJobs(DqnJobQueue *const queue)
{
	while (DqnJobQueue_TryExecuteNextJob(queue) || !DqnJobQueue_AllJobsComplete(queue))
		;
}

DQN_FILE_SCOPE bool DqnJobQueue_TryExecuteNextJob(DqnJobQueue *const queue)
{
	if (!queue) return false;

	i32 originalJobToExecute = queue->jobToExecuteIndex;
	if (originalJobToExecute != queue->jobInsertIndex)
	{
		i32 newJobIndexForNextThread = (originalJobToExecute + 1) % queue->size;
		i32 index = DqnAtomic_CompareSwap32(&queue->jobToExecuteIndex, newJobIndexForNextThread,
		                                    originalJobToExecute);

		// NOTE: If we weren't successful at the interlock, another thread has
		// taken the work and we can't know if there's more work or not. So
		// irrespective of that result, return true to let the thread check
		// again for more work.
		if (index == originalJobToExecute)
		{
			DqnJob job = queue->jobList[index];
			job.callback(queue, job.userData);
			DqnAtomic_Add32(&queue->numJobsToComplete, -1);
		}

		return true;
	}

	return false;
}

DQN_FILE_SCOPE bool DqnJobQueue_AllJobsComplete(DqnJobQueue *const queue)
{
	if (!queue) return false;

	bool result = (queue->numJobsToComplete == 0);
	return result;
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnJobQueue CPP Implementation
////////////////////////////////////////////////////////////////////////////////
bool DqnJobQueue::Init(DqnJob *const jobList_, const u32 jobListSize, const u32 numThreads)
{
	bool result = DqnJobQueue_Init(this, jobList_, jobListSize, numThreads);
	return result;
}

bool DqnJobQueue::AddJob           (const DqnJob job) { return DqnJobQueue_AddJob(this, job);             }
void DqnJobQueue::BlockAndCompleteAllJobs()           {        DqnJobQueue_BlockAndCompleteAllJobs(this); }
bool DqnJobQueue::TryExecuteNextJob()                 { return DqnJobQueue_TryExecuteNextJob(this);       }
bool DqnJobQueue::AllJobsComplete  ()                 { return DqnJobQueue_AllJobsComplete(this);         }

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnAtomic Implementation
////////////////////////////////////////////////////////////////////////////////

#if defined(DQN_WIN32_PLATFORM)
	DQN_COMPILE_ASSERT(sizeof(LONG) == sizeof(i32));
#endif

DQN_FILE_SCOPE i32 DqnAtomic_CompareSwap32(i32 volatile *const dest, const i32 swapVal,
                                           const i32 compareVal)
{
	i32 result = 0;
#if defined(DQN_WIN32_PLATFORM)
	result = (i32)InterlockedCompareExchange((LONG volatile *)dest, (LONG)swapVal, (LONG)compareVal);

#elif defined(DQN_UNIX_PLATFORM)
	result = __sync_val_compare_and_swap(dest, compareVal, swapVal);

#else
	#error Unsupported platform

#endif
	return result;
}

DQN_FILE_SCOPE i32 DqnAtomic_Add32(i32 volatile *const src, const i32 value)
{
	i32 result = 0;
#if defined(DQN_WIN32_PLATFORM)
	result = (i32)InterlockedAdd((LONG volatile *)src, value);

#elif defined(DQN_UNIX_PLATFORM)
	result = __sync_add_and_fetch(src, value);

#else
	#error Unsupported platform

#endif

	return result;
}

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnPlatformInternal Implementation
////////////////////////////////////////////////////////////////////////////////
#if defined(DQN_UNIX_PLATFORM)
FILE_SCOPE void DqnPlatformInternal_UnixGetNumCoresAndThreads(u32 *const numCores,
                                                              u32 *const numThreadsPerCore)
{
	if (!numThreadsPerCore && !numCores) return;

	// TODO(doyle): Not exactly standard
	DqnFile file = {};
	DQN_ASSERT_HARD(
	    DqnFile_Open("/proc/cpuinfo", &file, DqnFilePermissionFlag_Read, DqnFileAction_OpenOnly));
	DQN_ASSERT_HARD(file.size > 0);

	u8 *readBuffer = (u8 *)DqnMem_Calloc(file.size);
	if (readBuffer)
	{
		size_t bytesRead = DqnFile_Read(&file, readBuffer, file.size);
		DQN_ASSERT_HARD(bytesRead == file.size);

		const char *srcPtr = (char *)readBuffer;
		u32 srcLen   = file.size;

#define DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(ptr, len, offset)                                     \
	ptr += offset;                                                                                 \
	len -= offset

		if (numThreadsPerCore)
		{
			*numThreadsPerCore = 0;
			// Find the offset to the processor field and move to it
			const char processorStr[] = "processor";
			i32 processorOffset       = DqnStr_FindFirstOccurence(srcPtr, srcLen, processorStr,
			                                                DQN_ARRAY_COUNT(processorStr));
			DQN_ASSERT_HARD(processorOffset != -1);
			DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, processorOffset);

			// Find the offset to the colon delimiter and advance to 1 after it
			i32 colonOffset = DqnStr_FindFirstOccurence(srcPtr, srcLen, ":", 1) + 1;
			DQN_ASSERT_HARD(colonOffset != -1);
			DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, colonOffset);

			// Read num processors, i.e. logical cores/hyper threads
			*numThreadsPerCore = Dqn_StrToI64(srcPtr, srcLen);
			if (*numThreadsPerCore == 0) *numThreadsPerCore = 1;
		}

		if (numCores)
		{
			*numCores   = 0;
			// Find the offset to the cpu cores field and move to it
			const char cpuCoresStr[] = "cpu cores";
			i32 cpuCoresOffset       = DqnStr_FindFirstOccurence(srcPtr, srcLen, cpuCoresStr,
			                                               DQN_ARRAY_COUNT(cpuCoresStr));
			DQN_ASSERT_HARD(cpuCoresOffset != -1);
			DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, cpuCoresOffset);

			// Find the offset to the colon delimiter and advance to 1 after it
			i32 colonOffset = DqnStr_FindFirstOccurence(srcPtr, srcLen, ":", 1) + 1;
			DQN_ASSERT_HARD(colonOffset != -1);
			DQN_ADVANCE_CHAR_PTR_AND_LEN_INTERNAL(srcPtr, srcLen, colonOffset);

			// Read num cores value, i.e. physical cores
			*numCores = Dqn_StrToI64(srcPtr, srcLen);
		}
		DqnFile_Close(&file);
		DqnMem_Free(readBuffer);
	}
	else
	{
		// TODO(doyle): Out of mem
		DQN_ASSERT_HARD(DQN_INVALID_CODE_PATH);
	}
}
#endif // DQN_UNIX_PLATFORM

#if defined(DQN_WIN32_PLATFORM)
FILE_SCOPE void DqnPlatformInternal_Win32GetNumCoresAndThreads(u32 *const numCores,
                                                               u32 *const numThreadsPerCore)
{
	if (numThreadsPerCore)
	{
		SYSTEM_INFO systemInfo;
		GetNativeSystemInfo(&systemInfo);
		*numThreadsPerCore = systemInfo.dwNumberOfProcessors;
	}

	if (numCores)
	{
		*numCores = 0;
		DWORD requiredSize    = 0;
		u8 insufficientBuffer = {0};
		GetLogicalProcessorInformationEx(
		    RelationProcessorCore, (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)insufficientBuffer,
		    &requiredSize);

		u8 *rawProcInfoArray = (u8 *)DqnMem_Calloc(requiredSize);
		if (!DQN_ASSERT_MSG(rawProcInfoArray, "Calloc failed, could not allocate memory"))
		{
			return;
		}

		if (GetLogicalProcessorInformationEx(
		        RelationProcessorCore, (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)rawProcInfoArray,
		        &requiredSize))
		{
			SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *logicalProcInfo =
			    (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)rawProcInfoArray;
			DWORD bytesRead = 0;

			do
			{
				// NOTE: High efficiency value has greater performance and less efficiency.
				PROCESSOR_RELATIONSHIP *procInfo = &logicalProcInfo->Processor;
				// u32 efficiency                   = procInfo->EfficiencyClass;
				(*numCores)++;
				DQN_ASSERT_HARD(logicalProcInfo->Relationship == RelationProcessorCore);
				DQN_ASSERT_HARD(procInfo->GroupCount == 1);

				bytesRead += logicalProcInfo->Size;
				logicalProcInfo =
				    (SYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX *)((u8 *)logicalProcInfo +
				                                                logicalProcInfo->Size);
			} while (bytesRead < requiredSize);
		}
		else
		{
			DqnWin32_DisplayLastError("GetLogicalProcessorInformationEx() failed");
		}

		DqnMem_Free(rawProcInfoArray);
	}
}
#endif // DQN_WIN32_PLATFORM

////////////////////////////////////////////////////////////////////////////////
// XPlatform > #DqnPlatform Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE void DqnPlatform_GetNumThreadsAndCores(u32 *const numCores, u32 *const numThreadsPerCore)
{
#if defined(DQN_WIN32_PLATFORM)
	DqnPlatformInternal_Win32GetNumCoresAndThreads(numCores, numThreadsPerCore);

#elif defined(DQN_UNIX_PLATFORM)
	DqnPlatformInternal_UnixGetNumCoresAndThreads(numCores, numThreadsPerCore);

#else
	#error Unsupported platform

#endif
}
#endif // DQN_XPLATFORM_LAYER

////////////////////////////////////////////////////////////////////////////////
// #Platform Implementation
////////////////////////////////////////////////////////////////////////////////

#ifdef DQN_WIN32_PLATFORM
////////////////////////////////////////////////////////////////////////////////
// #Win32Platform Implementation
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
// Win32Platform > #DqnWin32 Implementation
////////////////////////////////////////////////////////////////////////////////
DQN_FILE_SCOPE bool DqnWin32_UTF8ToWChar(const char *const in,
                                         wchar_t *const out, const i32 outLen)
{
	u32 result = MultiByteToWideChar(CP_UTF8, 0, in, -1, out, outLen-1);

	if (result == 0xFFFD || 0)
	{
		DQN_WIN32_ERROR_BOX("WideCharToMultiByte() failed.", NULL);
		return false;
	}

	return true;
}

DQN_FILE_SCOPE bool DqnWin32_WCharToUTF8(const wchar_t *const in,
                                         char *const out, const i32 outLen)
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

DQN_FILE_SCOPE void DqnWin32_GetClientDim(const HWND window, LONG *const width, LONG *const height)
{
	RECT rect;
	GetClientRect(window, &rect);
	if (width)  *width  = rect.right - rect.left;
	if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE void DqnWin32_GetRectDim(const RECT rect, LONG *const width, LONG *const height)
{
	if (width)  *width  = rect.right - rect.left;
	if (height) *height = rect.bottom - rect.top;
}

DQN_FILE_SCOPE void DqnWin32_DisplayLastError(const char *const errorPrefix)
{
	DWORD error         = GetLastError();
	char errorMsg[1024] = {0};
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, error, 0, errorMsg, DQN_ARRAY_COUNT(errorMsg), NULL);

	if (errorPrefix)
	{
		char formattedError[2048] = {0};
		Dqn_sprintf(formattedError, "%s: %s", errorPrefix, errorMsg);
		DQN_WIN32_ERROR_BOX(formattedError, NULL);
	}
	else
	{
		DQN_WIN32_ERROR_BOX(errorMsg, NULL);
	}
}

const i32 DQN_WIN32_INTERNAL_ERROR_MSG_SIZE = 2048;
DQN_FILE_SCOPE void DqnWin32_DisplayErrorCode(const DWORD error, const char *const errorPrefix)
{
	char errorMsg[DQN_WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};
	FormatMessageA(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	               NULL, error, 0, errorMsg, DQN_ARRAY_COUNT(errorMsg), NULL);

	char formattedError[2048] = {0};
	Dqn_sprintf(formattedError, "%s: %s", errorPrefix, errorMsg);
	DQN_WIN32_ERROR_BOX(formattedError, NULL);
}

DQN_FILE_SCOPE void DqnWin32_OutputDebugString(const char *const formatStr, ...)
{
	char str[DQN_WIN32_INTERNAL_ERROR_MSG_SIZE] = {0};

	va_list argList;
	va_start(argList, formatStr);
	{
		i32 numCopied = Dqn_vsprintf(str, formatStr, argList);
		DQN_ASSERT_HARD(numCopied < DQN_ARRAY_COUNT(str));
	}
	va_end(argList);

	OutputDebugStringA(str);
}

DQN_FILE_SCOPE i32 DqnWin32_GetEXEDirectory(char *const buf, const u32 bufLen)
{
	if (!buf || bufLen == 0) return 0;
	u32 copiedLen = GetModuleFileNameA(NULL, buf, bufLen);
	if (copiedLen == bufLen) return -1;

	// NOTE: Should always work if GetModuleFileName works and we're running an
	// executable.
	i32 lastSlashIndex = 0;
	for (i32 i = copiedLen; i > 0; i--)
	{
		if (buf[i] == '\\')
		{
			lastSlashIndex = i;
			break;
		}
	}

	return lastSlashIndex;
}

#endif // DQN_WIN32_PLATFORM
