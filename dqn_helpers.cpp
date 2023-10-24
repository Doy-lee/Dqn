// NOTE: [$PCGX] Dqn_PCG32 =========================================================================
#define DQN_PCG_DEFAULT_MULTIPLIER_64 6364136223846793005ULL
#define DQN_PCG_DEFAULT_INCREMENT_64  1442695040888963407ULL

DQN_API uint32_t Dqn_PCG32_Next(Dqn_PCG32 *rng)
{
    uint64_t state = rng->state;
    rng->state     = state * DQN_PCG_DEFAULT_MULTIPLIER_64 + DQN_PCG_DEFAULT_INCREMENT_64;

    // XSH-RR
    uint32_t value = (uint32_t)((state ^ (state >> 18)) >> 27);
    int      rot   = state >> 59;
    return rot ? (value >> rot) | (value << (32 - rot)) : value;
}

DQN_API uint64_t Dqn_PCG32_Next64(Dqn_PCG32 *rng)
{
    uint64_t value = Dqn_PCG32_Next(rng);
    value <<= 32;
    value |= Dqn_PCG32_Next(rng);
    return value;
}

DQN_API uint32_t Dqn_PCG32_Range(Dqn_PCG32 *rng, uint32_t low, uint32_t high)
{
    uint32_t bound     = high - low;
    uint32_t threshold = -(int32_t)bound % bound;

    for (;;) {
        uint32_t r = Dqn_PCG32_Next(rng);
        if (r >= threshold)
            return low + (r % bound);
    }
}

DQN_API float Dqn_PCG32_NextF32(Dqn_PCG32 *rng)
{
    uint32_t x = Dqn_PCG32_Next(rng);
    return (float)(int32_t)(x >> 8) * 0x1.0p-24f;
}

DQN_API double Dqn_PCG32_NextF64(Dqn_PCG32 *rng)
{
    uint64_t x = Dqn_PCG32_Next64(rng);
    return (double)(int64_t)(x >> 11) * 0x1.0p-53;
}

DQN_API void Dqn_PCG32_Seed(Dqn_PCG32 *rng, uint64_t seed)
{
    rng->state = 0ULL;
    Dqn_PCG32_Next(rng);
    rng->state += seed;
    Dqn_PCG32_Next(rng);
}

DQN_API void Dqn_PCG32_Advance(Dqn_PCG32 *rng, uint64_t delta)
{
    uint64_t cur_mult = DQN_PCG_DEFAULT_MULTIPLIER_64;
    uint64_t cur_plus = DQN_PCG_DEFAULT_INCREMENT_64;

    uint64_t acc_mult = 1;
    uint64_t acc_plus = 0;

    while (delta != 0) {
        if (delta & 1) {
            acc_mult *= cur_mult;
            acc_plus = acc_plus * cur_mult + cur_plus;
        }
        cur_plus = (cur_mult + 1) * cur_plus;
        cur_mult *= cur_mult;
        delta >>= 1;
    }

    rng->state = acc_mult * rng->state + acc_plus;
}

#if !defined(DQN_NO_JSON_BUILDER)
// NOTE: [$JSON] Dqn_JSONBuilder ===================================================================
DQN_API Dqn_JSONBuilder Dqn_JSONBuilder_Init(Dqn_Allocator allocator, int spaces_per_indent)
{
    Dqn_JSONBuilder result          = {};
    result.spaces_per_indent        = spaces_per_indent;
    result.string_builder.allocator = allocator;
    return result;
}

DQN_API Dqn_Str8 Dqn_JSONBuilder_Build(Dqn_JSONBuilder const *builder, Dqn_Allocator allocator)
{
    Dqn_Str8 result = Dqn_Str8Builder_Build(&builder->string_builder, allocator);
    return result;
}

DQN_API void Dqn_JSONBuilder_KeyValue(Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value)
{
    if (key.size == 0 && value.size == 0)
        return;

    Dqn_JSONBuilderItem item = Dqn_JSONBuilderItem_KeyValue;
    if (value.size == 1) {
        if (value.data[0] == '{' || value.data[0] == '[') {
            item = Dqn_JSONBuilderItem_OpenContainer;
        } else if (value.data[0] == '}' || value.data[0] == ']') {
            item = Dqn_JSONBuilderItem_CloseContainer;
        }
    }

    bool adding_to_container_with_items = item != Dqn_JSONBuilderItem_CloseContainer &&
                                          (builder->last_item == Dqn_JSONBuilderItem_KeyValue ||
                                           builder->last_item == Dqn_JSONBuilderItem_CloseContainer);

    uint8_t prefix_size = 0;
    char prefix[2]      = {0};
    if (adding_to_container_with_items)
        prefix[prefix_size++] = ',';

    if (builder->last_item != Dqn_JSONBuilderItem_Empty)
        prefix[prefix_size++] = '\n';

    if (item == Dqn_JSONBuilderItem_CloseContainer)
        builder->indent_level--;

    int spaces_per_indent = builder->spaces_per_indent ? builder->spaces_per_indent : 2;
    int spaces            = builder->indent_level * spaces_per_indent;

    if (key.size) {
        Dqn_Str8Builder_AppendF(&builder->string_builder,
                                   "%.*s%*c\"%.*s\": %.*s",
                                   prefix_size, prefix,
                                   spaces, ' ',
                                   DQN_STR_FMT(key),
                                   DQN_STR_FMT(value));
    } else {
        Dqn_Str8Builder_AppendF(&builder->string_builder,
                                   "%.*s%*c%.*s",
                                   prefix_size, prefix,
                                   spaces, ' ',
                                   DQN_STR_FMT(value));
    }

    if (item == Dqn_JSONBuilderItem_OpenContainer)
        builder->indent_level++;

    builder->last_item = item;
}

DQN_API void Dqn_JSONBuilder_KeyValueFV(Dqn_JSONBuilder *builder, Dqn_Str8 key, char const *value_fmt, va_list args)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(builder->string_builder.allocator.user_context);
    Dqn_Str8          value   = Dqn_Str8_InitFV(scratch.allocator, value_fmt, args);
    Dqn_JSONBuilder_KeyValue(builder, key, value);
}

DQN_API void Dqn_JSONBuilder_KeyValueF(Dqn_JSONBuilder *builder, Dqn_Str8 key, char const *value_fmt, ...)
{
    va_list args;
    va_start(args, value_fmt);
    Dqn_JSONBuilder_KeyValueFV(builder, key, value_fmt, args);
    va_end(args);
}

DQN_API void Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_Str8 name)
{
    Dqn_JSONBuilder_KeyValue(builder, name, DQN_STR8("{"));
}

DQN_API void Dqn_JSONBuilder_ObjectEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_KeyValue(builder, DQN_STR8(""), DQN_STR8("}"));
}

DQN_API void Dqn_JSONBuilder_ArrayBeginNamed(Dqn_JSONBuilder *builder, Dqn_Str8 name)
{
    Dqn_JSONBuilder_KeyValue(builder, name, DQN_STR8("["));
}

DQN_API void Dqn_JSONBuilder_ArrayEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_KeyValue(builder, DQN_STR8(""), DQN_STR8("]"));
}

DQN_API void Dqn_JSONBuilder_StrNamed(Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "\"%.*s\"", value.size, value.data);
}

DQN_API void Dqn_JSONBuilder_LiteralNamed(Dqn_JSONBuilder *builder, Dqn_Str8 key, Dqn_Str8 value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%.*s", value.size, value.data);
}

DQN_API void Dqn_JSONBuilder_U64Named(Dqn_JSONBuilder *builder, Dqn_Str8 key, uint64_t value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%I64u", value);
}

DQN_API void Dqn_JSONBuilder_I64Named(Dqn_JSONBuilder *builder, Dqn_Str8 key, int64_t value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%I64d", value);
}

DQN_API void Dqn_JSONBuilder_F64Named(Dqn_JSONBuilder *builder, Dqn_Str8 key, double value, int decimal_places)
{
    if (!builder)
        return;

    if (decimal_places >= 16)
        decimal_places = 16;

    // NOTE: Generate the format string for the float, depending on how many
    // decimals places it wants.
    char float_fmt[16];
    if (decimal_places > 0) {
        // NOTE: Emit the format string "%.<decimal_places>f" i.e. %.1f
        STB_SPRINTF_DECORATE(snprintf)(float_fmt, sizeof(float_fmt), "%%.%df", decimal_places);
    } else {
        // NOTE: Emit the format string "%f"
        STB_SPRINTF_DECORATE(snprintf)(float_fmt, sizeof(float_fmt), "%%f");
    }

    char fmt[32];
    if (key.size)
        STB_SPRINTF_DECORATE(snprintf)(fmt, sizeof(fmt), "\"%%.*s\": %s", float_fmt);
    else
        STB_SPRINTF_DECORATE(snprintf)(fmt, sizeof(fmt), "%s", float_fmt);

    Dqn_JSONBuilder_KeyValueF(builder, key, fmt, value);
}

DQN_API void Dqn_JSONBuilder_BoolNamed(Dqn_JSONBuilder *builder, Dqn_Str8 key, bool value)
{
    Dqn_Str8 value_string = value ? DQN_STR8("true") : DQN_STR8("false");
    Dqn_JSONBuilder_KeyValueF(builder, key, "%.*s", value_string.size, value_string.data);
}
#endif // !defined(DQN_NO_JSON_BUILDER)

#if !defined(DQN_NO_BIN)
// NOTE: [$BHEX] Dqn_Bin ===========================================================================
DQN_API char const *Dqn_Bin_HexBufferTrim0x(char const *hex, Dqn_usize size, Dqn_usize *real_size)
{
    Dqn_Str8 result = Dqn_Str8_TrimWhitespaceAround(Dqn_Str8_Init(hex, size));
    result          = Dqn_Str8_TrimPrefix(result, DQN_STR8("0x"), Dqn_Str8EqCase_Insensitive);
    if (real_size)
        *real_size = result.size;
    return result.data;
}

DQN_API Dqn_Str8 Dqn_Bin_HexTrim0x(Dqn_Str8 string)
{
    Dqn_usize   trimmed_size = 0;
    char const *trimmed      = Dqn_Bin_HexBufferTrim0x(string.data, string.size, &trimmed_size);
    Dqn_Str8    result       = Dqn_Str8_Init(trimmed, trimmed_size);
    return result;
}

DQN_API Dqn_BinHexU64Str8 Dqn_Bin_U64ToHexU64Str8(uint64_t number, uint32_t flags)
{
    Dqn_Str8 prefix = {};
    if (!(flags & Dqn_BinHexU64Str8Flags_No0xPrefix))
        prefix = DQN_STR8("0x");

    Dqn_BinHexU64Str8 result = {};
    DQN_MEMCPY(result.data, prefix.data, prefix.size);
    result.size += DQN_CAST(int8_t)prefix.size;

    char const *fmt = (flags & Dqn_BinHexU64Str8Flags_UppercaseHex) ? "%I64X" : "%I64x";
    int size        = STB_SPRINTF_DECORATE(snprintf)(result.data + result.size, DQN_ARRAY_UCOUNT(result.data) - result.size, fmt, number);
    result.size    += DQN_CAST(uint8_t)size;
    DQN_ASSERT(result.size < DQN_ARRAY_UCOUNT(result.data));

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not, but, always null terminates so
    // the last byte is wasted.
    result.size = DQN_MIN(result.size, DQN_ARRAY_UCOUNT(result.data) - 1);
    return result;
}

DQN_API Dqn_Str8 Dqn_Bin_U64ToHex(Dqn_Allocator allocator, uint64_t number, uint32_t flags)
{
    Dqn_Str8 prefix = {};
    if (!(flags & Dqn_BinHexU64Str8Flags_No0xPrefix))
        prefix = DQN_STR8("0x");

    char const *fmt         = (flags & Dqn_BinHexU64Str8Flags_UppercaseHex) ? "%I64X" : "%I64x";
    Dqn_usize required_size = Dqn_CStr8_FSize(fmt, number) + prefix.size;
    Dqn_Str8 result         = Dqn_Str8_Allocate(allocator, required_size, Dqn_ZeroMem_No);

    if (Dqn_Str8_IsValid(result)) {
        DQN_MEMCPY(result.data, prefix.data, prefix.size);
        int space = DQN_CAST(int)DQN_MAX((result.size - prefix.size) + 1, 0); /*null-terminator*/
        STB_SPRINTF_DECORATE(snprintf)(result.data + prefix.size, space, fmt, number);
    }
    return result;
}

DQN_API uint64_t Dqn_Bin_HexBufferToU64(char const *hex, Dqn_usize size)
{
    Dqn_usize   trim_size = size;
    char const *trim_hex  = hex;
    if (trim_size >= 2) {
        if (trim_hex[0] == '0' && (trim_hex[1] == 'x' || trim_hex[1] == 'X')) {
            trim_size -= 2;
            trim_hex += 2;
        }
    }

    DQN_ASSERT(DQN_CAST(Dqn_usize)(trim_size * 4 / 8) /*maximum amount of bytes represented in the hex string*/ <= sizeof(uint64_t));

    uint64_t   result  = 0;
    Dqn_usize max_size = DQN_MIN(size, 8 /*bytes*/ * 2 /*hex chars per byte*/);
    for (Dqn_usize hex_index = 0; hex_index < max_size; hex_index++) {
        char ch = trim_hex[hex_index];
        if (!Dqn_Char_IsHex(ch))
            break;
        uint8_t val = Dqn_Char_HexToU8(ch);
        result      = (result << 4) | val;
    }
    return result;
}

DQN_API uint64_t Dqn_Bin_HexToU64(Dqn_Str8 hex)
{
    uint64_t result = Dqn_Bin_HexBufferToU64(hex.data, hex.size);
    return result;
}

DQN_API bool Dqn_Bin_BytesToHexBuffer(void const *src, Dqn_usize src_size, char *dest, Dqn_usize dest_size)
{
    if (!src || !dest)
        return false;

    if (!DQN_CHECK(dest_size >= src_size * 2))
        return false;

    char const *HEX             = "0123456789abcdef";
    unsigned char const *src_u8 = DQN_CAST(unsigned char const *)src;
    for (Dqn_usize src_index = 0, dest_index = 0; src_index < src_size; src_index++) {
        char byte          = src_u8[src_index];
        char hex01         = (byte >> 4) & 0b1111;
        char hex02         = (byte >> 0) & 0b1111;
        dest[dest_index++] = HEX[(int)hex01];
        dest[dest_index++] = HEX[(int)hex02];
    }

    return true;
}

DQN_API char *Dqn_Bin_BytesToHexBufferArena(Dqn_Arena *arena, void const *src, Dqn_usize size)
{
    char *result = size > 0 ? Dqn_Arena_NewArray(arena, char, (size * 2) + 1 /*null terminate*/, Dqn_ZeroMem_No) : nullptr;
    if (result) {
        bool converted = Dqn_Bin_BytesToHexBuffer(src, size, result, size * 2);
        DQN_ASSERT(converted);
        result[size * 2] = 0;
    }
    return result;
}

DQN_API Dqn_Str8 Dqn_Bin_BytesToHexArena(Dqn_Arena *arena, void const *src, Dqn_usize size)
{
    Dqn_Str8 result = {};
    result.data        = Dqn_Bin_BytesToHexBufferArena(arena, src, size);
    if (result.data)
        result.size = size * 2;
    return result;
}

DQN_API Dqn_usize Dqn_Bin_HexBufferToBytes(char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result = 0;
    if (!hex || hex_size <= 0)
        return result;

    Dqn_usize trim_size  = 0;
    char const *trim_hex = Dqn_Bin_HexBufferTrim0x(hex,
                                                                hex_size,
                                                                &trim_size);

    // NOTE: Trimmed hex can be "0xf" -> "f" or "0xAB" -> "AB"
    // Either way, the size can be odd or even, hence we round up to the nearest
    // multiple of two to ensure that we calculate the min buffer size orrectly.
    Dqn_usize trim_size_rounded_up = trim_size + (trim_size % 2);
    Dqn_usize min_buffer_size      = trim_size_rounded_up / 2;
    if (dest_size < min_buffer_size || trim_size <= 0) {
        DQN_ASSERTF(dest_size >= min_buffer_size, "Insufficient buffer size for converting hex to binary");
        return result;
    }

    result = Dqn_Bin_HexBufferToBytesUnchecked(trim_hex,
                                                   trim_size,
                                                   dest,
                                                   dest_size);
    return result;
}

DQN_API Dqn_usize Dqn_Bin_HexBufferToBytesUnchecked(char const *hex, Dqn_usize hex_size, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result       = 0;
    unsigned char *dest_u8 = DQN_CAST(unsigned char *)dest;

    for (Dqn_usize hex_index = 0;
         hex_index < hex_size;
         hex_index += 2, result += 1)
    {
        char hex01   = hex[hex_index];
        char hex02   = (hex_index + 1 < hex_size) ? hex[hex_index + 1] : 0;

        char bit4_01 =   (hex01 >= '0' && hex01 <= '9') ?  0 + (hex01 - '0')
                       : (hex01 >= 'a' && hex01 <= 'f') ? 10 + (hex01 - 'a')
                       : (hex01 >= 'A' && hex01 <= 'F') ? 10 + (hex01 - 'A')
                       : 0;

        char bit4_02 =   (hex02 >= '0' && hex02 <= '9') ?  0 + (hex02 - '0')
                       : (hex02 >= 'a' && hex02 <= 'f') ? 10 + (hex02 - 'a')
                       : (hex02 >= 'A' && hex02 <= 'F') ? 10 + (hex02 - 'A')
                       : 0;

        char byte       = (bit4_01 << 4) | (bit4_02 << 0);
        dest_u8[result] = byte;
    }

    DQN_ASSERT(result <= dest_size);
    return result;
}

DQN_API Dqn_usize Dqn_Bin_HexToBytesUnchecked(Dqn_Str8 hex, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result = Dqn_Bin_HexBufferToBytesUnchecked(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API Dqn_usize Dqn_Bin_HexToBytes(Dqn_Str8 hex, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result = Dqn_Bin_HexBufferToBytes(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API char *Dqn_Bin_HexBufferToBytesArena(Dqn_Arena *arena, char const *hex, Dqn_usize size, Dqn_usize *real_size)
{
    char *result = nullptr;
    if (!arena || !hex || size <= 0)
        return result;

    Dqn_usize   trim_size   = 0;
    char const *trim_hex    = Dqn_Bin_HexBufferTrim0x(hex, size, &trim_size);
    Dqn_usize   binary_size = trim_size / 2;
    result                  = Dqn_Arena_NewArray(arena, char, binary_size, Dqn_ZeroMem_No);
    if (result) {
        Dqn_usize convert_size = Dqn_Bin_HexBufferToBytesUnchecked(trim_hex, trim_size, result, binary_size);
        if (real_size)
            *real_size = convert_size;
    }
    return result;
}

DQN_API Dqn_Str8 Dqn_Bin_HexToBytesArena(Dqn_Arena *arena, Dqn_Str8 hex)
{
    Dqn_Str8 result = {};
    result.data     = Dqn_Bin_HexBufferToBytesArena(arena, hex.data, hex.size, &result.size);
    return result;
}
#endif // !defined(DQN_NO_BIN)

// NOTE: [$BITS] Dqn_Bit ===========================================================================
DQN_API void Dqn_Bit_UnsetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags & ~bitfield);
}

DQN_API void Dqn_Bit_SetInplace(uint64_t *flags, uint64_t bitfield)
{
    *flags = (*flags | bitfield);
}

DQN_API bool Dqn_Bit_IsSet(uint64_t bits, uint64_t bits_to_set)
{
    auto result = DQN_CAST(bool)((bits & bits_to_set) == bits_to_set);
    return result;
}

DQN_API bool Dqn_Bit_IsNotSet(uint64_t bits, uint64_t bits_to_check)
{
    auto result = !Dqn_Bit_IsSet(bits, bits_to_check);
    return result;
}

// NOTE: [$SAFE] Dqn_Safe ==========================================================================
DQN_API int64_t Dqn_Safe_AddI64(int64_t a, int64_t b)
{
    int64_t result = DQN_CHECKF(a <= INT64_MAX - b, "a=%zd, b=%zd", a, b) ? (a + b) : INT64_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_MulI64(int64_t a, int64_t b)
{
    int64_t result = DQN_CHECKF(a <= INT64_MAX / b, "a=%zd, b=%zd", a, b) ? (a * b) : INT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_AddU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a <= UINT64_MAX - b, "a=%zu, b=%zu", a, b) ? (a + b) : UINT64_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SubU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a >= b, "a=%zu, b=%zu", a, b) ? (a - b) : 0;
    return result;
}

DQN_API uint64_t Dqn_Safe_MulU64(uint64_t a, uint64_t b)
{
    uint64_t result = DQN_CHECKF(a <= UINT64_MAX / b, "a=%zu, b=%zu", a, b) ? (a * b) : UINT64_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SubU32(uint32_t a, uint32_t b)
{
    uint32_t result = DQN_CHECKF(a >= b, "a=%u, b=%u", a, b) ? (a - b) : 0;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToI*
// -----------------------------------------------------------------------------
// INT*_MAX literals will be promoted to the type of uintmax_t as uintmax_t is
// the highest possible rank (unsigned > signed).
DQN_API int Dqn_Safe_SaturateCastUSizeToInt(Dqn_usize val)
{
    int result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT_MAX) ? DQN_CAST(int)val : INT_MAX;
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastUSizeToI8(Dqn_usize val)
{
    int8_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT8_MAX) ? DQN_CAST(int8_t)val : INT8_MAX;
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastUSizeToI16(Dqn_usize val)
{
    int16_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT16_MAX) ? DQN_CAST(int16_t)val : INT16_MAX;
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastUSizeToI32(Dqn_usize val)
{
    int32_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT32_MAX) ? DQN_CAST(int32_t)val : INT32_MAX;
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastUSizeToI64(Dqn_usize val)
{
    int64_t result = DQN_CHECK(DQN_CAST(uintmax_t)val <= INT64_MAX) ? DQN_CAST(int64_t)val : INT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastUSizeToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API uint8_t Dqn_Safe_SaturateCastUSizeToU8(Dqn_usize val)
{
    uint8_t result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastUSizeToU16(Dqn_usize val)
{
    uint16_t result = DQN_CHECK(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastUSizeToU32(Dqn_usize val)
{
    uint32_t result = DQN_CHECK(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastUSizeToU64(Dqn_usize val)
{
    uint64_t result = DQN_CHECK(DQN_CAST(uint64_t)val <= UINT64_MAX) ? DQN_CAST(uint64_t)val : UINT64_MAX;
    return result;
}

// NOTE: Dqn_Safe_SaturateCastU64ToU*
// -----------------------------------------------------------------------------
// Both operands are unsigned and the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API unsigned int Dqn_Safe_SaturateCastU64ToUInt(uint64_t val)
{
    unsigned int result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(unsigned int)val : UINT_MAX;
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastU64ToU8(uint64_t val)
{
    uint8_t result = DQN_CHECK(val <= UINT8_MAX) ? DQN_CAST(uint8_t)val : UINT8_MAX;
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastU64ToU16(uint64_t val)
{
    uint16_t result = DQN_CHECK(val <= UINT16_MAX) ? DQN_CAST(uint16_t)val : UINT16_MAX;
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastU64ToU32(uint64_t val)
{
    uint32_t result = DQN_CHECK(val <= UINT32_MAX) ? DQN_CAST(uint32_t)val : UINT32_MAX;
    return result;
}


// NOTE: Dqn_Safe_SaturateCastISizeToI*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API int Dqn_Safe_SaturateCastISizeToInt(Dqn_isize val)
{
    DQN_ASSERT(val >= INT_MIN && val <= INT_MAX);
    int result = DQN_CAST(int)DQN_CLAMP(val, INT_MIN, INT_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastISizeToI8(Dqn_isize val)
{
    DQN_ASSERT(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastISizeToI16(Dqn_isize val)
{
    DQN_ASSERT(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastISizeToI32(Dqn_isize val)
{
    DQN_ASSERT(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

DQN_API int64_t Dqn_Safe_SaturateCastISizeToI64(Dqn_isize val)
{
    DQN_ASSERT(DQN_CAST(int64_t)val >= INT64_MIN && DQN_CAST(int64_t)val <= INT64_MAX);
    int64_t result = DQN_CAST(int64_t)DQN_CLAMP(DQN_CAST(int64_t)val, INT64_MIN, INT64_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastISizeToU*
// -----------------------------------------------------------------------------
// If the value is a negative integer, we clamp to 0. Otherwise, we know that
// the value is >=0, we can upcast safely to bounds check against the maximum
// allowed value.
DQN_API unsigned int Dqn_Safe_SaturateCastISizeToUInt(Dqn_isize val)
{
    unsigned int result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT_MAX))
            result = DQN_CAST(unsigned int)val;
        else
            result = UINT_MAX;
    }
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastISizeToU8(Dqn_isize val)
{
    uint8_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastISizeToU16(Dqn_isize val)
{
    uint16_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastISizeToU32(Dqn_isize val)
{
    uint32_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT32_MAX))
            result = DQN_CAST(uint32_t)val;
        else
            result = UINT32_MAX;
    }
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastISizeToU64(Dqn_isize val)
{
    uint64_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT64_MAX))
            result = DQN_CAST(uint64_t)val;
        else
            result = UINT64_MAX;
    }
    return result;
}

// NOTE: Dqn_Safe_SaturateCastI64To*
// -----------------------------------------------------------------------------
// Both operands are signed so the lowest rank operand will be promoted to
// match the highest rank operand.
DQN_API Dqn_isize Dqn_Safe_SaturateCastI64ToISize(int64_t val)
{
    DQN_CHECK(val >= DQN_ISIZE_MIN && val <= DQN_ISIZE_MAX);
    Dqn_isize result = DQN_CAST(int64_t)DQN_CLAMP(val, DQN_ISIZE_MIN, DQN_ISIZE_MAX);
    return result;
}

DQN_API int8_t Dqn_Safe_SaturateCastI64ToI8(int64_t val)
{
    DQN_CHECK(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastI64ToI16(int64_t val)
{
    DQN_CHECK(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API int32_t Dqn_Safe_SaturateCastI64ToI32(int64_t val)
{
    DQN_CHECK(val >= INT32_MIN && val <= INT32_MAX);
    int32_t result = DQN_CAST(int32_t)DQN_CLAMP(val, INT32_MIN, INT32_MAX);
    return result;
}

// NOTE: Dqn_Safe_SaturateCastIntTo*
// -----------------------------------------------------------------------------
DQN_API int8_t Dqn_Safe_SaturateCastIntToI8(int val)
{
    DQN_CHECK(val >= INT8_MIN && val <= INT8_MAX);
    int8_t result = DQN_CAST(int8_t)DQN_CLAMP(val, INT8_MIN, INT8_MAX);
    return result;
}

DQN_API int16_t Dqn_Safe_SaturateCastIntToI16(int val)
{
    DQN_CHECK(val >= INT16_MIN && val <= INT16_MAX);
    int16_t result = DQN_CAST(int16_t)DQN_CLAMP(val, INT16_MIN, INT16_MAX);
    return result;
}

DQN_API uint8_t Dqn_Safe_SaturateCastIntToU8(int val)
{
    uint8_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT8_MAX))
            result = DQN_CAST(uint8_t)val;
        else
            result = UINT8_MAX;
    }
    return result;
}

DQN_API uint16_t Dqn_Safe_SaturateCastIntToU16(int val)
{
    uint16_t result = 0;
    if (DQN_CHECK(val >= DQN_CAST(Dqn_isize)0)) {
        if (DQN_CHECK(DQN_CAST(uintmax_t)val <= UINT16_MAX))
            result = DQN_CAST(uint16_t)val;
        else
            result = UINT16_MAX;
    }
    return result;
}

DQN_API uint32_t Dqn_Safe_SaturateCastIntToU32(int val)
{
    static_assert(sizeof(val) <= sizeof(uint32_t), "Sanity check to allow simplifying of casting");
    uint32_t result = 0;
    if (DQN_CHECK(val >= 0))
        result = DQN_CAST(uint32_t)val;
    return result;
}

DQN_API uint64_t Dqn_Safe_SaturateCastIntToU64(int val)
{
    static_assert(sizeof(val) <= sizeof(uint64_t), "Sanity check to allow simplifying of casting");
    uint64_t result = 0;
    if (DQN_CHECK(val >= 0))
        result = DQN_CAST(uint64_t)val;
    return result;
}

// NOTE: [$MISC] Misc ==============================================================================
DQN_API int Dqn_SNPrintFDotTruncate(char *buffer, int size, DQN_FMT_ATTRIB char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    int size_required = STB_SPRINTF_DECORATE(vsnprintf)(buffer, size, fmt, args);
    int result        = DQN_MAX(DQN_MIN(size_required, size - 1), 0);
    if (result == size - 1) {
        buffer[size - 2] = '.';
        buffer[size - 3] = '.';
    }
    va_end(args);
    return result;
}

DQN_API Dqn_U64Str8 Dqn_U64ToStr8(uint64_t val, char separator)
{
    Dqn_U64Str8 result = {};
    if (val == 0) {
        result.data[result.size++] = '0';
    } else {
        // NOTE: The number is written in reverse because we form the string by
        // dividing by 10, so we write it in, then reverse it out after all is
        // done.
        Dqn_U64Str8 temp = {};
        for (Dqn_usize digit_count = 0; val > 0; digit_count++) {
            if (separator && (digit_count != 0) && (digit_count % 3 == 0))
                temp.data[temp.size++] = separator;

            auto digit             = DQN_CAST(char)(val % 10);
            temp.data[temp.size++] = '0' + digit;
            val /= 10;
        }

        // NOTE: Reverse the string
        DQN_MSVC_WARNING_PUSH
        DQN_MSVC_WARNING_DISABLE(6293) // Ill-defined for-loop
        DQN_MSVC_WARNING_DISABLE(6385) // Reading invalid data from 'temp.data' NOTE(doyle): Unsigned overflow is valid for loop termination
        for (Dqn_usize temp_index = temp.size - 1; temp_index < temp.size; temp_index--) {
            char ch = temp.data[temp_index];
            result.data[result.size++] = ch;
        }
        DQN_MSVC_WARNING_POP
    }

    return result;
}

// NOTE: [$DLIB] Dqn_Library =======================================================================
Dqn_Library *g_dqn_library;

DQN_API Dqn_Library *Dqn_Library_Init(Dqn_LibraryOnInit on_init)
{
    if (!g_dqn_library) {
        static Dqn_Library default_instance = {};
        g_dqn_library                       = &default_instance;
    }

    // NOTE: Init check ===========================================================================

    Dqn_Library *result = g_dqn_library;
    Dqn_TicketMutex_Begin(&result->lib_mutex);
    DQN_DEFER { Dqn_TicketMutex_End(&result->lib_mutex); };
    if (result->lib_init)
        return result;

    // NOTE: Query OS page size ====================================================================

    {
        #if defined(DQN_OS_WIN32)
        SYSTEM_INFO system_info = {};
        GetSystemInfo(&system_info);
        result->os_page_size         = system_info.dwPageSize;
        result->os_alloc_granularity = system_info.dwAllocationGranularity;
        #else
        // TODO(doyle): Get the proper page size from the OS.
        result->os_page_size         = DQN_KILOBYTES(4);
        result->os_alloc_granularity = DQN_KILOBYTES(64);
        #endif
    }

    // NOTE Initialise fields ======================================================================

    #if !defined(DQN_NO_PROFILER)
    result->profiler = &result->profiler_default_instance;
    #endif

    result->lib_init = true;
    Dqn_ArenaCatalog_Init(&result->arena_catalog, &result->arena);
    result->exe_dir  = Dqn_OS_EXEDir(&result->arena);

    // NOTE: Leak tracing ==========================================================================

    #if defined(DQN_LEAK_TRACING) // NOTE: Initialise the allocation leak tracker
    {
        result->alloc_tracking_disabled = true; // TODO(doyle): @robust Does this need to be atomic?

        Dqn_Str8 sample_backtrace                    = Dqn_Str8_InitCStr8(b_stacktrace_get_string());
        Dqn_Str8 clean_backtrace                     = Dqn_Debug_CleanStackTrace(sample_backtrace);
        result->stack_trace_offset_to_our_call_stack = DQN_CAST(uint16_t)(sample_backtrace.size - clean_backtrace.size);
        free(sample_backtrace.data);

        result->alloc_table             = Dqn_DSMap_Init<Dqn_AllocRecord>(4096);
        result->alloc_tracking_disabled = false;
    }
    #endif

    // NOTE: Print out init features ===============================================================
    if (on_init == Dqn_LibraryOnInit_LogFeatures) {
        Dqn_Log_DebugF("Dqn Library initialised:\n");

        // NOTE: %$$_I32u is a stb_sprintf format specifier, non-standard
        DQN_MSVC_WARNING_PUSH
        DQN_MSVC_WARNING_DISABLE(6271) // Extra argument passed to 'Dqn_Print_StdLnF'.
        Dqn_Print_StdLnF(Dqn_PrintStd_Err, "  OS Page Size/Alloc Granularity: %$$_I32u/%$$_I32u", result->os_page_size, result->os_alloc_granularity);
        DQN_MSVC_WARNING_POP

        #if __has_feature(address_sanitizer) || defined(__SANITIZE_ADDRESS__)
        if (DQN_ASAN_POISON) {
            Dqn_Print_StdLnF(Dqn_PrintStd_Err, "  ASAN manual poisoning%s", DQN_ASAN_VET_POISON ? " (+vet sanity checks)" : "");
            Dqn_Print_StdLnF(Dqn_PrintStd_Err, "  ASAN poison guard size: %$$_I32u", DQN_ASAN_POISON_GUARD_SIZE);
        }
        #endif

        #if defined(DQN_LEAK_TRACING)
        Dqn_Print_StdLnF(Dqn_PrintStd_Err, "  Allocation leak tracing");
        #endif

        #if !defined(DQN_NO_PROFILER)
        Dqn_Print_StdLnF(Dqn_PrintStd_Err, "  TSC profiler available");
        #endif

        // TODO(doyle): Add stacktrace feature log

        Dqn_Print_StdLnF(Dqn_PrintStd_Err, "");
    }
    return result;
}

DQN_API void Dqn_Library_SetPointer(Dqn_Library *library)
{
    if (library)
        g_dqn_library = library;
}

#if !defined(DQN_NO_PROFILER)
DQN_API void Dqn_Library_SetProfiler(Dqn_Profiler *profiler)
{
    if (profiler)
        g_dqn_library->profiler = profiler;
}
#endif

DQN_API void Dqn_Library_SetLogCallback(Dqn_LogProc *proc, void *user_data)
{
    g_dqn_library->log_callback  = proc;
    g_dqn_library->log_user_data = user_data;
}

DQN_API void Dqn_Library_DumpThreadContextArenaStat(Dqn_Str8 file_path)
{
    #if defined(DQN_DEBUG_THREAD_CONTEXT)
    // NOTE: Open a file to write the arena stats to
    FILE *file = nullptr;
    fopen_s(&file, file_path.data, "a+b");
    if (file) {
        Dqn_Log_ErrorF("Failed to dump thread context arenas [file=%.*s]", DQN_STR_FMT(file_path));
        return;
    }

    // NOTE: Copy the stats from library book-keeping
    // NOTE: Extremely short critical section, copy the stats then do our
    // work on it.
    Dqn_ArenaStat stats[Dqn_CArray_CountI(g_dqn_library->thread_context_arena_stats)];
    int stats_size = 0;

    Dqn_TicketMutex_Begin(&g_dqn_library->thread_context_mutex);
    stats_size = g_dqn_library->thread_context_arena_stats_count;
    DQN_MEMCPY(stats, g_dqn_library->thread_context_arena_stats, sizeof(stats[0]) * stats_size);
    Dqn_TicketMutex_End(&g_dqn_library->thread_context_mutex);

    // NOTE: Print the cumulative stat
    Dqn_DateHMSTimeStr now = Dqn_Date_HMSLocalTimeStrNow();
    fprintf(file,
            "Time=%.*s %.*s | Thread Context Arenas | Count=%d\n",
            now.date_size, now.date,
            now.hms_size, now.hms,
            g_dqn_library->thread_context_arena_stats_count);

    // NOTE: Write the cumulative thread arena data
    {
        Dqn_ArenaStat stat = {};
        for (Dqn_usize index = 0; index < stats_size; index++) {
            Dqn_ArenaStat const *current = stats + index;
            stat.capacity += current->capacity;
            stat.used     += current->used;
            stat.wasted   += current->wasted;
            stat.blocks   += current->blocks;

            stat.capacity_hwm = DQN_MAX(stat.capacity_hwm, current->capacity_hwm);
            stat.used_hwm     = DQN_MAX(stat.used_hwm, current->used_hwm);
            stat.wasted_hwm   = DQN_MAX(stat.wasted_hwm, current->wasted_hwm);
            stat.blocks_hwm   = DQN_MAX(stat.blocks_hwm, current->blocks_hwm);
        }

        Dqn_ArenaStatStr stats_string = Dqn_Arena_StatStr(&stat);
        fprintf(file, "  [ALL] CURR %.*s\n", stats_string.size, stats_string.data);
    }

    // NOTE: Print individual thread arena data
    for (Dqn_usize index = 0; index < stats_size; index++) {
        Dqn_ArenaStat const *current = stats + index;
        Dqn_ArenaStatStr current_string = Dqn_Arena_StatStr(current);
        fprintf(file, "  [%03d] CURR %.*s\n", DQN_CAST(int)index, current_string.size, current_string.data);
    }

    fclose(file);
    Dqn_Log_InfoF("Dumped thread context arenas [file=%.*s]", DQN_STR_FMT(file_path));
    #else
    (void)file_path;
    #endif // #if defined(DQN_DEBUG_THREAD_CONTEXT)
}


#if !defined(DQN_NO_PROFILER)
// NOTE: [$PROF] Dqn_Profiler ======================================================================
Dqn_ProfilerZoneScope::Dqn_ProfilerZoneScope(Dqn_Str8 name, uint16_t anchor_index)
{
    zone = Dqn_Profiler_BeginZoneWithIndex(name, anchor_index);
}

Dqn_ProfilerZoneScope::~Dqn_ProfilerZoneScope()
{
    Dqn_Profiler_EndZone(zone);
}

Dqn_ProfilerZone Dqn_Profiler_BeginZoneWithIndex(Dqn_Str8 name, uint16_t anchor_index)
{
    Dqn_ProfilerAnchor *anchor           = Dqn_Profiler_AnchorBuffer(Dqn_ProfilerAnchorBuffer_Back) + anchor_index;
    anchor->name                         = name;
    Dqn_ProfilerZone result              = {};
    result.begin_tsc                     = Dqn_CPU_TSC();
    result.anchor_index                  = anchor_index;
    result.parent_zone                   = g_dqn_library->profiler->parent_zone;
    result.elapsed_tsc_at_zone_start     = anchor->tsc_inclusive;
    g_dqn_library->profiler->parent_zone = anchor_index;
    return result;
}

void Dqn_Profiler_EndZone(Dqn_ProfilerZone zone)
{
    uint64_t elapsed_tsc              = Dqn_CPU_TSC() - zone.begin_tsc;
    Dqn_ProfilerAnchor *anchor_buffer = Dqn_Profiler_AnchorBuffer(Dqn_ProfilerAnchorBuffer_Back);
    Dqn_ProfilerAnchor *anchor        = anchor_buffer + zone.anchor_index;

    anchor->hit_count++;
    anchor->tsc_inclusive  = zone.elapsed_tsc_at_zone_start + elapsed_tsc;
    anchor->tsc_exclusive += elapsed_tsc;

    Dqn_ProfilerAnchor *parent_anchor     = anchor_buffer + zone.parent_zone;
    parent_anchor->tsc_exclusive         -= elapsed_tsc;
    g_dqn_library->profiler->parent_zone  = zone.parent_zone;
}

Dqn_ProfilerAnchor *Dqn_Profiler_AnchorBuffer(Dqn_ProfilerAnchorBuffer buffer)
{
    uint8_t offset             = buffer == Dqn_ProfilerAnchorBuffer_Back ? 0 : 1;
    uint8_t anchor_buffer      = (g_dqn_library->profiler->active_anchor_buffer + offset) % DQN_ARRAY_UCOUNT(g_dqn_library->profiler->anchors);
    Dqn_ProfilerAnchor *result = g_dqn_library->profiler->anchors[anchor_buffer];
    return result;
}

void Dqn_Profiler_SwapAnchorBuffer()
{
    g_dqn_library->profiler->active_anchor_buffer++;
    Dqn_ProfilerAnchor *anchors = Dqn_Profiler_AnchorBuffer(Dqn_ProfilerAnchorBuffer_Back);
    DQN_MEMSET(anchors, 0, DQN_ARRAY_UCOUNT(g_dqn_library->profiler->anchors[0]) * sizeof(g_dqn_library->profiler->anchors[0][0]));
}

void Dqn_Profiler_Dump(uint64_t tsc_per_second)
{
    Dqn_ProfilerAnchor *anchors = Dqn_Profiler_AnchorBuffer(Dqn_ProfilerAnchorBuffer_Back);
    for (size_t anchor_index = 1; anchor_index < DQN_PROFILER_ANCHOR_BUFFER_SIZE; anchor_index++) {
        Dqn_ProfilerAnchor const *anchor   = anchors + anchor_index;
        if (!anchor->hit_count)
            continue;

        uint64_t tsc_exclusive             = anchor->tsc_exclusive;
        uint64_t tsc_inclusive             = anchor->tsc_inclusive;
        Dqn_f64 tsc_exclusive_milliseconds = tsc_exclusive * 1000 / DQN_CAST(Dqn_f64)tsc_per_second;
        if (tsc_exclusive == tsc_inclusive) {
            Dqn_Print_LnF("%.*s[%u]: %.1fms",
                          DQN_STR_FMT(anchor->name),
                          anchor->hit_count,
                          tsc_exclusive_milliseconds);
        } else {
            Dqn_f64 tsc_inclusive_milliseconds        = tsc_inclusive * 1000 / DQN_CAST(Dqn_f64)tsc_per_second;
            Dqn_Print_LnF("%.*s[%u]: %.1f/%.1fms",
                          DQN_STR_FMT(anchor->name),
                          anchor->hit_count,
                          tsc_exclusive_milliseconds,
                          tsc_inclusive_milliseconds);
        }
    }
}
#endif // !defined(DQN_NO_PROFILER)
