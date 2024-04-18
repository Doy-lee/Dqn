#pragma once
#include "dqn.h"

/*
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//   $$\      $$\  $$$$$$\ $$$$$$$$\ $$\   $$\
//   $$$\    $$$ |$$  __$$\\__$$  __|$$ |  $$ |
//   $$$$\  $$$$ |$$ /  $$ |  $$ |   $$ |  $$ |
//   $$\$$\$$ $$ |$$$$$$$$ |  $$ |   $$$$$$$$ |
//   $$ \$$$  $$ |$$  __$$ |  $$ |   $$  __$$ |
//   $$ |\$  /$$ |$$ |  $$ |  $$ |   $$ |  $$ |
//   $$ | \_/ $$ |$$ |  $$ |  $$ |   $$ |  $$ |
//   \__|     \__|\__|  \__|  \__|   \__|  \__|
//
//   dqn_math.h -- Basic math functions
//
////////////////////////////////////////////////////////////////////////////////////////////////////
//
// [$VEC2] Dqn_V2, V2i -- DQN_V2
// [$VEC3] Dqn_V3, V3i -- DQN_V3
// [$VEC4] Dqn_V4, V4i -- DQN_V4
// [$MAT4] Dqn_M4      -- DQN_M4
// [$M2x3] Dqn_M2x3    --
// [$RECT] Dqn_Rect    -- DQN_RECT
// [$MATH] Other       --
//
////////////////////////////////////////////////////////////////////////////////////////////////////
*/

DQN_MSVC_WARNING_PUSH
DQN_MSVC_WARNING_DISABLE(4201) // warning C4201: nonstandard extension used: nameless struct/union
#if !defined(DQN_NO_V2)
// NOTE: [$VEC2] Vector2 ///////////////////////////////////////////////////////////////////////////
union Dqn_V2I
{
    struct { int32_t x, y; };
    struct { int32_t w, h; };
    int32_t data[2];
};

union Dqn_V2U16
{
    struct { uint16_t x, y; };
    struct { uint16_t w, h; };
    uint16_t data[2];
};

union Dqn_V2
{
    struct { Dqn_f32 x, y; };
    struct { Dqn_f32 w, h; };
    Dqn_f32 data[2];
};
#endif // !defined(DQN_NO_V2)

#if !defined(DQN_NO_V3)
// NOTE: [$VEC3] Vector3 ///////////////////////////////////////////////////////////////////////////
union Dqn_V3
{
    struct { Dqn_f32 x, y, z; };
    struct { Dqn_f32 r, g, b; };
    Dqn_f32 data[3];
};

#endif // !defined(DQN_NO_V3)

#if !defined(DQN_NO_V4)
// NOTE: [$VEC4] Vector4 ///////////////////////////////////////////////////////////////////////////
union Dqn_V4
{
    struct { Dqn_f32 x, y, z, w; };
    struct { Dqn_f32 r, g, b, a; };
    #if !defined(DQN_NO_V3)
    Dqn_V3  rgb;
    Dqn_V3  xyz;
    #endif
    Dqn_f32 data[4];
};
#endif // !defined(DQN_NO_V4)
DQN_MSVC_WARNING_POP

#if !defined(DQN_NO_M4)
// NOTE: [$MAT4] Dqn_M4 ////////////////////////////////////////////////////////////////////////////
struct Dqn_M4
{
    Dqn_f32 columns[4][4]; // Column major matrix
};
#endif // !defined(DQN_M4)

// NOTE: [$M2x3] Dqn_M2x3 //////////////////////////////////////////////////////////////////////////
union Dqn_M2x3
{
    Dqn_f32 e[6];
    Dqn_f32 row[2][3];
};

// NOTE: [$RECT] Dqn_Rect //////////////////////////////////////////////////////////////////////////
#if !defined(DQN_NO_RECT)
#if defined(DQN_NO_V2)
    #error "Rectangles requires V2, DQN_NO_V2 must not be defined"
#endif
struct Dqn_Rect
{
    Dqn_V2 pos, size;
};

struct Dqn_RectMinMax
{
    Dqn_V2 min, max;
};

enum Dqn_RectCutClip
{
    Dqn_RectCutClip_No,
    Dqn_RectCutClip_Yes,
};

enum Dqn_RectCutSide
{
    Dqn_RectCutSide_Left,
    Dqn_RectCutSide_Right,
    Dqn_RectCutSide_Top,
    Dqn_RectCutSide_Bottom,
};

struct Dqn_RectCut
{
    Dqn_Rect*       rect;
    Dqn_RectCutSide side;
};
#endif // !defined(DQN_NO_RECT)

// NOTE: [$MATH] Other /////////////////////////////////////////////////////////////////////////////
// NOTE: API
struct Dqn_RaycastLineIntersectV2Result
{
    bool    hit; // True if there was an intersection, false if the lines are parallel
    Dqn_f32 t_a; // Distance along `dir_a` that the intersection occurred, e.g. `origin_a + (dir_a * t_a)`
    Dqn_f32 t_b; // Distance along `dir_b` that the intersection occurred, e.g. `origin_b + (dir_b * t_b)`
};

#if !defined(DQN_NO_V2)
// NOTE: [$VEC2] Vector2 ///////////////////////////////////////////////////////////////////////////
#define                Dqn_V2I_Zero                 DQN_LITERAL(Dqn_V2I){{(int32_t)(0),    (int32_t)(0)}}
#define                Dqn_V2I_One                  DQN_LITERAL(Dqn_V2I){{(int32_t)(1),    (int32_t)(1)}}
#define                Dqn_V2I_InitNx1(x)           DQN_LITERAL(Dqn_V2I){{(int32_t)(x),    (int32_t)(x)}}
#define                Dqn_V2I_InitNx2(x, y)        DQN_LITERAL(Dqn_V2I){{(int32_t)(x),    (int32_t)(y)}}
#define                Dqn_V2I_InitV2(xy)           DQN_LITERAL(Dqn_V2I){{(int32_t)(xy).x, (int32_t)(xy).y}}

DQN_API bool           operator!=                   (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API bool           operator==                   (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API bool           operator>=                   (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API bool           operator<=                   (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API bool           operator<                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API bool           operator>                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I        operator-                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I        operator-                    (Dqn_V2I  lhs);
DQN_API Dqn_V2I        operator+                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I        operator*                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I        operator*                    (Dqn_V2I  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2I        operator*                    (Dqn_V2I  lhs, int32_t rhs);
DQN_API Dqn_V2I        operator/                    (Dqn_V2I  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I        operator/                    (Dqn_V2I  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2I        operator/                    (Dqn_V2I  lhs, int32_t rhs);
DQN_API Dqn_V2I &      operator*=                   (Dqn_V2I& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I &      operator*=                   (Dqn_V2I& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2I &      operator*=                   (Dqn_V2I& lhs, int32_t rhs);
DQN_API Dqn_V2I &      operator/=                   (Dqn_V2I& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I &      operator/=                   (Dqn_V2I& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2I &      operator/=                   (Dqn_V2I& lhs, int32_t rhs);
DQN_API Dqn_V2I &      operator-=                   (Dqn_V2I& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2I &      operator+=                   (Dqn_V2I& lhs, Dqn_V2I rhs);

DQN_API Dqn_V2I        Dqn_V2I_Min                  (Dqn_V2I a, Dqn_V2I b);
DQN_API Dqn_V2I        Dqn_V2I_Max                  (Dqn_V2I a, Dqn_V2I b);
DQN_API Dqn_V2I        Dqn_V2I_Abs                  (Dqn_V2I a);

#define                Dqn_V2U16_Zero               DQN_LITERAL(Dqn_V2U16){{(uint16_t)(0), (uint16_t)(0)}}
#define                Dqn_V2U16_One                DQN_LITERAL(Dqn_V2U16){{(uint16_t)(1), (uint16_t)(1)}}
#define                Dqn_V2U16_InitNx1(x)         DQN_LITERAL(Dqn_V2U16){{(uint16_t)(x), (uint16_t)(x)}}
#define                Dqn_V2U16_InitNx2(x, y)      DQN_LITERAL(Dqn_V2U16){{(uint16_t)(x), (uint16_t)(y)}}

DQN_API bool           operator!=                   (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API bool           operator==                   (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API bool           operator>=                   (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API bool           operator<=                   (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API bool           operator<                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API bool           operator>                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16      operator-                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16      operator+                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16      operator*                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16      operator*                    (Dqn_V2U16  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2U16      operator*                    (Dqn_V2U16  lhs, int32_t rhs);
DQN_API Dqn_V2U16      operator/                    (Dqn_V2U16  lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16      operator/                    (Dqn_V2U16  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2U16      operator/                    (Dqn_V2U16  lhs, int32_t rhs);
DQN_API Dqn_V2U16 &    operator*=                   (Dqn_V2U16& lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16 &    operator*=                   (Dqn_V2U16& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2U16 &    operator*=                   (Dqn_V2U16& lhs, int32_t rhs);
DQN_API Dqn_V2U16 &    operator/=                   (Dqn_V2U16& lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16 &    operator/=                   (Dqn_V2U16& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2U16 &    operator/=                   (Dqn_V2U16& lhs, int32_t rhs);
DQN_API Dqn_V2U16 &    operator-=                   (Dqn_V2U16& lhs, Dqn_V2U16 rhs);
DQN_API Dqn_V2U16 &    operator+=                   (Dqn_V2U16& lhs, Dqn_V2U16 rhs);

#define                Dqn_V2_Zero                  DQN_LITERAL(Dqn_V2){{(Dqn_f32)(0),    (Dqn_f32)(0)}}
#define                Dqn_V2_One                   DQN_LITERAL(Dqn_V2){{(Dqn_f32)(1),    (Dqn_f32)(1)}}
#define                Dqn_V2_InitNx1(x)            DQN_LITERAL(Dqn_V2){{(Dqn_f32)(x),    (Dqn_f32)(x)}}
#define                Dqn_V2_InitNx2(x, y)         DQN_LITERAL(Dqn_V2){{(Dqn_f32)(x),    (Dqn_f32)(y)}}
#define                Dqn_V2_InitV2I(xy)           DQN_LITERAL(Dqn_V2){{(Dqn_f32)(xy).x, (Dqn_f32)(xy).y}}

DQN_API bool           operator!=                   (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API bool           operator==                   (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API bool           operator>=                   (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API bool           operator<=                   (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API bool           operator<                    (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API bool           operator>                    (Dqn_V2  lhs, Dqn_V2  rhs);

DQN_API Dqn_V2         operator-                    (Dqn_V2  lhs);
DQN_API Dqn_V2         operator-                    (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API Dqn_V2         operator-                    (Dqn_V2  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2         operator-                    (Dqn_V2  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2         operator-                    (Dqn_V2  lhs, int32_t rhs);

DQN_API Dqn_V2         operator+                    (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API Dqn_V2         operator+                    (Dqn_V2  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2         operator+                    (Dqn_V2  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2         operator+                    (Dqn_V2  lhs, int32_t rhs);

DQN_API Dqn_V2         operator*                    (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API Dqn_V2         operator*                    (Dqn_V2  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2         operator*                    (Dqn_V2  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2         operator*                    (Dqn_V2  lhs, int32_t rhs);

DQN_API Dqn_V2         operator/                    (Dqn_V2  lhs, Dqn_V2  rhs);
DQN_API Dqn_V2         operator/                    (Dqn_V2  lhs, Dqn_V2I rhs);
DQN_API Dqn_V2         operator/                    (Dqn_V2  lhs, Dqn_f32 rhs);
DQN_API Dqn_V2         operator/                    (Dqn_V2  lhs, int32_t rhs);

DQN_API Dqn_V2 &       operator*=                   (Dqn_V2& lhs, Dqn_V2  rhs);
DQN_API Dqn_V2 &       operator*=                   (Dqn_V2& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2 &       operator*=                   (Dqn_V2& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2 &       operator*=                   (Dqn_V2& lhs, int32_t rhs);

DQN_API Dqn_V2 &       operator/=                   (Dqn_V2& lhs, Dqn_V2  rhs);
DQN_API Dqn_V2 &       operator/=                   (Dqn_V2& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2 &       operator/=                   (Dqn_V2& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2 &       operator/=                   (Dqn_V2& lhs, int32_t rhs);

DQN_API Dqn_V2 &       operator-=                   (Dqn_V2& lhs, Dqn_V2  rhs);
DQN_API Dqn_V2 &       operator-=                   (Dqn_V2& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2 &       operator-=                   (Dqn_V2& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2 &       operator-=                   (Dqn_V2& lhs, int32_t rhs);

DQN_API Dqn_V2 &       operator+=                   (Dqn_V2& lhs, Dqn_V2  rhs);
DQN_API Dqn_V2 &       operator+=                   (Dqn_V2& lhs, Dqn_V2I rhs);
DQN_API Dqn_V2 &       operator+=                   (Dqn_V2& lhs, Dqn_f32 rhs);
DQN_API Dqn_V2 &       operator+=                   (Dqn_V2& lhs, int32_t rhs);

DQN_API Dqn_V2         Dqn_V2_Min                   (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2         Dqn_V2_Max                   (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_V2         Dqn_V2_Abs                   (Dqn_V2 a);
DQN_API Dqn_f32        Dqn_V2_Dot                   (Dqn_V2 a, Dqn_V2 b);
DQN_API Dqn_f32        Dqn_V2_LengthSq_V2x2         (Dqn_V2 lhs, Dqn_V2 rhs);
DQN_API Dqn_f32        Dqn_V2_Length_V2x2           (Dqn_V2 lhs, Dqn_V2 rhs);
DQN_API Dqn_f32        Dqn_V2_LengthSq              (Dqn_V2 lhs);
DQN_API Dqn_f32        Dqn_V2_Length                (Dqn_V2 lhs);
DQN_API Dqn_V2         Dqn_V2_Normalise             (Dqn_V2 a);
DQN_API Dqn_V2         Dqn_V2_Perpendicular         (Dqn_V2 a);
DQN_API Dqn_V2         Dqn_V2_Reflect               (Dqn_V2 in, Dqn_V2 surface);
DQN_API Dqn_f32        Dqn_V2_Area                  (Dqn_V2 a);
#endif // !defined(DQN_NO_V2)
#if !defined(DQN_NO_V3)
// NOTE: [$VEC3] Vector3 ///////////////////////////////////////////////////////////////////////////
#define                Dqn_V3_InitNx1(x)            DQN_LITERAL(Dqn_V3){{(Dqn_f32)(x),    (Dqn_f32)(x),    (Dqn_f32)(x)}}
#define                Dqn_V3_InitNx3(x, y, z)      DQN_LITERAL(Dqn_V3){{(Dqn_f32)(x),    (Dqn_f32)(y),    (Dqn_f32)(z)}}
#define                Dqn_V3_InitV2x1_Nx1(xy, z)   DQN_LITERAL(Dqn_V3){{(Dqn_f32)(xy.x), (Dqn_f32)(xy.y), (Dqn_f32)(z)}}

DQN_API bool           operator!=                   (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API bool           operator==                   (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API bool           operator>=                   (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API bool           operator<=                   (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API bool           operator<                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API bool           operator>                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API Dqn_V3         operator-                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API Dqn_V3         operator-                    (Dqn_V3  lhs);
DQN_API Dqn_V3         operator+                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API Dqn_V3         operator*                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API Dqn_V3         operator*                    (Dqn_V3  lhs, Dqn_f32 rhs);
DQN_API Dqn_V3         operator*                    (Dqn_V3  lhs, int32_t rhs);
DQN_API Dqn_V3         operator/                    (Dqn_V3  lhs, Dqn_V3  rhs);
DQN_API Dqn_V3         operator/                    (Dqn_V3  lhs, Dqn_f32 rhs);
DQN_API Dqn_V3         operator/                    (Dqn_V3  lhs, int32_t rhs);
DQN_API Dqn_V3 &       operator*=                   (Dqn_V3 &lhs, Dqn_V3  rhs);
DQN_API Dqn_V3 &       operator*=                   (Dqn_V3 &lhs, Dqn_f32 rhs);
DQN_API Dqn_V3 &       operator*=                   (Dqn_V3 &lhs, int32_t rhs);
DQN_API Dqn_V3 &       operator/=                   (Dqn_V3 &lhs, Dqn_V3  rhs);
DQN_API Dqn_V3 &       operator/=                   (Dqn_V3 &lhs, Dqn_f32 rhs);
DQN_API Dqn_V3 &       operator/=                   (Dqn_V3 &lhs, int32_t rhs);
DQN_API Dqn_V3 &       operator-=                   (Dqn_V3 &lhs, Dqn_V3  rhs);
DQN_API Dqn_V3 &       operator+=                   (Dqn_V3 &lhs, Dqn_V3  rhs);
DQN_API Dqn_f32        Dqn_V3_LengthSq              (Dqn_V3 a);
DQN_API Dqn_f32        Dqn_V3_Length                (Dqn_V3 a);
DQN_API Dqn_V3         Dqn_V3_Normalise             (Dqn_V3 a);
#endif // !defined(DQN_NO_V3)
#if !defined(DQN_NO_V4)
// NOTE: [$VEC4] Vector4 ///////////////////////////////////////////////////////////////////////////
#define                Dqn_V4_InitNx1(x)            DQN_LITERAL(Dqn_V4){{(Dqn_f32)(x), (Dqn_f32)(x), (Dqn_f32)(x), (Dqn_f32)(x)}}
#define                Dqn_V4_InitNx4(x, y, z, w)   DQN_LITERAL(Dqn_V4){{(Dqn_f32)(x), (Dqn_f32)(y), (Dqn_f32)(z), (Dqn_f32)(w)}}
#define                Dqn_V4_Init_V3x1_Nx1(xyz, w) DQN_LITERAL(Dqn_V4){{xyz.x,        xyz.y,        xyz.z,        w}}
DQN_API bool           operator!=                   (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API bool           operator==                   (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API bool           operator>=                   (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API bool           operator<=                   (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API bool           operator<                    (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API bool           operator>                    (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API Dqn_V4         operator-                    (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API Dqn_V4         operator-                    (Dqn_V4  lhs);
DQN_API Dqn_V4         operator+                    (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API Dqn_V4         operator*                    (Dqn_V4  lhs, Dqn_V4  rhs);
DQN_API Dqn_V4         operator*                    (Dqn_V4  lhs, Dqn_f32 rhs);
DQN_API Dqn_V4         operator*                    (Dqn_V4  lhs, int32_t rhs);
DQN_API Dqn_V4         operator/                    (Dqn_V4  lhs, Dqn_f32 rhs);
DQN_API Dqn_V4 &       operator*=                   (Dqn_V4 &lhs, Dqn_V4  rhs);
DQN_API Dqn_V4 &       operator*=                   (Dqn_V4 &lhs, Dqn_f32 rhs);
DQN_API Dqn_V4 &       operator*=                   (Dqn_V4 &lhs, int32_t rhs);
DQN_API Dqn_V4 &       operator-=                   (Dqn_V4 &lhs, Dqn_V4  rhs);
DQN_API Dqn_V4 &       operator+=                   (Dqn_V4 &lhs, Dqn_V4  rhs);
#endif // !defined(DQN_NO_V4)
#if !defined(DQN_NO_M4)
// NOTE: [$MAT4] Dqn_M4 ////////////////////////////////////////////////////////////////////////////
DQN_API Dqn_f32        Dqn_V4Dot                    (Dqn_V4 a, Dqn_V4 b);
DQN_API Dqn_M4         Dqn_M4_Identity              ();
DQN_API Dqn_M4         Dqn_M4_ScaleF                (Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4         Dqn_M4_Scale                 (Dqn_V3 xyz);
DQN_API Dqn_M4         Dqn_M4_TranslateF            (Dqn_f32 x, Dqn_f32 y, Dqn_f32 z);
DQN_API Dqn_M4         Dqn_M4_Translate             (Dqn_V3 xyz);
DQN_API Dqn_M4         Dqn_M4_Transpose             (Dqn_M4 mat);
DQN_API Dqn_M4         Dqn_M4_Rotate                (Dqn_V3 axis, Dqn_f32 radians);
DQN_API Dqn_M4         Dqn_M4_Orthographic          (Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4         Dqn_M4_Perspective           (Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far);
DQN_API Dqn_M4         Dqn_M4_Add                   (Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4         Dqn_M4_Sub                   (Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4         Dqn_M4_Mul                   (Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4         Dqn_M4_Div                   (Dqn_M4 lhs, Dqn_M4 rhs);
DQN_API Dqn_M4         Dqn_M4_AddF                  (Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4         Dqn_M4_SubF                  (Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4         Dqn_M4_MulF                  (Dqn_M4 lhs, Dqn_f32 rhs);
DQN_API Dqn_M4         Dqn_M4_DivF                  (Dqn_M4 lhs, Dqn_f32 rhs);
#if !defined(DQN_NO_FSTR8)
DQN_API Dqn_FStr8<256> Dqn_M4_ColumnMajorString     (Dqn_M4 mat);
#endif
#endif // !defined(DQN_NO_M4)
// NOTE: [$M2x3] Dqn_M2x3 //////////////////////////////////////////////////////////////////////////
DQN_API bool           operator==                   (Dqn_M2x3 const &lhs, Dqn_M2x3 const &rhs);
DQN_API bool           operator!=                   (Dqn_M2x3 const &lhs, Dqn_M2x3 const &rhs);
DQN_API Dqn_M2x3       Dqn_M2x3_Identity            ();
DQN_API Dqn_M2x3       Dqn_M2x3_Translate           (Dqn_V2 offset);
DQN_API Dqn_M2x3       Dqn_M2x3_Scale               (Dqn_V2 scale);
DQN_API Dqn_M2x3       Dqn_M2x3_Rotate              (Dqn_f32 radians);
DQN_API Dqn_M2x3       Dqn_M2x3_Mul                 (Dqn_M2x3 m1, Dqn_M2x3 m2);
DQN_API Dqn_V2         Dqn_M2x3_Mul2F32             (Dqn_M2x3 m1, Dqn_f32 x, Dqn_f32 y);
DQN_API Dqn_V2         Dqn_M2x3_MulV2               (Dqn_M2x3 m1, Dqn_V2 v2);

#if !defined(DQN_NO_RECT)
// NOTE: [$RECT] Dqn_Rect //////////////////////////////////////////////////////////////////////////
#define                Dqn_Rect_InitV2x2(pos, size) DQN_LITERAL(Dqn_Rect){(pos), (size)}
#define                Dqn_Rect_InitNx4(x, y, w, h) DQN_LITERAL(Dqn_Rect){DQN_LITERAL(Dqn_V2){{x, y}}, DQN_LITERAL(Dqn_V2){{w, h}}}

DQN_API bool           operator==                   (const Dqn_Rect& lhs, const Dqn_Rect& rhs);
DQN_API Dqn_V2         Dqn_Rect_Center              (Dqn_Rect rect);
DQN_API bool           Dqn_Rect_ContainsPoint       (Dqn_Rect rect, Dqn_V2 p);
DQN_API bool           Dqn_Rect_ContainsRect        (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect       Dqn_Rect_Expand              (Dqn_Rect a, Dqn_f32 amount);
DQN_API Dqn_Rect       Dqn_Rect_ExpandV2            (Dqn_Rect a, Dqn_V2 amount);
DQN_API bool           Dqn_Rect_Intersects          (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect       Dqn_Rect_Intersection        (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_Rect       Dqn_Rect_Union               (Dqn_Rect a, Dqn_Rect b);
DQN_API Dqn_RectMinMax Dqn_Rect_MinMax              (Dqn_Rect a);
DQN_API Dqn_f32        Dqn_Rect_Area                (Dqn_Rect a);
DQN_API Dqn_V2         Dqn_Rect_InterpolatedPoint   (Dqn_Rect rect, Dqn_V2 t01);
DQN_API Dqn_V2         Dqn_Rect_TopLeft             (Dqn_Rect rect);
DQN_API Dqn_V2         Dqn_Rect_TopRight            (Dqn_Rect rect);
DQN_API Dqn_V2         Dqn_Rect_BottomLeft          (Dqn_Rect rect);
DQN_API Dqn_V2         Dqn_Rect_BottomRight         (Dqn_Rect rect);

DQN_API Dqn_Rect       Dqn_Rect_CutLeftClip         (Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip);
DQN_API Dqn_Rect       Dqn_Rect_CutRightClip        (Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip);
DQN_API Dqn_Rect       Dqn_Rect_CutTopClip          (Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip);
DQN_API Dqn_Rect       Dqn_Rect_CutBottomClip       (Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip);

#define                Dqn_Rect_CutLeft(rect, amount)         Dqn_Rect_CutLeftClip(rect, amount, Dqn_RectCutClip_Yes)
#define                Dqn_Rect_CutRight(rect, amount)        Dqn_Rect_CutRightClip(rect, amount, Dqn_RectCutClip_Yes)
#define                Dqn_Rect_CutTop(rect, amount)          Dqn_Rect_CutTopClip(rect, amount, Dqn_RectCutClip_Yes)
#define                Dqn_Rect_CutBottom(rect, amount)       Dqn_Rect_CutBottomClip(rect, amount, Dqn_RectCutClip_Yes)

#define                Dqn_Rect_CutLeftNoClip(rect, amount)   Dqn_Rect_CutLeftClip(rect, amount, Dqn_RectCutClip_No)
#define                Dqn_Rect_CutRightNoClip(rect, amount)  Dqn_Rect_CutRightClip(rect, amount, Dqn_RectCutClip_No)
#define                Dqn_Rect_CutTopNoClip(rect, amount)    Dqn_Rect_CutTopClip(rect, amount, Dqn_RectCutClip_No)
#define                Dqn_Rect_CutBottomNoClip(rect, amount) Dqn_Rect_CutBottomClip(rect, amount, Dqn_RectCutClip_No)

DQN_API Dqn_Rect       Dqn_RectCut_Cut                        (Dqn_RectCut rect_cut, Dqn_V2 size, Dqn_RectCutClip clip);
#define                Dqn_RectCut_Init(rect, side)           DQN_LITERAL(Dqn_RectCut){rect, side}
#define                Dqn_RectCut_Left(rect)                 DQN_LITERAL(Dqn_RectCut){rect, Dqn_RectCutSide_Left}
#define                Dqn_RectCut_Right(rect)                DQN_LITERAL(Dqn_RectCut){rect, Dqn_RectCutSide_Right}
#define                Dqn_RectCut_Top(rect)                  DQN_LITERAL(Dqn_RectCut){rect, Dqn_RectCutSide_Top}
#define                Dqn_RectCut_Bottom(rect)               DQN_LITERAL(Dqn_RectCut){rect, Dqn_RectCutSide_Bottom}
#endif // !defined(DQN_NO_RECT)
// NOTE: [$MATH] Other /////////////////////////////////////////////////////////////////////////////
DQN_API Dqn_RaycastLineIntersectV2Result Dqn_Raycast_LineIntersectV2(Dqn_V2 origin_a, Dqn_V2 dir_a, Dqn_V2 origin_b, Dqn_V2 dir_b);
DQN_API Dqn_V2                           Dqn_Lerp_V2                (Dqn_V2 a, Dqn_f32 t, Dqn_V2 b);
DQN_API Dqn_f32                          Dqn_Lerp_F32               (Dqn_f32 a, Dqn_f32 t, Dqn_f32 b);
