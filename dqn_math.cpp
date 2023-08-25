#if !defined(DQN_NO_V2)
// NOTE: [$VEC2] Vector2 ===========================================================================
// NOTE: Dqn_V2I
DQN_API bool operator!=(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = !(lhs == rhs);
    return result;
}

DQN_API bool operator==(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = (lhs.x == rhs.x) && (lhs.y == rhs.y);
    return result;
}

DQN_API bool operator>=(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = (lhs.x >= rhs.x) && (lhs.y >= rhs.y);
    return result;
}

DQN_API bool operator<=(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = (lhs.x <= rhs.x) && (lhs.y <= rhs.y);
    return result;
}

DQN_API bool operator<(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = (lhs.x <  rhs.x) && (lhs.y <  rhs.y);
    return result;
}

DQN_API bool operator>(Dqn_V2I lhs, Dqn_V2I rhs)
{
    bool result = (lhs.x >  rhs.x) && (lhs.y >  rhs.y);
    return result;
}

DQN_API Dqn_V2I operator-(Dqn_V2I lhs, Dqn_V2I rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x - rhs.x, lhs.y - rhs.y);
    return result;
}

DQN_API Dqn_V2I operator+(Dqn_V2I lhs, Dqn_V2I rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x + rhs.x, lhs.y + rhs.y);
    return result;
}

DQN_API Dqn_V2I operator*(Dqn_V2I lhs, Dqn_V2I rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x * rhs.x, lhs.y * rhs.y);
    return result;
}

DQN_API Dqn_V2I operator*(Dqn_V2I lhs, Dqn_f32 rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2I operator*(Dqn_V2I lhs, int32_t rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2I operator/(Dqn_V2I lhs, Dqn_V2I  rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x / rhs.x, lhs.y / rhs.y);
    return result;
}

DQN_API Dqn_V2I operator/(Dqn_V2I lhs, Dqn_f32 rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2I operator/(Dqn_V2I lhs, int32_t rhs)
{
    Dqn_V2I result = Dqn_V2I_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2I &operator*=(Dqn_V2I &lhs, Dqn_V2I rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator*=(Dqn_V2I &lhs, Dqn_f32 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator*=(Dqn_V2I &lhs, int32_t rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator/=(Dqn_V2I &lhs, Dqn_V2I rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator/=(Dqn_V2I &lhs, Dqn_f32 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator/=(Dqn_V2I &lhs, int32_t rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator-=(Dqn_V2I &lhs, Dqn_V2I rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

DQN_API Dqn_V2I &operator+=(Dqn_V2I &lhs, Dqn_V2I rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

// NOTE: Dqn_V2U16
DQN_API bool operator!=(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = !(lhs == rhs);
    return result;
}

DQN_API bool operator==(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = (lhs.x == rhs.x) && (lhs.y == rhs.y);
    return result;
}

DQN_API bool operator>=(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = (lhs.x >= rhs.x) && (lhs.y >= rhs.y);
    return result;
}

DQN_API bool operator<=(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = (lhs.x <= rhs.x) && (lhs.y <= rhs.y);
    return result;
}

DQN_API bool operator<(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = (lhs.x <  rhs.x) && (lhs.y <  rhs.y);
    return result;
}

DQN_API bool operator>(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    bool result = (lhs.x >  rhs.x) && (lhs.y >  rhs.y);
    return result;
}

DQN_API Dqn_V2U16 operator-(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x - rhs.x, lhs.y - rhs.y);
    return result;
}

DQN_API Dqn_V2U16 operator+(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x + rhs.x, lhs.y + rhs.y);
    return result;
}

DQN_API Dqn_V2U16 operator*(Dqn_V2U16 lhs, Dqn_V2U16 rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x * rhs.x, lhs.y * rhs.y);
    return result;
}

DQN_API Dqn_V2U16 operator*(Dqn_V2U16 lhs, Dqn_f32 rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2U16 operator*(Dqn_V2U16 lhs, int32_t rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2U16 operator/(Dqn_V2U16 lhs, Dqn_V2U16  rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x / rhs.x, lhs.y / rhs.y);
    return result;
}

DQN_API Dqn_V2U16 operator/(Dqn_V2U16 lhs, Dqn_f32 rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2U16 operator/(Dqn_V2U16 lhs, int32_t rhs)
{
    Dqn_V2U16 result = Dqn_V2U16_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2U16 &operator*=(Dqn_V2U16 &lhs, Dqn_V2U16 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator*=(Dqn_V2U16 &lhs, Dqn_f32 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator*=(Dqn_V2U16 &lhs, int32_t rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator/=(Dqn_V2U16 &lhs, Dqn_V2U16 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator/=(Dqn_V2U16 &lhs, Dqn_f32 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator/=(Dqn_V2U16 &lhs, int32_t rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator-=(Dqn_V2U16 &lhs, Dqn_V2U16 rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

DQN_API Dqn_V2U16 &operator+=(Dqn_V2U16 &lhs, Dqn_V2U16 rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

// NOTE: Dqn_V2
DQN_API bool operator!=(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = !(lhs == rhs);
    return result;
}

DQN_API bool operator==(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = (lhs.x == rhs.x) && (lhs.y == rhs.y);
    return result;
}

DQN_API bool operator>=(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = (lhs.x >= rhs.x) && (lhs.y >= rhs.y);
    return result;
}

DQN_API bool operator<=(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = (lhs.x <= rhs.x) && (lhs.y <= rhs.y);
    return result;
}

DQN_API bool operator<(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = (lhs.x <  rhs.x) && (lhs.y <  rhs.y);
    return result;
}

DQN_API bool operator>(Dqn_V2 lhs, Dqn_V2 rhs)
{
    bool result = (lhs.x >  rhs.x) && (lhs.y >  rhs.y);
    return result;
}

DQN_API Dqn_V2 operator-(Dqn_V2 lhs, Dqn_V2 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x - rhs.x, lhs.y - rhs.y);
    return result;
}

DQN_API Dqn_V2 operator-(Dqn_V2 lhs, Dqn_f32 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x - rhs, lhs.y - rhs);
    return result;
}

DQN_API Dqn_V2 operator+(Dqn_V2 lhs, Dqn_V2 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x + rhs.x, lhs.y + rhs.y);
    return result;
}

DQN_API Dqn_V2 operator+(Dqn_V2 lhs, Dqn_f32 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x + rhs, lhs.y + rhs);
    return result;
}

DQN_API Dqn_V2 operator*(Dqn_V2 lhs, Dqn_V2 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x * rhs.x, lhs.y * rhs.y);
    return result;
}

DQN_API Dqn_V2 operator*(Dqn_V2 lhs, Dqn_f32 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2 operator*(Dqn_V2 lhs, int32_t rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x * rhs, lhs.y * rhs);
    return result;
}

DQN_API Dqn_V2 operator/(Dqn_V2 lhs, Dqn_V2  rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x / rhs.x, lhs.y / rhs.y);
    return result;
}

DQN_API Dqn_V2 operator/(Dqn_V2 lhs, Dqn_f32 rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2 operator/(Dqn_V2 lhs, int32_t rhs)
{
    Dqn_V2 result = Dqn_V2_InitNx2(lhs.x / rhs, lhs.y / rhs);
    return result;
}

DQN_API Dqn_V2 &operator*=(Dqn_V2 &lhs, Dqn_V2 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator*=(Dqn_V2 &lhs, Dqn_f32 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator*=(Dqn_V2 &lhs, int32_t rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator/=(Dqn_V2 &lhs, Dqn_V2 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator/=(Dqn_V2 &lhs, Dqn_f32 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator/=(Dqn_V2 &lhs, int32_t rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator-=(Dqn_V2 &lhs, Dqn_V2 rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

DQN_API Dqn_V2 &operator+=(Dqn_V2 &lhs, Dqn_V2 rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

DQN_API Dqn_V2 Dqn_V2_Min(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2_InitNx2(DQN_MIN(a.x, b.x), DQN_MIN(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Max(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_V2 result = Dqn_V2_InitNx2(DQN_MAX(a.x, b.x), DQN_MAX(a.y, b.y));
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Abs(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2_InitNx2(DQN_ABS(a.x), DQN_ABS(a.y));
    return result;
}

DQN_API Dqn_f32 Dqn_V2_Dot(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y);
    return result;
}

DQN_API Dqn_f32 Dqn_V2_LengthSq(Dqn_V2 a, Dqn_V2 b)
{
    Dqn_f32 x_side = b.x - a.x;
    Dqn_f32 y_side = b.y - a.y;
    Dqn_f32 result = DQN_SQUARED(x_side) + DQN_SQUARED(y_side);
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Normalise(Dqn_V2 a)
{
    Dqn_f32 length_sq = DQN_SQUARED(a.x) + DQN_SQUARED(a.y);
    Dqn_f32 length    = DQN_SQRTF(length_sq);
    Dqn_V2 result     = a / length;
    return result;
}

DQN_API Dqn_V2 Dqn_V2_Perpendicular(Dqn_V2 a)
{
    Dqn_V2 result = Dqn_V2_InitNx2(-a.y, a.x);
    return result;
}
#endif // !defined(DQN_NO_V2)

#if !defined(DQN_NO_V3)
// NOTE: [$VEC3] Vector3 ===========================================================================
DQN_API bool operator!=(Dqn_V3 lhs, Dqn_V3  rhs)
{
    bool result = !(lhs == rhs);
    return result;
}

DQN_API bool operator==(Dqn_V3 lhs, Dqn_V3 rhs)
{
    bool result = (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z);
    return result;
}

DQN_API bool operator>=(Dqn_V3 lhs, Dqn_V3 rhs)
{
    bool result = (lhs.x >= rhs.x) && (lhs.y >= rhs.y) && (lhs.z >= rhs.z);
    return result;
}

DQN_API bool operator<=(Dqn_V3 lhs, Dqn_V3 rhs)
{
    bool result = (lhs.x <= rhs.x) && (lhs.y <= rhs.y) && (lhs.z <= rhs.z);
    return result;
}

DQN_API bool operator< (Dqn_V3 lhs, Dqn_V3 rhs)
{
    bool result = (lhs.x <  rhs.x) && (lhs.y <  rhs.y) && (lhs.z <  rhs.z);
    return result;
}

DQN_API bool operator>(Dqn_V3 lhs, Dqn_V3 rhs)
{
    bool result = (lhs.x >  rhs.x) && (lhs.y >  rhs.y) && (lhs.z >  rhs.z);
    return result;
}

DQN_API Dqn_V3 operator-(Dqn_V3 lhs, Dqn_V3 rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z);
    return result;
}

DQN_API Dqn_V3 operator+(Dqn_V3 lhs, Dqn_V3  rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z);
    return result;
}

DQN_API Dqn_V3 operator*(Dqn_V3 lhs, Dqn_V3  rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z);
    return result;
}

DQN_API Dqn_V3 operator*(Dqn_V3 lhs, Dqn_f32 rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    return result;
}

DQN_API Dqn_V3 operator*(Dqn_V3 lhs, int32_t rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs);
    return result;
}

DQN_API Dqn_V3 operator/(Dqn_V3 lhs, Dqn_V3  rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x / rhs.x, lhs.y / rhs.y, lhs.z / rhs.z);
    return result;
}

DQN_API Dqn_V3 operator/(Dqn_V3 lhs, Dqn_f32 rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
    return result;
}

DQN_API Dqn_V3 operator/(Dqn_V3 lhs, int32_t rhs)
{
    Dqn_V3 result = Dqn_V3_InitNx3(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs);
    return result;
}

DQN_API Dqn_V3 &operator*=(Dqn_V3 &lhs, Dqn_V3 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator*=(Dqn_V3 &lhs, Dqn_f32 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator*=(Dqn_V3 &lhs, int32_t rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator/=(Dqn_V3 &lhs, Dqn_V3 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator/=(Dqn_V3 &lhs, Dqn_f32 rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator/=(Dqn_V3 &lhs, int32_t rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator-=(Dqn_V3 &lhs, Dqn_V3 rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

DQN_API Dqn_V3 &operator+=(Dqn_V3 &lhs, Dqn_V3 rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

DQN_API Dqn_f32 Dqn_V3_LengthSq(Dqn_V3 a)
{
    Dqn_f32 result = DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z);
    return result;
}

DQN_API Dqn_f32 Dqn_V3_Length(Dqn_V3 a)
{
    Dqn_f32 length_sq = DQN_SQUARED(a.x) + DQN_SQUARED(a.y) + DQN_SQUARED(a.z);
    Dqn_f32 result    = DQN_SQRTF(length_sq);
    return result;
}

DQN_API Dqn_V3 Dqn_V3_Normalise(Dqn_V3 a)
{
    Dqn_f32 length = Dqn_V3_Length(a);
    Dqn_V3  result = a / length;
    return result;
}
#endif // !defined(DQN_NO_V3)

#if !defined(DQN_NO_V4)
// NOTE: [$VEC4] Vector4 ===========================================================================
DQN_API bool operator!=(Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = !(lhs == rhs);
    return result;
}

DQN_API bool operator==(Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = (lhs.x == rhs.x) && (lhs.y == rhs.y) && (lhs.z == rhs.z) && (lhs.w == rhs.w);
    return result;
}

DQN_API bool operator>=(Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = (lhs.x >= rhs.x) && (lhs.y >= rhs.y) && (lhs.z >= rhs.z) && (lhs.w >= rhs.w);
    return result;
}

DQN_API bool operator<=(Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = (lhs.x <= rhs.x) && (lhs.y <= rhs.y) && (lhs.z <= rhs.z) && (lhs.w <= rhs.w);
    return result;
}

DQN_API bool operator< (Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = (lhs.x < rhs.x) && (lhs.y < rhs.y) && (lhs.z < rhs.z) && (lhs.w < rhs.w);
    return result;
}

DQN_API bool operator>(Dqn_V4 lhs, Dqn_V4 rhs)
{
    bool result = (lhs.x >  rhs.x) && (lhs.y > rhs.y) && (lhs.z > rhs.z) && (lhs.w > rhs.w);
    return result;
}

DQN_API Dqn_V4 operator-(Dqn_V4 lhs, Dqn_V4 rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x - rhs.x, lhs.y - rhs.y, lhs.z - rhs.z, lhs.w - rhs.w);
    return result;
}

DQN_API Dqn_V4 operator+(Dqn_V4 lhs, Dqn_V4 rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x + rhs.x, lhs.y + rhs.y, lhs.z + rhs.z, lhs.w + rhs.w);
    return result;
}

DQN_API Dqn_V4 operator* (Dqn_V4 lhs, Dqn_V4 rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x * rhs.x, lhs.y * rhs.y, lhs.z * rhs.z, lhs.w * rhs.w);
    return result;
}

DQN_API Dqn_V4 operator*(Dqn_V4 lhs, Dqn_f32 rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
    return result;
}

DQN_API Dqn_V4 operator*(Dqn_V4 lhs, int32_t rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x * rhs, lhs.y * rhs, lhs.z * rhs, lhs.w * rhs);
    return result;
}

DQN_API Dqn_V4 operator/(Dqn_V4 lhs, Dqn_f32 rhs)
{
    Dqn_V4 result = Dqn_V4_InitNx4(lhs.x / rhs, lhs.y / rhs, lhs.z / rhs, lhs.w / rhs);
    return result;
}

DQN_API Dqn_V4 &operator*=(Dqn_V4 &lhs, Dqn_V4 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V4 &operator*=(Dqn_V4 &lhs, Dqn_f32 rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V4 &operator*=(Dqn_V4 &lhs, int32_t rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

DQN_API Dqn_V4 &operator-=(Dqn_V4 &lhs, Dqn_V4  rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

DQN_API Dqn_V4 &operator+=(Dqn_V4 &lhs, Dqn_V4  rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

DQN_API Dqn_f32 Dqn_V4Dot(Dqn_V4 a, Dqn_V4 b)
{
    Dqn_f32 result = (a.x * b.x) + (a.y * b.y) + (a.z * b.z) + (a.w * b.w);
    return result;
}
#endif // !defined(DQN_NO_V4)

#if !defined(DQN_NO_M4)
// NOTE: [$MAT4] Dqn_M4 ============================================================================
DQN_API Dqn_M4 Dqn_M4_Identity()
{
    Dqn_M4 result =
    {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {0, 0, 0, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_ScaleF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_M4 result =
    {{
        {x, 0, 0, 0},
        {0, y, 0, 0},
        {0, 0, z, 0},
        {0, 0, 0, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Scale(Dqn_V3 xyz)
{
    Dqn_M4 result =
    {{
        {xyz.x, 0,     0,     0},
        {0,     xyz.y, 0,     0},
        {0,     0,     xyz.z, 0},
        {0,     0,     0,     1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_TranslateF(Dqn_f32 x, Dqn_f32 y, Dqn_f32 z)
{
    Dqn_M4 result =
    {{
        {1, 0, 0, 0},
        {0, 1, 0, 0},
        {0, 0, 1, 0},
        {x, y, z, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Translate(Dqn_V3 xyz)
{
    Dqn_M4 result =
    {{
        {1,     0,     0,     0},
        {0,     1,     0,     0},
        {0,     0,     1,     0},
        {xyz.x, xyz.y, xyz.z, 1},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Transpose(Dqn_M4 mat)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
        for (int row = 0; row < 4; row++)
            result.columns[col][row] = mat.columns[row][col];
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Rotate(Dqn_V3 axis01, Dqn_f32 radians)
{
    DQN_ASSERTF(DQN_ABS(Dqn_V3_Length(axis01) - 1.f) <= 0.01f,
                "Rotation axis must be normalised, length = %f",
                Dqn_V3_Length(axis01));

    Dqn_f32 sin           = DQN_SINF(radians);
    Dqn_f32 cos           = DQN_COSF(radians);
    Dqn_f32 one_minus_cos = 1.f - cos;

    Dqn_f32 x  = axis01.x;
    Dqn_f32 y  = axis01.y;
    Dqn_f32 z  = axis01.z;
    Dqn_f32 x2 = DQN_SQUARED(x);
    Dqn_f32 y2 = DQN_SQUARED(y);
    Dqn_f32 z2 = DQN_SQUARED(z);

    Dqn_M4 result =
    {{
        {cos + x2*one_minus_cos,    y*x*one_minus_cos + z*sin, z*x*one_minus_cos - y*sin, 0}, // Col 1
        {x*y*one_minus_cos - z*sin, cos + y2*one_minus_cos,    z*y*one_minus_cos + x*sin, 0}, // Col 2
        {x*z*one_minus_cos + y*sin, y*z*one_minus_cos - x*sin, cos + z2*one_minus_cos,    0}, // Col 3
        {0,                         0,                         0,                         1}, // Col 4
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Orthographic(Dqn_f32 left, Dqn_f32 right, Dqn_f32 bottom, Dqn_f32 top, Dqn_f32 z_near, Dqn_f32 z_far)
{
    // NOTE: Here is the matrix in column major for readability. Below it's
    // transposed due to how you have to declare column major matrices in C/C++.
    //
    // m = [2/r-l, 0,      0,     -1*(r+l)/(r-l)]
    //     [0,     2/t-b,  0,      1*(t+b)/(t-b)]
    //     [0,     0,     -2/f-n, -1*(f+n)/(f-n)]
    //     [0,     0,      0,      1            ]

    Dqn_M4 result =
    {{
         {2.f / (right - left),                     0.f,                                      0.f,                                          0.f},
         {0.f,                                      2.f / (top - bottom),                     0.f,                                          0.f},
         {0.f,                                      0.f,                                      -2.f / (z_far - z_near),                      0.f},
         {(-1.f * (right + left)) / (right - left), (-1.f * (top + bottom)) / (top - bottom), (-1.f * (z_far + z_near)) / (z_far - z_near), 1.f},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Perspective(Dqn_f32 fov /*radians*/, Dqn_f32 aspect, Dqn_f32 z_near, Dqn_f32 z_far)
{
    Dqn_f32 tan_fov = DQN_TANF(fov / 2.f);
    Dqn_M4 result =
    {{
         {1.f / (aspect * tan_fov), 0.f,           0.f,                                     0.f},
         {0,                        1.f / tan_fov, 0.f,                                     0.f},
         {0.f,                      0.f,           (z_near + z_far) / (z_near - z_far),    -1.f},
         {0.f,                      0.f,           (2.f * z_near * z_far)/(z_near - z_far), 0.f},
    }};

    return result;
}

DQN_API Dqn_M4 Dqn_M4_Add(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Sub(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Mul(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int row = 0; row < 4; row++)
        {
            Dqn_f32 sum = 0;
            for (int f32_it = 0; f32_it < 4; f32_it++)
                sum += lhs.columns[f32_it][row] * rhs.columns[col][f32_it];

            result.columns[col][row] = sum;
        }
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_Div(Dqn_M4 lhs, Dqn_M4 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs.columns[col][it];
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_AddF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] + rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_SubF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] - rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_MulF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] * rhs;
    }
    return result;
}

DQN_API Dqn_M4 Dqn_M4_DivF(Dqn_M4 lhs, Dqn_f32 rhs)
{
    Dqn_M4 result;
    for (int col = 0; col < 4; col++)
    {
        for (int it = 0; it < 4; it++)
            result.columns[col][it] = lhs.columns[col][it] / rhs;
    }
    return result;
}

#if !defined(DQN_NO_FSTRING8)
DQN_API Dqn_FString8<256> Dqn_M4_ColumnMajorString(Dqn_M4 mat)
{
    Dqn_FString8<256> result = {};
    for (int row = 0; row < 4; row++) {
        for (int it = 0; it < 4; it++) {
            if (it == 0) Dqn_FString8_Append(&result, DQN_STRING8("|"));
            Dqn_FString8_AppendF(&result, "%.5f", mat.columns[it][row]);
            if (it != 3) Dqn_FString8_Append(&result, DQN_STRING8(", "));
            else         Dqn_FString8_Append(&result, DQN_STRING8("|\n"));
        }
    }

    return result;
}
#endif
#endif // !defined(DQN_M4)

#if !defined(DQN_NO_RECT)
// NOTE: [$RECT] Dqn_Rect ==========================================================================
DQN_API bool operator==(const Dqn_Rect& lhs, const Dqn_Rect& rhs)
{
    bool result = (lhs.pos == rhs.pos) && (lhs.size == rhs.size);
    return result;
}

DQN_API Dqn_V2 Dqn_Rect_Center(Dqn_Rect rect)
{
    Dqn_V2 result = rect.pos + (rect.size * .5f);
    return result;
}

DQN_API bool Dqn_Rect_ContainsPoint(Dqn_Rect rect, Dqn_V2 p)
{
    Dqn_V2 min  = rect.pos;
    Dqn_V2 max  = rect.pos + rect.size;
    bool result = (p.x >= min.x && p.x <= max.x && p.y >= min.y && p.y <= max.y);
    return result;
}

DQN_API bool Dqn_Rect_ContainsRect(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_V2 a_min = a.pos;
    Dqn_V2 a_max = a.pos + a.size;
    Dqn_V2 b_min = b.pos;
    Dqn_V2 b_max = b.pos + b.size;
    bool result = (b_min >= a_min && b_max <= a_max);
    return result;
}

DQN_API bool Dqn_Rect_Intersects(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_V2 a_min = a.pos;
    Dqn_V2 a_max = a.pos + a.size;
    Dqn_V2 b_min = b.pos;
    Dqn_V2 b_max = b.pos + b.size;
    bool result = (a_min.x <= b_max.x && a_max.x >= b_min.x) &&
                  (a_min.y <= b_max.y && a_max.y >= b_min.y);
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Intersection(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_Rect result = Dqn_Rect_InitV2x2(a.pos, Dqn_V2_InitNx1(0));
    if (Dqn_Rect_Intersects(a, b)) {
        Dqn_V2 a_min = a.pos;
        Dqn_V2 a_max = a.pos + a.size;
        Dqn_V2 b_min = b.pos;
        Dqn_V2 b_max = b.pos + b.size;

        Dqn_V2 min = {};
        Dqn_V2 max = {};
        min.x      = DQN_MAX(a_min.x, b_min.x);
        min.y      = DQN_MAX(a_min.y, b_min.y);
        max.x      = DQN_MIN(a_max.x, b_max.x);
        max.y      = DQN_MIN(a_max.y, b_max.y);
        result     = Dqn_Rect_InitV2x2(min, max - min);
    }
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_Union(Dqn_Rect a, Dqn_Rect b)
{
    Dqn_V2 a_min = a.pos;
    Dqn_V2 a_max = a.pos + a.size;
    Dqn_V2 b_min = b.pos;
    Dqn_V2 b_max = b.pos + b.size;

    Dqn_V2 min, max;
    min.x = DQN_MIN(a_min.x, b_min.x);
    min.y = DQN_MIN(a_min.y, b_min.y);
    max.x = DQN_MAX(a_max.x, b_max.x);
    max.y = DQN_MAX(a_max.y, b_max.y);
    Dqn_Rect result = Dqn_Rect_InitV2x2(min, max - min);
    return result;
}

DQN_API Dqn_RectMinMax Dqn_Rect_MinMax(Dqn_Rect a)
{
    Dqn_RectMinMax result = {};
    result.min            = a.pos;
    result.max            = a.pos + a.size;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_CutLeftClip(Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip)
{
    Dqn_f32 min_x        = rect->pos.x;
    Dqn_f32 max_x        = rect->pos.x + rect->size.w;
    Dqn_f32 result_max_x = min_x + amount;
    if (clip)
        result_max_x = DQN_MIN(result_max_x, max_x);
    Dqn_Rect result      = Dqn_Rect_InitNx4(min_x, rect->pos.y, result_max_x - min_x, rect->size.h);
    rect->pos.x          = result_max_x;
    rect->size.w         = max_x - result_max_x;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_CutRightClip(Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip)
{
    Dqn_f32 min_x        = rect->pos.x;
    Dqn_f32 max_x        = rect->pos.x + rect->size.w;
    Dqn_f32 result_min_x = max_x - amount;
    if (clip)
        result_min_x = DQN_MAX(result_min_x, 0);
    Dqn_Rect result      = Dqn_Rect_InitNx4(result_min_x, rect->pos.y, max_x - result_min_x, rect->size.h);
    rect->size.w         = result_min_x - min_x;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_CutTopClip(Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip)
{
    Dqn_f32 min_y        = rect->pos.y;
    Dqn_f32 max_y        = rect->pos.y + rect->size.h;
    Dqn_f32 result_max_y = min_y + amount;
    if (clip)
        result_max_y = DQN_MIN(result_max_y, max_y);
    Dqn_Rect result      = Dqn_Rect_InitNx4(rect->pos.x, min_y, rect->size.w, result_max_y - min_y);
    rect->pos.y          = result_max_y;
    rect->size.h         = max_y - result_max_y;
    return result;
}

DQN_API Dqn_Rect Dqn_Rect_CutBottomClip(Dqn_Rect *rect, Dqn_f32 amount, Dqn_RectCutClip clip)
{
    Dqn_f32 min_y        = rect->pos.y;
    Dqn_f32 max_y        = rect->pos.y + rect->size.h;
    Dqn_f32 result_min_y = max_y - amount;
    if (clip)
        result_min_y = DQN_MAX(result_min_y, 0);
    Dqn_Rect result      = Dqn_Rect_InitNx4(rect->pos.x, result_min_y, rect->size.w, max_y - result_min_y);
    rect->size.h         = result_min_y - min_y;
    return result;
}

DQN_API Dqn_Rect Dqn_RectCut_Cut(Dqn_RectCut rect_cut, Dqn_V2 size, Dqn_RectCutClip clip)
{
    Dqn_Rect result = {};
    if (rect_cut.rect) {
        switch (rect_cut.side) {
            case Dqn_RectCutSide_Left:   result = Dqn_Rect_CutLeftClip(rect_cut.rect,   size.w, clip); break;
            case Dqn_RectCutSide_Right:  result = Dqn_Rect_CutRightClip(rect_cut.rect,  size.w, clip); break;
            case Dqn_RectCutSide_Top:    result = Dqn_Rect_CutTopClip(rect_cut.rect,    size.h, clip); break;
            case Dqn_RectCutSide_Bottom: result = Dqn_Rect_CutBottomClip(rect_cut.rect, size.h, clip); break;
        }
    }
    return result;
}

#endif // !defined(DQN_NO_RECT)

// NOTE: [$MATH] Other =============================================================================
DQN_API Dqn_V2 Dqn_Lerp_V2(Dqn_V2 a, Dqn_f32 t, Dqn_V2 b)
{
    Dqn_V2 result = {};
    result.x  = a.x + ((b.x - a.x) * t);
    result.y  = a.y + ((b.y - a.y) * t);
    return result;
}

DQN_API Dqn_f32 Dqn_Lerp_F32(Dqn_f32 a, Dqn_f32 t, Dqn_f32 b)
{
    Dqn_f32 result = a + ((b - a) * t);
    return result;
}
