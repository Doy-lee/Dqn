// -------------------------------------------------------------------------------------------------
//
// NOTE: Typedefs, Macros, Utils
//
// -------------------------------------------------------------------------------------------------
constexpr Dqn_usize                       Dqn_ArrayCount(T const (&)[N]);
constexpr Dqn_isize                       Dqn_ArrayCountI(T const (&)[N]);
constexpr Dqn_usize                       Dqn_CharCount(char const (&)[N]);
constexpr Dqn_isize                       Dqn_CharCountI(char const (&)[N]);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Logging
//
// -------------------------------------------------------------------------------------------------
constexpr inline char const *             Dqn_LogTypeTag(Dqn_LogType type);
// NOTE: Set the callback to get called whenever a log message has been printed
#define DQN_LOG_CALLBACK(name) void name(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *log_str)
typedef DQN_LOG_CALLBACK(Dqn_LogCallback);
Dqn_LogCallback *Dqn_log_callback;

#define DQN_LOG_E(fmt, ...) Dqn_Log(Dqn_LogType::Error,   DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_D(fmt, ...) Dqn_Log(Dqn_LogType::Debug,   DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_W(fmt, ...) Dqn_Log(Dqn_LogType::Warning, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_I(fmt, ...) Dqn_Log(Dqn_LogType::Info,    DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG_M(fmt, ...) Dqn_Log(Dqn_LogType::Memory, DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)
#define DQN_LOG(log_type, fmt, ...) Dqn_Log(log_type,    DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__, fmt, ## __VA_ARGS__)

// -------------------------------------------------------------------------------------------------
//
// NOTE: Math
//
// -------------------------------------------------------------------------------------------------
union Dqn_V2I
{
  struct { Dqn_i32 x, y; };
  struct { Dqn_i32 w, h; };
  struct { Dqn_i32 min, max; };
  Dqn_i32 e[2];

  Dqn_V2I() = default;
  constexpr Dqn_V2I(Dqn_f32 x_, Dqn_f32 y_): x((Dqn_i32)x_), y((Dqn_i32)y_) {}
  constexpr Dqn_V2I(Dqn_i32 x_, Dqn_i32 y_): x(x_), y(y_) {}
  constexpr Dqn_V2I(Dqn_i32 xy):         x(xy), y(xy) {}

  constexpr bool     operator!=(Dqn_V2I other) const { return !(*this == other);                }
  constexpr bool     operator==(Dqn_V2I other) const { return (x == other.x) && (y == other.y); }
  constexpr bool     operator>=(Dqn_V2I other) const { return (x >= other.x) && (y >= other.y); }
  constexpr bool     operator<=(Dqn_V2I other) const { return (x <= other.x) && (y <= other.y); }
  constexpr bool     operator< (Dqn_V2I other) const { return (x <  other.x) && (y <  other.y); }
  constexpr bool     operator> (Dqn_V2I other) const { return (x >  other.x) && (y >  other.y); }
  constexpr Dqn_V2I  operator- (Dqn_V2I other) const { Dqn_V2I result(x - other.x, y - other.y); return result; }
  constexpr Dqn_V2I  operator+ (Dqn_V2I other) const { Dqn_V2I result(x + other.x, y + other.y); return result; }
  constexpr Dqn_V2I  operator* (Dqn_V2I other) const { Dqn_V2I result(x * other.x, y * other.y); return result; }
  constexpr Dqn_V2I  operator* (Dqn_f32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
  constexpr Dqn_V2I  operator* (Dqn_i32 other) const { Dqn_V2I result(x * other,   y * other);   return result; }
  constexpr Dqn_V2I  operator/ (Dqn_V2I other) const { Dqn_V2I result(x / other.x, y / other.y); return result; }
  constexpr Dqn_V2I  operator/ (Dqn_f32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
  constexpr Dqn_V2I  operator/ (Dqn_i32 other) const { Dqn_V2I result(x / other,   y / other);   return result; }
  constexpr Dqn_V2I &operator*=(Dqn_V2I other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator*=(Dqn_f32 other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator*=(Dqn_i32 other)       { *this = *this * other;                    return *this;  }
  constexpr Dqn_V2I &operator-=(Dqn_V2I other)       { *this = *this - other;                    return *this;  }
  constexpr Dqn_V2I &operator+=(Dqn_V2I other)       { *this = *this + other;                    return *this;  }
};

union Dqn_V2
{
  struct { Dqn_f32 x, y; };
  struct { Dqn_f32 w, h; };
  struct { Dqn_f32 min, max; };
  Dqn_f32 e[2];

  Dqn_V2() = default;
  constexpr Dqn_V2(Dqn_f32 a)         : x(a),        y(a)        {}
  constexpr Dqn_V2(Dqn_i32 a)         : x((Dqn_f32)a),   y((Dqn_f32)a)   {}
  constexpr Dqn_V2(Dqn_f32 x_, Dqn_f32 y_): x(x_),       y(y_)       {}
  constexpr Dqn_V2(Dqn_i32 x_, Dqn_i32 y_): x((Dqn_f32)x_),  y((Dqn_f32)y_)  {}
  constexpr Dqn_V2(Dqn_V2I a)         : x((Dqn_f32)a.x), y((Dqn_f32)a.y) {}

  constexpr bool    operator!=(Dqn_V2  other) const { return !(*this == other);                }
  constexpr bool    operator==(Dqn_V2  other) const { return (x == other.x) && (y == other.y); }
  constexpr bool    operator>=(Dqn_V2  other) const { return (x >= other.x) && (y >= other.y); }
  constexpr bool    operator<=(Dqn_V2  other) const { return (x <= other.x) && (y <= other.y); }
  constexpr bool    operator< (Dqn_V2  other) const { return (x <  other.x) && (y <  other.y); }
  constexpr bool    operator> (Dqn_V2  other) const { return (x >  other.x) && (y >  other.y); }
  constexpr Dqn_V2  operator- (Dqn_V2  other) const { Dqn_V2 result(x - other.x, y - other.y); return result; }
  constexpr Dqn_V2  operator+ (Dqn_V2  other) const { Dqn_V2 result(x + other.x, y + other.y); return result; }
  constexpr Dqn_V2  operator* (Dqn_V2  other) const { Dqn_V2 result(x * other.x, y * other.y); return result; }
  constexpr Dqn_V2  operator* (Dqn_f32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
  constexpr Dqn_V2  operator* (Dqn_i32 other) const { Dqn_V2 result(x * other,   y * other);   return result; }
  constexpr Dqn_V2  operator/ (Dqn_V2  other) const { Dqn_V2 result(x / other.x, y / other.y); return result; }
  constexpr Dqn_V2  operator/ (Dqn_f32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
  constexpr Dqn_V2  operator/ (Dqn_i32 other) const { Dqn_V2 result(x / other,   y / other);   return result; }
  constexpr Dqn_V2 &operator*=(Dqn_V2  other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator*=(Dqn_i32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_V2  other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator/=(Dqn_i32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V2 &operator-=(Dqn_V2  other)       { *this = *this - other;               return *this;  }
  constexpr Dqn_V2 &operator+=(Dqn_V2  other)       { *this = *this + other;               return *this;  }
};

union Dqn_V3
{
  struct { Dqn_f32 x, y, z; };
  struct { Dqn_f32 r, g, b; };
  Dqn_V2  xy;
  Dqn_f32 e[3];

  Dqn_V3() = default;
  constexpr Dqn_V3(Dqn_f32 a)                 : x(a),        y(a),       z(a)        {}
  constexpr Dqn_V3(Dqn_i32 a)                 : x((Dqn_f32)a),   y((Dqn_f32)a),  z((Dqn_f32)a)   {}
  constexpr Dqn_V3(Dqn_f32 x_, Dqn_f32 y_, Dqn_f32 z_): x(x_),       y(y_),      z(z_)       {}
  constexpr Dqn_V3(Dqn_i32 x_, Dqn_i32 y_, Dqn_f32 z_): x((Dqn_f32)x_),  y((Dqn_f32)y_), z((Dqn_f32)z_)  {}
  constexpr Dqn_V3(Dqn_V2  xy, Dqn_f32 z_)        : x(xy.x),     y(xy.y),    z(z_)       {}

  constexpr bool   operator!= (Dqn_V3  other) const { return !(*this == other);                }
  constexpr bool   operator== (Dqn_V3  other) const { return (x == other.x) && (y == other.y) && (z == other.z); }
  constexpr bool   operator>= (Dqn_V3  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z); }
  constexpr bool   operator<= (Dqn_V3  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z); }
  constexpr bool   operator<  (Dqn_V3  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z); }
  constexpr bool   operator>  (Dqn_V3  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z); }
  constexpr Dqn_V3 operator-  (Dqn_V3  other) const { Dqn_V3 result(x - other.x, y - other.y, z - other.z); return result; }
  constexpr Dqn_V3 operator+  (Dqn_V3  other) const { Dqn_V3 result(x + other.x, y + other.y, z + other.z); return result; }
  constexpr Dqn_V3 operator*  (Dqn_V3  other) const { Dqn_V3 result(x * other.x, y * other.y, z * other.z); return result; }
  constexpr Dqn_V3 operator*  (Dqn_f32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
  constexpr Dqn_V3 operator*  (Dqn_i32 other) const { Dqn_V3 result(x * other,   y * other,   z * other);   return result; }
  constexpr Dqn_V3 operator/  (Dqn_V3  other) const { Dqn_V3 result(x / other.x, y / other.y, z / other.z); return result; }
  constexpr Dqn_V3 operator/  (Dqn_f32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
  constexpr Dqn_V3 operator/  (Dqn_i32 other) const { Dqn_V3 result(x / other,   y / other,   z / other);   return result; }
  constexpr Dqn_V3 &operator*=(Dqn_V3  other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator*=(Dqn_f32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator*=(Dqn_i32 other)       { *this = *this * other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_V3  other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_f32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator/=(Dqn_i32 other)       { *this = *this / other;               return *this;  }
  constexpr Dqn_V3 &operator-=(Dqn_V3  other)       { *this = *this - other;               return *this;  }
  constexpr Dqn_V3 &operator+=(Dqn_V3  other)       { *this = *this + other;               return *this;  }
};

union Dqn_V4
{
  struct { Dqn_f32 x, y, z, w; };
  struct { Dqn_f32 r, g, b, a; };
  Dqn_V3 rgb;
  Dqn_f32 e[4];

  Dqn_V4() = default;
  constexpr Dqn_V4(Dqn_f32 xyzw)                      : x(xyzw),    y(xyzw),    z(xyzw),    w(xyzw)    {}
  constexpr Dqn_V4(Dqn_f32 x_, Dqn_f32 y_, Dqn_f32 z_, Dqn_f32 w_): x(x_),      y(y_),      z(z_),      w(w_)      {}
  constexpr Dqn_V4(Dqn_i32 x_, Dqn_i32 y_, Dqn_i32 z_, Dqn_i32 w_): x((Dqn_f32)x_), y((Dqn_f32)y_), z((Dqn_f32)z_), w((Dqn_f32)w_) {}
  constexpr Dqn_V4(Dqn_V3 xyz, Dqn_f32 w_)                : x(xyz.x),  y(xyz.y),   z(xyz.z),   w(w_)       {}

  constexpr bool    operator!=(Dqn_V4  other) const { return !(*this == other);            }
  constexpr bool    operator==(Dqn_V4  other) const { return (x == other.x) && (y == other.y) && (z == other.z) && (w == other.w); }
  constexpr bool    operator>=(Dqn_V4  other) const { return (x >= other.x) && (y >= other.y) && (z >= other.z) && (w >= other.w); }
  constexpr bool    operator<=(Dqn_V4  other) const { return (x <= other.x) && (y <= other.y) && (z <= other.z) && (w <= other.w); }
  constexpr bool    operator< (Dqn_V4  other) const { return (x <  other.x) && (y <  other.y) && (z <  other.z) && (w <  other.w); }
  constexpr bool    operator> (Dqn_V4  other) const { return (x >  other.x) && (y >  other.y) && (z >  other.z) && (w >  other.w); }
  constexpr Dqn_V4  operator- (Dqn_V4  other) const { Dqn_V4 result(x - other.x, y - other.y, z - other.z, w - other.w); return result;  }
  constexpr Dqn_V4  operator+ (Dqn_V4  other) const { Dqn_V4 result(x + other.x, y + other.y, z + other.z, w + other.w); return result;  }
  constexpr Dqn_V4  operator* (Dqn_V4  other) const { Dqn_V4 result(x * other.x, y * other.y, z * other.z, w * other.w); return result;  }
  constexpr Dqn_V4  operator* (Dqn_f32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  constexpr Dqn_V4  operator* (Dqn_i32 other) const { Dqn_V4 result(x * other,   y * other,   z * other,   w * other);   return result;  }
  constexpr Dqn_V4  operator/ (Dqn_f32 other) const { Dqn_V4 result(x / other,   y / other,   z / other,   w / other);   return result;  }
  constexpr Dqn_V4 &operator*=(Dqn_V4  other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator*=(Dqn_f32 other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator*=(Dqn_i32 other)       { *this = *this * other;                                             return *this;   }
  constexpr Dqn_V4 &operator-=(Dqn_V4  other)       { *this = *this - other;                                             return *this;   }
  constexpr Dqn_V4 &operator+=(Dqn_V4  other)       { *this = *this + other;                                             return *this;   }
};

struct Dqn_Rect
{
    Dqn_V2 min, max;
    Dqn_Rect() = default;
    Dqn_Rect(Dqn_V2  min, Dqn_V2 max) : min(min), max(max) {}
    Dqn_Rect(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
};

struct Dqn_RectI32
{
    Dqn_V2I min, max;
    Dqn_RectI32() = default;
    Dqn_RectI32(Dqn_V2I min, Dqn_V2I max) : min(min), max(max) {}
};

union Dqn_Mat4
{
    Dqn_f32 e[16];
    Dqn_V4  row[4];
    Dqn_f32 row_major[4][4];
    Dqn_f32 operator[](Dqn_usize i) const { return e[i]; }
};

template <typename T> int                 Dqn_MemCmpType(T const *ptr1, T const *ptr2);
template <typename T> T *                 Dqn_MemZero(T *src);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Allocator
//
// -------------------------------------------------------------------------------------------------
// NOTE: The default allocator is the heap allocator
enum struct Dqn_Allocator_Type
{
    Heap,          // Malloc, realloc, free
    XHeap,         // Malloc realloc, free, crash on failure
    Arena,
    Custom,
    Null,
};

#define DQN_ALLOCATOR_ALLOCATE_PROC(name) void *name(Dqn_usize size)
#define DQN_ALLOCATOR_REALLOC_PROC(name) void *name(void *old_ptr, Dqn_usize old_size, Dqn_usize new_size)
#define DQN_ALLOCATOR_FREE_PROC(name) void name(void *ptr)
typedef DQN_ALLOCATOR_ALLOCATE_PROC(Dqn_Allocator_AllocateProc);
typedef DQN_ALLOCATOR_REALLOC_PROC(Dqn_Allocator_ReallocProc);
typedef DQN_ALLOCATOR_FREE_PROC(Dqn_Allocator_FreeProc);
struct Dqn_Allocator
{
    Dqn_Allocator_Type type;
    void              *user_context;

    isize              bytes_allocated;
    isize              total_bytes_allocated;

    isize              allocations;
    isize              total_allocations;

    // NOTE: Only required if type == Dqn_Allocator_Type::Custom
    Dqn_Allocator_AllocateProc *allocate;
    Dqn_Allocator_ReallocProc  *realloc;
    Dqn_Allocator_FreeProc     *free;
};


// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_MemArena
//
// -------------------------------------------------------------------------------------------------
struct Dqn_MemBlock
{
    void         *memory;
    Dqn_usize     size;
    Dqn_usize     used;
    Dqn_MemBlock *prev;
    Dqn_MemBlock *next;
};

struct Dqn_MemArena
{
    // NOTE: Configuration (fill once after "Zero Initialisation {}")
    int           min_block_size = DQN_KILOBYTES(4);
    Dqn_Allocator allocator;

    // NOTE: Read Only
    Dqn_MemBlock *curr_mem_block;
    Dqn_MemBlock *top_mem_block;
    Dqn_usize     highest_used_mark;
    int           total_allocated_mem_blocks;
};

struct Dqn_MemArenaScopedRegion
{
    Dqn_MemArenaScopedRegion(Dqn_MemArena *arena);
    ~Dqn_MemArenaScopedRegion();
    Dqn_MemArena *arena;
    Dqn_MemBlock *curr_mem_block;
    Dqn_usize     curr_mem_block_used;
    Dqn_MemBlock *top_mem_block;
};

#if defined(DQN_DEBUG_DQN_MEM_ARENA_LOGGING)
    #define DQN_DEBUG_ARGS , char const *file, Dqn_isize file_len, char const *func, Dqn_isize func_len, Dqn_isize line
    #define DQN_DEBUG_PARAMS , DQN_STR_AND_LEN(__FILE__), DQN_STR_AND_LEN(__func__), __LINE__
#else
    #define DQN_DEBUG_ARGS
    #define DQN_DEBUG_PARAMS
#endif

#define DQN_MEM_ARENA_INIT_WITH_ALLOCATOR(allocator, size) Dqn_MemArena_InitWithAllocator(allocator, size DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_INIT_MEMORY(src, size)         Dqn_MemArena_InitMemory(src, size DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_ALLOC(arena, size)             Dqn_MemArena_Alloc(arena, size DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_ALLOC_ARRAY(arena, T, num)     (T *)Dqn_MemArena_Alloc(arena, sizeof(T) * num DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_ALLOC_STRUCT(arena, T)         (T *)Dqn_MemArena_Alloc(arena, sizeof(T) DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_RESERVE(arena, size)           Dqn_MemArena_Reserve(arena, size DQN_DEBUG_PARAMS)
#define DQN_MEM_ARENA_FREE(arena)                    Dqn_MemArena_Free(arena)

// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Allocator
//
// -------------------------------------------------------------------------------------------------
Dqn_Allocator                             inline Dqn_Allocator_Null();
Dqn_Allocator                             inline Dqn_Allocator_Heap();
Dqn_Allocator                             inline Dqn_Allocator_XHeap();
Dqn_Allocator                             inline Dqn_Allocator_Arena(Dqn_MemArena *arena);
// -------------------------------------------------------------------------------------------------
//
// NOTE: String
//
// -------------------------------------------------------------------------------------------------
struct Dqn_String
{
    union {
        // NOTE: To appease GCC, Clang can't assign C string literal to char *
        //       Only UB if you try modify a string originally declared const
        char const *str_;
        char *str;
    };

    Dqn_isize   len;
    char const *begin() const { return str; }
    char const *end  () const { return str + len; }
    char       *begin()       { return str; }
    char       *end  ()       { return str + len; }
};
#define DQN_STRING_LITERAL(string) {string, Dqn_CharCountI(string)}

// -------------------------------------------------------------------------------------------------
//
// NOTE: String Builder
//
// -------------------------------------------------------------------------------------------------
struct Dqn_StringBuilderBuffer
{
    char                    *mem;
    Dqn_usize                size;
    Dqn_usize                used;
    Dqn_StringBuilderBuffer *next;
};

Dqn_usize constexpr DQN_STRING_BUILDER_MIN_MEM_BUF_ALLOC_SIZE = DQN_KILOBYTES(4);
template <Dqn_usize N = DQN_KILOBYTES(16)>
struct Dqn_StringBuilder
{
    Dqn_Allocator            allocator;
    char                     fixed_mem[N];
    Dqn_usize                fixed_mem_used;
    Dqn_StringBuilderBuffer *next_mem_buf;
    Dqn_StringBuilderBuffer *last_mem_buf;
    Dqn_isize                string_len;
};

// The necessary length to build the string, it returns the length including the null-terminator
template <Dqn_usize N> Dqn_isize          Dqn_StringBuilder_BuildLen(Dqn_StringBuilder<N> const *builder);
template <Dqn_usize N> void               Dqn_StringBuilder_BuildInBuffer(Dqn_StringBuilder<N> const *builder, char *dest, Dqn_usize dest_size);
template <Dqn_usize N> char *             Dqn_StringBuilder_Build(Dqn_StringBuilder<N> *builder, Dqn_Allocator *allocator, Dqn_isize *len = nullptr);
template <Dqn_usize N> Dqn_String         Dqn_StringBuilder_BuildString(Dqn_StringBuilder<N> *builder, Dqn_Allocator *allocator);
template <Dqn_usize N> void               Dqn_StringBuilder_VFmtAppend(Dqn_StringBuilder<N> *builder, char const *fmt, va_list va);
template <Dqn_usize N> void               Dqn_StringBuilder_FmtAppend(Dqn_StringBuilder<N> *builder, char const *fmt, ...);
template <Dqn_usize N> void               Dqn_StringBuilder_Append(Dqn_StringBuilder<N> *builder, char const *str, Dqn_isize len = -1);
template <Dqn_usize N> void               Dqn_StringBuilder_AppendString(Dqn_StringBuilder<N> *builder, Dqn_String const string);
template <Dqn_usize N> void               Dqn_StringBuilder_AppendChar(Dqn_StringBuilder<N> *builder, char ch);
template <Dqn_usize N> void               Dqn_StringBuilder_Free(Dqn_StringBuilder<N> *builder);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Slices
//
// -------------------------------------------------------------------------------------------------
template <typename T> inline Dqn_Slice<T> Dqn_Slice_CopyNullTerminated(Dqn_MemArena *arena, T const *src, Dqn_isize len);
template <typename T> inline Dqn_Slice<T> Dqn_Slice_CopyNullTerminated(Dqn_MemArena *arena, Dqn_Slice<T> const src);
template <typename T> inline Dqn_Slice<T> Dqn_Slice_Copy(Dqn_MemArena *arena, T const *src, Dqn_isize len);
template <typename T> inline Dqn_Slice<T> Dqn_Slice_Copy(Dqn_MemArena *arena, Dqn_Slice<T> const src);
template <typename T> inline bool         Dqn_Slice_Equals(Dqn_Slice<T> const a, Dqn_Slice<T> const b);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_FixedArray
//
// -------------------------------------------------------------------------------------------------
#define DQN_FIXED_ARRAY_TEMPLATE template <typename T, int MAX_>
#define DQN_FIXED_ARRAY_TEMPLATE_DECL Dqn_FixedArray<T, MAX_>
DQN_FIXED_ARRAY_TEMPLATE struct Dqn_FixedArray
{
    T               data[MAX_];
    Dqn_isize       len;

    T       &operator[] (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= len, "%jd >= 0 && %jd < %jd", i, len); return  data[i]; }
    T       *begin      ()                  { return data; }
    T       *end        ()                  { return data + len; }
    T       *operator+  (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i <= len, "%jd >= 0 && %jd < %jd", i, len); return data + i; }

    T const &operator[] (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= len, "%jd >= 0 && %jd < %jd", i, i, len); return  data[i]; }
    T const *begin      ()            const { return data; }
    T const *end        ()            const { return data + len; }
    T const *operator+  (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i <= len, "%jd >= 0 && %jd < %jd", i, len); return data + i; }
};

int                                       Dqn_FixedArray_Capacity(DQN_FIXED_ARRAY_TEMPLATE_DECL *);
DQN_FIXED_ARRAY_TEMPLATE_DECL             Dqn_FixedArray_Init(T const *item, int num);
T *                                       Dqn_FixedArray_Add(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *items, Dqn_isize num);
T *                                       Dqn_FixedArray_Add(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const item);
T *                                       Dqn_FixedArray_Make(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num);
void                                      Dqn_FixedArray_Clear(DQN_FIXED_ARRAY_TEMPLATE_DECL *a);
void                                      Dqn_FixedArray_EraseStable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index);
void                                      Dqn_FixedArray_EraseUnstable(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize index);
void                                      Dqn_FixedArray_Pop(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, Dqn_isize num = 1);
T *                                       Dqn_FixedArray_Peek(DQN_FIXED_ARRAY_TEMPLATE_DECL *a);
Dqn_isize                                 Dqn_FixedArray_GetIndex(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T const *entry);
T *                                       Dqn_FixedArray_Find(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, EqualityProc IsEqual);
T *                                       Dqn_FixedArray_Find(DQN_FIXED_ARRAY_TEMPLATE_DECL *a, T *find);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Array
//
// -------------------------------------------------------------------------------------------------
// TODO(doyle): Make this either initialised from memory or dynamically allocating
template <typename T> struct Dqn_Array
{
    Dqn_Allocator allocator;
    T            *data;
    Dqn_isize     len;
    Dqn_isize     max;

    T const    operator[](Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return  data[i]; }
    T          operator[](Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return  data[i]; }
    T const   *begin    ()             const { return data; }
    T const   *end      ()             const { return data + len; }
    T         *begin    ()                   { return data; }
    T         *end      ()                   { return data + len; }
    T const   *operator+(Dqn_isize i)  const { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return data + i; }
    T         *operator+(Dqn_isize i)        { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return data + i; }
};

template <typename T> Dqn_Array<T>        Dqn_Array_InitMemory(T *memory, Dqn_isize max, Dqn_isize len = 0);
template <typename T> bool                Dqn_Array_Reserve(Dqn_Array<T> *a, Dqn_isize size);
template <typename T> void                Dqn_Array_Free(Dqn_Array<T> *a);
template <typename T> T *                 Dqn_Array_Add(Dqn_Array<T> *a, T const *items, Dqn_isize num);
template <typename T> T *                 Dqn_Array_Add(Dqn_Array<T> *a, T const item);
template <typename T> T *                 Dqn_Array_Make(Dqn_Array<T> *a, Dqn_isize num);
template <typename T> void                Dqn_Array_Clear(Dqn_Array<T> *a, bool zero_mem = false);
template <typename T> void                Dqn_Array_EraseStable(Dqn_Array<T> *a, Dqn_isize index);
template <typename T> void                Dqn_Array_EraseUnstable(Dqn_Array<T> *a, Dqn_isize index);
template <typename T> void                Dqn_Array_Pop(Dqn_Array<T> *a, Dqn_isize num);
template <typename T> T *                 Dqn_Array_Peek(Dqn_Array<T> *a);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_FixedString
//
// -------------------------------------------------------------------------------------------------
template <Dqn_isize MAX_>
struct Dqn_FixedString
{
    union { char data[MAX_]; char str[MAX_]; char buf[MAX_]; };
    Dqn_isize len;

    Dqn_FixedString()                            { data[0] = 0; len = 0; }
    Dqn_FixedString(char const *fmt, ...)
    {
        *this = {};
        va_list va;
        va_start(va, fmt);
        Dqn_FixedString_AppendVFmt(this, fmt, va);
        va_end(va);
    }

    Dqn_b32 operator==(Dqn_FixedString const &other) const
    {
        if (len != other.len) return false;
        bool result = memcmp(data, other.data, len);
        return result;
    }

    Dqn_b32 operator!=(Dqn_FixedString const &other) const { return !(*this == other); }

    char const &operator[]   (Dqn_isize i) const { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return data[i]; }
    char       &operator[]   (Dqn_isize i)       { DQN_ASSERT_MSG(i >= 0 && i < len, "%d >= 0 && %d < %d", i, len); return data[i]; }
    char const *begin        ()        const { return data; }
    char const *end          ()        const { return data + len; }
    char       *begin        ()              { return data; }
    char       *end          ()              { return data + len; }
};

template <Dqn_isize MAX_> int             Dqn_FixedString_Capacity(Dqn_FixedString<MAX_> *);
template <Dqn_isize MAX_> void            Dqn_FixedString_Clear(Dqn_FixedString<MAX_> *str);
template <Dqn_isize MAX_> Dqn_b32         Dqn_FixedString_AppendVFmt(Dqn_FixedString<MAX_> *str, char const *fmt, va_list va);
template <Dqn_isize MAX_> Dqn_b32         Dqn_FixedString_AppendFmt(Dqn_FixedString<MAX_> *str, char const *fmt, ...);
template <Dqn_isize MAX_> Dqn_b32         Dqn_FixedString_Append(Dqn_FixedString<MAX_> *str, char const *src, Dqn_isize len = -1);
template <Dqn_isize MAX_> Dqn_String      Dqn_FixedString_ToString(Dqn_FixedString<MAX_> const *str);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_U64Str
//
// -------------------------------------------------------------------------------------------------
struct Dqn_U64Str
{
    // Points to the start of the str in the buffer, not necessarily buf since
    // we write into the buffer in reverse
    char *str;
    char  buf[27]; // NOTE(doyle): 27 is the maximum size of Dqn_u64 including commas
    int   len;
};

char *                                    Dqn_U64Str_ToStr(Dqn_u64 val, Dqn_U64Str *result, Dqn_b32 comma_sep);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Logging
//
// -------------------------------------------------------------------------------------------------
void                                      Dqn_LogV(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, va_list va);
// return: This returns a boolean as a hack so you can combine it in if expressions. I use it for my IF_ASSERT macro
Dqn_b32                                   Dqn_Log(Dqn_LogType type, char const *file, Dqn_usize file_len, char const *func, Dqn_usize func_len, Dqn_usize line, char const *fmt, ...);
void *                                    Dqn_Allocator_Allocate(Dqn_Allocator *allocator, Dqn_usize size);
void *                                    Dqn_Allocator_Realloc(Dqn_Allocator *allocator, void *old_ptr, Dqn_isize old_size, Dqn_isize new_size);
void                                      Dqn_Allocator_Free(Dqn_Allocator *allocator, void *ptr);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_MemArena
//
// -------------------------------------------------------------------------------------------------
void *                                    Dqn_MemArena_Alloc(Dqn_MemArena *arena, Dqn_usize size DQN_DEBUG_ARGS);
void                                      Dqn_MemArena_Free(Dqn_MemArena *arena DQN_DEBUG_ARGS);
Dqn_b32                                   Dqn_MemArena_Reserve(Dqn_MemArena *arena, Dqn_usize size DQN_DEBUG_ARGS);
Dqn_MemArena                              Dqn_MemArena_InitWithAllocator(Dqn_Allocator allocator, Dqn_usize size DQN_DEBUG_ARGS);
Dqn_MemArena                              Dqn_MemArena_InitMemory(void *memory, Dqn_usize size DQN_DEBUG_ARGS);
void                                      Dqn_MemArena_ResetUsage(Dqn_MemArena *arena, Dqn_ZeroMem zero_mem);
Dqn_MemArenaScopedRegion                  Dqn_MemArena_MakeScopedRegion(Dqn_MemArena *arena);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Asprintf (Allocate Sprintf)
//
// -------------------------------------------------------------------------------------------------
Dqn_String                                Dqn_Asprintf(Dqn_Allocator *allocator, char const *fmt, va_list va);
Dqn_String                                Dqn_Asprintf(Dqn_Allocator *allocator, char const *fmt, ...);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Vectors
//
// -------------------------------------------------------------------------------------------------
Dqn_V2I                                   Dqn_V2_ToV2I(Dqn_V2 a);
Dqn_V2                                    Dqn_V2_Max(Dqn_V2 a, Dqn_V2 b);
Dqn_V2                                    Dqn_V2_Abs(Dqn_V2 a);
Dqn_f32                                   Dqn_V2_Dot(Dqn_V2 a, Dqn_V2 b);
Dqn_f32                                   Dqn_V2_LengthSq(Dqn_V2 a, Dqn_V2 b);
Dqn_V2                                    Dqn_V2_Normalise(Dqn_V2 a);
Dqn_V2                                    Dqn_V2_Perpendicular(Dqn_V2 a);
Dqn_f32                                   Dqn_V4_Dot(Dqn_V4 const *a, Dqn_V4 const *b);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Rect
//
// -------------------------------------------------------------------------------------------------
Dqn_Rect                                  Dqn_Rect_InitFromPosAndSize(Dqn_V2 pos, Dqn_V2 size);
Dqn_V2                                    Dqn_Rect_Center(Dqn_Rect rect);
Dqn_b32                                   Dqn_Rect_ContainsPoint(Dqn_Rect rect, Dqn_V2 p);
Dqn_b32                                   Dqn_Rect_ContainsRect(Dqn_Rect a, Dqn_Rect b);
Dqn_V2                                    Dqn_Rect_Size(Dqn_Rect rect);
Dqn_Rect                                  Dqn_Rect_Move(Dqn_Rect src, Dqn_V2 move_amount);
Dqn_Rect                                  Dqn_Rect_Intersection(Dqn_Rect a, Dqn_Rect b);
Dqn_Rect                                  Dqn_Rect_Union(Dqn_Rect a, Dqn_Rect b);
Dqn_Rect                                  Dqn_Rect_FromRectI32(Dqn_RectI32 a);
Dqn_V2I                                   Dqn_RectI32_Size(Dqn_RectI32 rect);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Math Utils
//
// -------------------------------------------------------------------------------------------------
Dqn_V2                                    Dqn_LerpV2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_Mat4
//
// -------------------------------------------------------------------------------------------------
Dqn_Mat4                                  Dqn_Mat4_Identity();
Dqn_Mat4                                  Dqn_Mat4_Scale3f(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
Dqn_Mat4                                  Dqn_Mat4_ScaleV3(Dqn_V3 vec);
Dqn_Mat4                                  Dqn_Mat4_Translate3f(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
Dqn_Mat4                                  Dqn_Mat4_TranslateV3(Dqn_V3 vec);
Dqn_Mat4                                  operator*(Dqn_Mat4 const &a, Dqn_Mat4 const &b);
Dqn_V4                                    operator*(Dqn_Mat4 const &mat, Dqn_V4 const &vec);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Helper Functions
//
// -------------------------------------------------------------------------------------------------
void                                      Dqn_Bit_UnsetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
void                                      Dqn_Bit_SetInplace(Dqn_u32 *flags, Dqn_u32 bitfield);
Dqn_b32                                   Dqn_Bit_IsSet(Dqn_u32 flags, Dqn_u32 bitfield);
Dqn_b32                                   Dqn_Bit_IsNotSet(Dqn_u32 flags, Dqn_u32 bitfield);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Safe Arithmetic
//
// -------------------------------------------------------------------------------------------------
Dqn_i64                                   Dqn_Safe_AddI64(Dqn_i64 a, Dqn_i64 b);
Dqn_i64                                   Dqn_Safe_MulI64(Dqn_i64 a, Dqn_i64 b);
Dqn_u64                                   Dqn_Safe_AddU64(Dqn_u64 a, Dqn_u64 b);
Dqn_u64                                   Dqn_Safe_MulU64(Dqn_u64 a, Dqn_u64 b);
int                                       Dqn_Safe_TruncateISizeToInt(Dqn_isize val);
Dqn_i32                                   Dqn_Safe_TruncateISizeToI32(Dqn_isize val);
Dqn_i8                                    Dqn_Safe_TruncateISizeToI8(Dqn_isize val);
Dqn_u32                                   Dqn_Safe_TruncateUSizeToU32(Dqn_u64 val);
int                                       Dqn_Safe_TruncateUSizeToI32(Dqn_usize val);
int                                       Dqn_Safe_TruncateUSizeToInt(Dqn_usize val);
Dqn_isize                                 Dqn_Safe_TruncateUSizeToISize(Dqn_usize val);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Char Helpers
//
// -------------------------------------------------------------------------------------------------
Dqn_b32                                   Dqn_Char_IsAlpha(char ch);
Dqn_b32                                   Dqn_Char_IsDigit(char ch);
Dqn_b32                                   Dqn_Char_IsAlphaNum(char ch);
Dqn_b32                                   Dqn_Char_IsWhitespace(char ch);
char                                      Dqn_Char_ToLower(char ch);
// -------------------------------------------------------------------------------------------------
//
// NOTE: String Helpers
//
// -------------------------------------------------------------------------------------------------
Dqn_b32                                   Dqn_Str_Equals(char const *a, char const *b, Dqn_isize a_len = -1, Dqn_isize b_len = -1);
char const *                              Dqn_Str_FindMulti(char const *buf, char const *find_list[], Dqn_isize const *find_string_lens, Dqn_isize find_len, Dqn_isize *match_index, Dqn_isize buf_len = -1);
char const *                              Dqn_Str_Find(char const *buf, char const *find, Dqn_isize buf_len = -1, Dqn_isize find_len = -1);
Dqn_b32                                   Dqn_Str_Match(char const *src, char const *find, int find_len);
char const *                              Dqn_Str_SkipToChar(char const *src, char ch);
char const *                              Dqn_Str_SkipToNextAlphaNum(char const *src);
char const *                              Dqn_Str_SkipToNextDigit(char const *src);
char const *                              Dqn_Str_SkipToNextChar(char const *src);
char const *                              Dqn_Str_SkipToNextWord(char const *src);
char const *                              Dqn_Str_SkipToNextWhitespace(char const *src);
char const *                              Dqn_Str_SkipWhitespace(char const *src);
char const *                              Dqn_Str_SkipToCharInPlace(char const **src, char ch);
char const *                              Dqn_Str_SkipToNextAlphaNumInPlace(char const **src);
char const *                              Dqn_Str_SkipToNextCharInPlace(char const **src);
char const *                              Dqn_Str_SkipToNextWhitespaceInPlace(char const **src);
char const *                              Dqn_Str_SkipToNextWordInPlace(char const **src);
char const *                              Dqn_Str_SkipWhitespaceInPlace(char const **src);
Dqn_u64                                   Dqn_Str_ToU64(char const *buf, int len = -1);
Dqn_i64                                   Dqn_Str_ToI64(char const *buf, int len = -1);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Dqn_String
//
// -------------------------------------------------------------------------------------------------
Dqn_b32                                   Dqn_String_Compare(Dqn_String const lhs, Dqn_String const rhs);
Dqn_b32                                   Dqn_String_CompareCaseInsensitive(Dqn_String const lhs, Dqn_String const rhs);
Dqn_String                                Dqn_String_Copy(Dqn_Allocator *allocator, Dqn_String const src);
// -------------------------------------------------------------------------------------------------
//
// NOTE: File
//
// -------------------------------------------------------------------------------------------------
char *                                    Dqn_File_ReadAll(Dqn_Allocator *allocator, char const *file, Dqn_isize *file_size);
// -------------------------------------------------------------------------------------------------
//
// NOTE: Utils
//
// -------------------------------------------------------------------------------------------------
char *                                    Dqn_EpochTimeToDate(Dqn_i64 timestamp, char *buf, Dqn_isize buf_len);
