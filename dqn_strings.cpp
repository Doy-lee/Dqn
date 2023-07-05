// NOTE: [$CSTR] Dqn_CString8 ======================================================================
DQN_API Dqn_usize Dqn_CString8_FSize(char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_usize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args);
    va_end(args);
    return result;
}

DQN_API Dqn_usize Dqn_CString8_FVSize(char const *fmt, va_list args)
{
    va_list args_copy;
    va_copy(args_copy, args);
    Dqn_usize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args_copy);
    va_end(args_copy);
    return result;
}


DQN_API Dqn_usize Dqn_CString8_Size(char const *src)
{
    Dqn_usize result = 0;
    while (src && src[0] != 0) {
        src++;
        result++;
    }
    return result;
}

DQN_API Dqn_usize Dqn_CString16_Size(wchar_t const *src)
{
    Dqn_usize result = 0;
    while (src && src[0] != 0) {
        src++;
        result++;
    }

    return result;
}

// NOTE: [$STR8] Dqn_String8 =======================================================================
DQN_API Dqn_String8 Dqn_String8_InitCString8(char const *src)
{
    Dqn_usize size     = Dqn_CString8_Size(src);
    Dqn_String8 result = Dqn_String8_Init(src, size);
    return result;
}

DQN_API bool Dqn_String8_IsValid(Dqn_String8 string)
{
    bool result = string.data;
    return result;
}

DQN_API bool Dqn_String8_IsAll(Dqn_String8 string, Dqn_String8IsAll is_all)
{
    bool result = Dqn_String8_IsValid(string);
    if (!result)
        return result;

    switch (is_all) {
        case Dqn_String8IsAll_Digits: {
            for (Dqn_usize index = 0; result && index < string.size; index++)
                result = string.data[index] >= '0' && string.data[index] <= '9';
        } break;

        case Dqn_String8IsAll_Hex: {
            Dqn_String8 trimmed = Dqn_String8_TrimPrefix(string, DQN_STRING8("0x"), Dqn_String8EqCase_Insensitive);
            for (Dqn_usize index = 0; result && index < string.size; index++) {
                char ch = trimmed.data[index];
                result  = (ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F');
            }
        } break;
    }

    return result;
}

DQN_API Dqn_String8 Dqn_String8_Slice(Dqn_String8 string, Dqn_usize offset, Dqn_usize size)
{
    Dqn_String8 result = Dqn_String8_Init(string.data, 0);
    if (!Dqn_String8_IsValid(result))
        return result;

    Dqn_usize capped_offset = DQN_MIN(offset, string.size);
    Dqn_usize max_size      = string.size - capped_offset;
    Dqn_usize capped_size   = DQN_MIN(size, max_size);
    result                  = Dqn_String8_Init(string.data + capped_offset, capped_size);
    return result;
}

DQN_API Dqn_String8BinarySplitResult Dqn_String8_BinarySplitArray(Dqn_String8 string, Dqn_String8 const *find, Dqn_usize find_size)
{
    Dqn_String8BinarySplitResult result = {};
    if (!Dqn_String8_IsValid(string) || !find || find_size == 0)
        return result;

    result.lhs = string;
    for (size_t index = 0; index <= string.size; index++) {
        for (Dqn_usize find_index = 0; find_index < find_size; find_index++) {
            Dqn_String8 find_item    = find[find_index];
            Dqn_String8 string_slice = Dqn_String8_Slice(string, index, find_item.size);
            if (Dqn_String8_Eq(string_slice, find_item)) {
                result.lhs.size = index;
                result.rhs.data = string_slice.data + find_item.size;
                result.rhs.size = string.size - (index + find_item.size);
                break;
            }
        }
    }

    return result;
}

DQN_API Dqn_String8BinarySplitResult Dqn_String8_BinarySplit(Dqn_String8 string, Dqn_String8 find)
{
    Dqn_String8BinarySplitResult result = Dqn_String8_BinarySplitArray(string, &find, 1);
    return result;
}

DQN_API Dqn_usize Dqn_String8_Split(Dqn_String8 string, Dqn_String8 delimiter, Dqn_String8 *splits, Dqn_usize splits_count)
{
    Dqn_usize result = 0; // The number of splits in the actual string.
    if (!Dqn_String8_IsValid(string) || !Dqn_String8_IsValid(delimiter) || delimiter.size <= 0)
        return result;

    Dqn_usize splits_index = 0; // The number of splits written.
    Dqn_usize begin        = 0;
    for (Dqn_usize index = 0; index < string.size; ) {
        // NOTE: Check if we encountered the substring that is the delimiter
        Dqn_String8 check = Dqn_String8_Slice(string, index, delimiter.size);
        if (!Dqn_String8_Eq(check, delimiter)) {
            index++;
            continue;
        }

        // NOTE: Generate the split
        Dqn_String8 split = Dqn_String8_Init(string.data + begin, index - begin);
        if (splits && splits_index < splits_count && split.size)
            splits[splits_index++] = split;

        // NOTE: Advance the iterators
        result += (split.size > 0);
        index  += delimiter.size;
        begin   = index;
    }

    return result;
}

DQN_API Dqn_String8FindResult Dqn_String8_FindFirstStringArray(Dqn_String8 string, Dqn_String8 const *find, Dqn_usize find_size)
{
    Dqn_String8FindResult result = {};
    if (!Dqn_String8_IsValid(string) || !find || find_size == 0)
        return result;

    for (Dqn_usize index = 0; !result.found && index < string.size; index++) {
        for (Dqn_usize find_index = 0; find_index < find_size; find_index++) {
            Dqn_String8 find_item    = find[find_index];
            Dqn_String8 string_slice = Dqn_String8_Slice(string, index, find_item.size);
            if (Dqn_String8_Eq(string_slice, find_item)) {
                result.found                  = true;
                result.index                  = index;
                result.match                  = Dqn_String8_Init(string.data + index, find_item.size);
                result.match_to_end_of_buffer = Dqn_String8_Init(result.match.data, string.size - index);
                break;
            }
        }
    }
    return result;
}

DQN_API Dqn_String8FindResult Dqn_String8_FindFirstString(Dqn_String8 string, Dqn_String8 find)
{
    Dqn_String8FindResult result = Dqn_String8_FindFirstStringArray(string, &find, 1);
    return result;
}

DQN_API Dqn_String8FindResult Dqn_String8_FindFirst(Dqn_String8 string, uint32_t flags)
{
    Dqn_String8FindResult result = {};
    for (size_t index = 0; !result.found && index < string.size; index++) {
        result.found |= ((flags & Dqn_String8FindFlag_Digit)      && Dqn_Char_IsDigit(string.data[index]));
        result.found |= ((flags & Dqn_String8FindFlag_Alphabet)   && Dqn_Char_IsAlphabet(string.data[index]));
        result.found |= ((flags & Dqn_String8FindFlag_Whitespace) && Dqn_Char_IsWhitespace(string.data[index]));
        result.found |= ((flags & Dqn_String8FindFlag_Plus)       && string.data[index] == '+');
        result.found |= ((flags & Dqn_String8FindFlag_Minus)      && string.data[index] == '-');
        if (result.found) {
            result.index                  = index;
            result.match                  = Dqn_String8_Init(string.data + index, 1);
            result.match_to_end_of_buffer = Dqn_String8_Init(result.match.data, string.size - index);
        }
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Segment(Dqn_Allocator allocator, Dqn_String8 src, Dqn_usize segment_size, char segment_char)
{
    Dqn_usize result_size = src.size;
    if (result_size > segment_size)
        result_size += (src.size / segment_size) - 1; // NOTE: No segment on the first chunk.

    Dqn_String8 result    = Dqn_String8_Allocate(allocator, result_size, Dqn_ZeroMem_Yes);
    Dqn_usize write_index = 0;
    DQN_FOR_UINDEX(src_index, src.size) {
        result.data[write_index++] = src.data[src_index];
        if ((src_index + 1) % segment_size == 0 && (src_index + 1) < src.size)
            result.data[write_index++] = segment_char;
        DQN_ASSERTF(write_index <= result.size, "result.size=%zu, write_index=%zu", result.size, write_index);
    }

    DQN_ASSERTF(write_index == result.size, "result.size=%zu, write_index=%zu", result.size, write_index);
    return result;
}


DQN_API bool Dqn_String8_Eq(Dqn_String8 lhs, Dqn_String8 rhs, Dqn_String8EqCase eq_case)
{
    if (lhs.size != rhs.size)
        return false;

    if (lhs.size == 0)
        return true;

    if (!lhs.data || !rhs.data)
        return false;

    bool result = true;
    switch (eq_case) {
        case Dqn_String8EqCase_Sensitive: {
            result = (DQN_MEMCMP(lhs.data, rhs.data, lhs.size) == 0);
        } break;

        case Dqn_String8EqCase_Insensitive: {
            for (Dqn_usize index = 0; index < lhs.size && result; index++)
                result = (Dqn_Char_ToLower(lhs.data[index]) == Dqn_Char_ToLower(rhs.data[index]));
        } break;
    }
    return result;
}

DQN_API bool Dqn_String8_EqInsensitive(Dqn_String8 lhs, Dqn_String8 rhs)
{
    bool result = Dqn_String8_Eq(lhs, rhs, Dqn_String8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_String8_StartsWith(Dqn_String8 string, Dqn_String8 prefix, Dqn_String8EqCase eq_case)
{
    Dqn_String8 substring = {string.data, DQN_MIN(prefix.size, string.size)};
    bool result           = Dqn_String8_Eq(substring, prefix, eq_case);
    return result;
}

DQN_API bool Dqn_String8_StartsWithInsensitive(Dqn_String8 string, Dqn_String8 prefix)
{
    bool result = Dqn_String8_StartsWith(string, prefix, Dqn_String8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_String8_EndsWith(Dqn_String8 string, Dqn_String8 suffix, Dqn_String8EqCase eq_case)
{
    Dqn_String8 substring = {string.data + string.size - suffix.size, DQN_MIN(string.size, suffix.size)};
    bool result           = Dqn_String8_Eq(substring, suffix, eq_case);
    return result;
}

DQN_API bool Dqn_String8_EndsWithInsensitive(Dqn_String8 string, Dqn_String8 suffix)
{
    bool result = Dqn_String8_EndsWith(string, suffix, Dqn_String8EqCase_Insensitive);
    return result;
}

DQN_API bool Dqn_String8_HasChar(Dqn_String8 string, char ch)
{
    bool result = false;
    for (Dqn_usize index = 0; !result && index < string.size; index++)
        result = string.data[index] == ch;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimPrefix(Dqn_String8 string, Dqn_String8 prefix, Dqn_String8EqCase eq_case)
{
    Dqn_String8 result = string;
    if (Dqn_String8_StartsWith(string, prefix, eq_case)) {
        result.data += prefix.size;
        result.size -= prefix.size;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimSuffix(Dqn_String8 string, Dqn_String8 suffix, Dqn_String8EqCase eq_case)
{
    Dqn_String8 result = string;
    if (Dqn_String8_EndsWith(string, suffix, eq_case))
        result.size -= suffix.size;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimWhitespaceAround(Dqn_String8 string)
{
    Dqn_String8 result = string;
    if (!Dqn_String8_IsValid(string))
        return result;

    char const *start = string.data;
    char const *end   = string.data + string.size;
    while (start < end && Dqn_Char_IsWhitespace(start[0]))
        start++;

    while (end > start && Dqn_Char_IsWhitespace(end[-1]))
        end--;

    result = Dqn_String8_Init(start, end - start);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_TrimByteOrderMark(Dqn_String8 string)
{
    Dqn_String8 result = string;
    if (!Dqn_String8_IsValid(result))
        return result;

    // TODO(dqn): This is little endian
    Dqn_String8 UTF8_BOM     = DQN_STRING8("\xEF\xBB\xBF");
    Dqn_String8 UTF16_BOM_BE = DQN_STRING8("\xEF\xFF");
    Dqn_String8 UTF16_BOM_LE = DQN_STRING8("\xFF\xEF");
    Dqn_String8 UTF32_BOM_BE = DQN_STRING8("\x00\x00\xFE\xFF");
    Dqn_String8 UTF32_BOM_LE = DQN_STRING8("\xFF\xFE\x00\x00");

    result = Dqn_String8_TrimPrefix(result, UTF8_BOM,     Dqn_String8EqCase_Sensitive);
    result = Dqn_String8_TrimPrefix(result, UTF16_BOM_BE, Dqn_String8EqCase_Sensitive);
    result = Dqn_String8_TrimPrefix(result, UTF16_BOM_LE, Dqn_String8EqCase_Sensitive);
    result = Dqn_String8_TrimPrefix(result, UTF32_BOM_BE, Dqn_String8EqCase_Sensitive);
    result = Dqn_String8_TrimPrefix(result, UTF32_BOM_LE, Dqn_String8EqCase_Sensitive);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_FileNameFromPath(Dqn_String8 path)
{
    Dqn_String8 result = path;
    if (!Dqn_String8_IsValid(result))
        return result;

    for (Dqn_usize i = result.size - 1; i < result.size; --i) {
        if (result.data[i] == '\\' || result.data[i] == '/') {
            char const *end = result.data + result.size;
            result.data     = result.data + (i + 1);
            result.size     = end - result.data;
            break;
        }
    }
    return result;
}

DQN_API bool Dqn_String8_ToU64Checked(Dqn_String8 string, char separator, uint64_t *output)
{
    // NOTE: Argument check
    if (!Dqn_String8_IsValid(string))
        return false;

    // NOTE: Sanitize input/output
    *output = 0;
    Dqn_String8 trim_string = Dqn_String8_TrimWhitespaceAround(string);
    if (trim_string.size == 0)
        return true;

    // NOTE: Handle prefix '+'
    Dqn_usize start_index = 0;
    if (!Dqn_Char_IsDigit(trim_string.data[0])) {
        if (trim_string.data[0] != '+')
            return false;
        start_index++;
    }

    // NOTE: Convert the string number to the binary number
    for (Dqn_usize index = start_index; index < trim_string.size; index++) {
        char ch = trim_string.data[index];
        if (index) {
            if (separator != 0 && ch == separator)
                continue;
        }

        if (!Dqn_Char_IsDigit(ch))
            return false;

        *output        = Dqn_Safe_MulU64(*output, 10);
        uint64_t digit = ch - '0';
        *output        = Dqn_Safe_AddU64(*output, digit);
    }

    return true;
}

DQN_API uint64_t Dqn_String8_ToU64(Dqn_String8 string, char separator)
{
    uint64_t result = 0;
    Dqn_String8_ToU64Checked(string, separator, &result);
    return result;
}

DQN_API bool Dqn_String8_ToI64Checked(Dqn_String8 string, char separator, int64_t *output)
{
    // NOTE: Argument check
    if (!Dqn_String8_IsValid(string))
        return false;

    // NOTE: Sanitize input/output
    *output = 0;
    Dqn_String8 trim_string = Dqn_String8_TrimWhitespaceAround(string);
    if (trim_string.size == 0)
        return true;

    bool negative         = false;
    Dqn_usize start_index = 0;
    if (!Dqn_Char_IsDigit(trim_string.data[0])) {
        negative = (trim_string.data[start_index] == '-');
        if (!negative && trim_string.data[0] != '+')
            return false;
        start_index++;
    }

    // NOTE: Convert the string number to the binary number
    for (Dqn_usize index = start_index; index < trim_string.size; index++) {
        char ch = trim_string.data[index];
        if (index) {
            if (separator != 0 && ch == separator)
                continue;
        }

        if (!Dqn_Char_IsDigit(ch))
            return false;

        *output        = Dqn_Safe_MulU64(*output, 10);
        uint64_t digit = ch - '0';
        *output        = Dqn_Safe_AddU64(*output, digit);
    }

    if (negative)
        *output *= -1;

    return true;
}

DQN_API int64_t Dqn_String8_ToI64(Dqn_String8 string, char separator)
{
    int64_t result = 0;
    Dqn_String8_ToI64Checked(string, separator, &result);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Replace(Dqn_String8 string,
                                        Dqn_String8 find,
                                        Dqn_String8 replace,
                                        Dqn_usize start_index,
                                        Dqn_Allocator allocator,
                                        Dqn_String8EqCase eq_case)
{
    Dqn_String8 result = {};
    if (!Dqn_String8_IsValid(string) || !Dqn_String8_IsValid(find) || find.size > string.size || find.size == 0 || string.size == 0) {
        result = Dqn_String8_Copy(allocator, string);
        return result;
    }

    Dqn_ThreadScratch scratch         = Dqn_Thread_GetScratch(allocator.user_context);
    Dqn_String8Builder string_builder = {};
    string_builder.allocator          = scratch.allocator;
    Dqn_usize max                     = string.size - find.size;
    Dqn_usize head                    = start_index;

    for (Dqn_usize tail = head; tail <= max; tail++) {
        Dqn_String8 check = Dqn_String8_Slice(string, tail, find.size);
        if (!Dqn_String8_Eq(check, find, eq_case))
            continue;

        if (start_index > 0 && string_builder.string_size == 0) {
            // User provided a hint in the string to start searching from, we
            // need to add the string up to the hint. We only do this if there's
            // a replacement action, otherwise we have a special case for no
            // replacements, where the entire string gets copied.
            Dqn_String8 slice = Dqn_String8_Init(string.data, head);
            Dqn_String8Builder_AppendRef(&string_builder, slice);
        }

        Dqn_String8 range = Dqn_String8_Slice(string, head, (tail - head));
        Dqn_String8Builder_AppendRef(&string_builder, range);
        Dqn_String8Builder_AppendRef(&string_builder, replace);
        head = tail + find.size;
        tail += find.size - 1; // NOTE: -1 since the for loop will post increment us past the end of the find string
    }

    if (string_builder.string_size == 0) {
        // NOTE: No replacement possible, so we just do a full-copy
        result = Dqn_String8_Copy(allocator, string);
    } else {
        Dqn_String8 remainder = Dqn_String8_Init(string.data + head, string.size - head);
        Dqn_String8Builder_AppendRef(&string_builder, remainder);
        result = Dqn_String8Builder_Build(&string_builder, allocator);
    }

    return result;
}

DQN_API Dqn_String8 Dqn_String8_ReplaceInsensitive(Dqn_String8 string, Dqn_String8 find, Dqn_String8 replace, Dqn_usize start_index, Dqn_Allocator allocator)
{
    Dqn_String8 result = Dqn_String8_Replace(string, find, replace, start_index, allocator, Dqn_String8EqCase_Insensitive);
    return result;
}

DQN_API void Dqn_String8_Remove(Dqn_String8 *string, Dqn_usize offset, Dqn_usize size)
{
    if (!string || !Dqn_String8_IsValid(*string))
        return;

    char *end               = string->data + string->size;
    char *dest              = DQN_MIN(string->data + offset,        end);
    char *src               = DQN_MIN(string->data + offset + size, end);
    Dqn_usize bytes_to_move = end - src;
    DQN_MEMMOVE(dest, src, bytes_to_move);
    string->size -= bytes_to_move;
}

#if defined(__cplusplus)
DQN_API bool operator==(Dqn_String8 const &lhs, Dqn_String8 const &rhs)
{
    bool result = Dqn_String8_Eq(lhs, rhs, Dqn_String8EqCase_Sensitive);
    return result;
}

DQN_API bool operator!=(Dqn_String8 const &lhs, Dqn_String8 const &rhs)
{
    bool result = !(lhs == rhs);
    return result;
}
#endif

DQN_API Dqn_String8 Dqn_String8_InitF_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_String8 result = Dqn_String8_InitFV_(DQN_LEAK_TRACE_ARG allocator, fmt, va);
    va_end(va);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_InitFV_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *fmt, va_list args)
{
    Dqn_String8 result = {};
    if (!fmt)
        return result;

    Dqn_usize size = Dqn_CString8_FVSize(fmt, args);
    if (size) {
        result = Dqn_String8_Allocate_(DQN_LEAK_TRACE_ARG allocator, size, Dqn_ZeroMem_No);
        if (Dqn_String8_IsValid(result))
            STB_SPRINTF_DECORATE(vsnprintf)(result.data, Dqn_Safe_SaturateCastISizeToInt(size + 1 /*null-terminator*/), fmt, args);
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Allocate_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_usize size, Dqn_ZeroMem zero_mem)
{
    Dqn_String8 result = {};
    result.data        = (char *)Dqn_Allocator_Alloc_(DQN_LEAK_TRACE_ARG allocator, size + 1, alignof(char), zero_mem);
    if (result.data)
        result.size = size;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_CopyCString_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, char const *string, Dqn_usize size)
{
    Dqn_String8 result = {};
    if (!string)
        return result;

    result = Dqn_String8_Allocate_(DQN_LEAK_TRACE_ARG allocator, size, Dqn_ZeroMem_No);
    if (Dqn_String8_IsValid(result)) {
        DQN_MEMCPY(result.data, string, size);
        result.data[size] = 0;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Copy_(DQN_LEAK_TRACE_FUNCTION Dqn_Allocator allocator, Dqn_String8 string)
{
    Dqn_String8 result = Dqn_String8_CopyCString_(DQN_LEAK_TRACE_ARG allocator, string.data, string.size);
    return result;
}

// NOTE: [$STRB] Dqn_String8Builder ================================================================
DQN_API bool Dqn_String8Builder_AppendRef(Dqn_String8Builder *builder, Dqn_String8 string)
{
    if (!builder || !string.data || string.size <= 0)
        return false;

    Dqn_String8Link *link = Dqn_Allocator_New(builder->allocator, Dqn_String8Link, Dqn_ZeroMem_No);
    if (!link)
        return false;

    link->string = string;
    link->next   = NULL;

    if (builder->head)
        builder->tail->next = link;
    else
        builder->head = link;

    builder->tail = link;
    builder->count++;
    builder->string_size += string.size;
    return true;
}

DQN_API bool Dqn_String8Builder_AppendCopy(Dqn_String8Builder *builder, Dqn_String8 string)
{
    Dqn_String8 copy = Dqn_String8_Copy(builder->allocator, string);
    bool result      = Dqn_String8Builder_AppendRef(builder, copy);
    return result;
}

DQN_API bool Dqn_String8Builder_AppendFV_(DQN_LEAK_TRACE_FUNCTION Dqn_String8Builder *builder, char const *fmt, va_list args)
{
    Dqn_String8 string = Dqn_String8_InitFV(builder->allocator, fmt, args);
    if (string.size == 0)
        return true;

    bool result = Dqn_String8Builder_AppendRef(builder, string);
    if (!result)
        Dqn_Allocator_Dealloc_(DQN_LEAK_TRACE_ARG builder->allocator, string.data, string.size + 1);
    return result;
}

DQN_API bool Dqn_String8Builder_AppendF(Dqn_String8Builder *builder, char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    bool result = Dqn_String8Builder_AppendFV(builder, fmt, args);
    va_end(args);
    return result;
}

DQN_API Dqn_String8 Dqn_String8Builder_Build(Dqn_String8Builder const *builder, Dqn_Allocator allocator)
{
    Dqn_String8 result = DQN_ZERO_INIT;
    if (!builder || builder->string_size <= 0 || builder->count <= 0)
        return result;

    result.data = Dqn_Allocator_NewArray(allocator, char, builder->string_size + 1, Dqn_ZeroMem_No);
    if (!result.data)
        return result;

    for (Dqn_String8Link *link = builder->head; link; link = link->next) {
        DQN_MEMCPY(result.data + result.size, link->string.data, link->string.size);
        result.size += link->string.size;
    }

    result.data[result.size] = 0;
    DQN_ASSERT(result.size == builder->string_size);
    return result;
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

DQN_API Dqn_String8 Dqn_JSONBuilder_Build(Dqn_JSONBuilder const *builder, Dqn_Allocator allocator)
{
    Dqn_String8 result = Dqn_String8Builder_Build(&builder->string_builder, allocator);
    return result;
}

DQN_API void Dqn_JSONBuilder_KeyValue(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value)
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
        Dqn_String8Builder_AppendF(&builder->string_builder,
                                   "%.*s%*c\"%.*s\": %.*s",
                                   prefix_size, prefix,
                                   spaces, ' ',
                                   DQN_STRING_FMT(key),
                                   DQN_STRING_FMT(value));
    } else {
        Dqn_String8Builder_AppendF(&builder->string_builder,
                                   "%.*s%*c%.*s",
                                   prefix_size, prefix,
                                   spaces, ' ',
                                   DQN_STRING_FMT(value));
    }

    if (item == Dqn_JSONBuilderItem_OpenContainer)
        builder->indent_level++;

    builder->last_item = item;
}

DQN_API void Dqn_JSONBuilder_KeyValueFV(Dqn_JSONBuilder *builder, Dqn_String8 key, char const *value_fmt, va_list args)
{
    Dqn_ThreadScratch scratch = Dqn_Thread_GetScratch(builder->string_builder.allocator.user_context);
    Dqn_String8 value         = Dqn_String8_InitFV(scratch.allocator, value_fmt, args);
    Dqn_JSONBuilder_KeyValue(builder, key, value);
}

DQN_API void Dqn_JSONBuilder_KeyValueF(Dqn_JSONBuilder *builder, Dqn_String8 key, char const *value_fmt, ...)
{
    va_list args;
    va_start(args, value_fmt);
    Dqn_JSONBuilder_KeyValueFV(builder, key, value_fmt, args);
    va_end(args);
}

DQN_API void Dqn_JSONBuilder_ObjectBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name)
{
    Dqn_JSONBuilder_KeyValue(builder, name, DQN_STRING8("{"));
}

DQN_API void Dqn_JSONBuilder_ObjectEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_KeyValue(builder, DQN_STRING8(""), DQN_STRING8("}"));
}

DQN_API void Dqn_JSONBuilder_ArrayBeginNamed(Dqn_JSONBuilder *builder, Dqn_String8 name)
{
    Dqn_JSONBuilder_KeyValue(builder, name, DQN_STRING8("["));
}

DQN_API void Dqn_JSONBuilder_ArrayEnd(Dqn_JSONBuilder *builder)
{
    Dqn_JSONBuilder_KeyValue(builder, DQN_STRING8(""), DQN_STRING8("]"));
}

DQN_API void Dqn_JSONBuilder_StringNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "\"%.*s\"", value.size, value.data);
}

DQN_API void Dqn_JSONBuilder_LiteralNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, Dqn_String8 value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%.*s", value.size, value.data);
}

DQN_API void Dqn_JSONBuilder_U64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, uint64_t value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%I64u", value);
}

DQN_API void Dqn_JSONBuilder_I64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, int64_t value)
{
    Dqn_JSONBuilder_KeyValueF(builder, key, "%I64d", value);
}

DQN_API void Dqn_JSONBuilder_F64Named(Dqn_JSONBuilder *builder, Dqn_String8 key, double value, int decimal_places)
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

DQN_API void Dqn_JSONBuilder_BoolNamed(Dqn_JSONBuilder *builder, Dqn_String8 key, bool value)
{
    Dqn_String8 value_string = value ? DQN_STRING8("true") : DQN_STRING8("false");
    Dqn_JSONBuilder_KeyValueF(builder, key, "%.*s", value_string.size, value_string.data);
}
#endif // !defined(DQN_NO_JSON_BUILDER)

// NOTE: [$CHAR] Dqn_Char ==========================================================================
DQN_API bool Dqn_Char_IsAlphabet(char ch)
{
    bool result = (ch >= 'A' && ch <= 'Z') || (ch >= 'a' && ch <= 'z');
    return result;
}

DQN_API bool Dqn_Char_IsDigit(char ch)
{
    bool result = (ch >= '0' && ch <= '9');
    return result;
}

DQN_API bool Dqn_Char_IsAlphaNum(char ch)
{
    bool result = Dqn_Char_IsAlphabet(ch) || Dqn_Char_IsDigit(ch);
    return result;
}

DQN_API bool Dqn_Char_IsWhitespace(char ch)
{
    bool result = (ch == ' ' || ch == '\t' || ch == '\n' || ch == '\r');
    return result;
}

DQN_API bool Dqn_Char_IsHex(char ch)
{
    bool result = ((ch >= 'a' && ch <= 'f') || (ch >= 'A' && ch <= 'F') || (ch >= '0' && ch <= '9'));
    return result;
}

DQN_API uint8_t Dqn_Char_HexToU8(char ch)
{
    DQN_ASSERTF(Dqn_Char_IsHex(ch), "Hex character not valid '%c'", ch);

    uint8_t result = 0;
    if (ch >= 'a' && ch <= 'f')
        result = ch - 'a' + 10;
    else if (ch >= 'A' && ch <= 'F')
        result = ch - 'A' + 10;
    else
        result = ch - '0';
    return result;
}

char constexpr DQN_HEX_LUT[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
DQN_API char Dqn_Char_ToHex(char ch)
{
    char result = DQN_CAST(char)-1;
    if (ch <= 16) result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_Char_ToHexUnchecked(char ch)
{
    char result = DQN_HEX_LUT[DQN_CAST(unsigned)ch];
    return result;
}

DQN_API char Dqn_Char_ToLower(char ch)
{
    char result = ch;
    if (result >= 'A' && result <= 'Z')
        result += 'a' - 'A';
    return result;
}

// NOTE: [$UTFX] Dqn_UTF ===========================================================================
DQN_API int Dqn_UTF8_EncodeCodepoint(uint8_t utf8[4], uint32_t codepoint)
{
    // NOTE: Table from https://www.reedbeta.com/blog/programmers-intro-to-unicode/
    // ----------------------------------------+----------------------------+--------------------+
    // UTF-8 (binary)                          | Code point (binary)        | Range              |
    // ----------------------------------------+----------------------------+--------------------+
    // 0xxx'xxxx                               |                   xxx'xxxx | U+0000  - U+007F   |
    // 110x'xxxx 10yy'yyyy                     |              xxx'xxyy'yyyy | U+0080  - U+07FF   |
    // 1110'xxxx 10yy'yyyy 10zz'zzzz           |        xxxx'yyyy'yyzz'zzzz | U+0800  - U+FFFF   |
    // 1111'0xxx 10yy'yyyy 10zz'zzzz 10ww'wwww | x'xxyy'yyyy'zzzz'zzww'wwww | U+10000 - U+10FFFF |
    // ----------------------------------------+----------------------------+--------------------+

    if (codepoint <= 0b0111'1111)
    {
        utf8[0] = DQN_CAST(uint8_t)codepoint;
        return 1;
    }

    if (codepoint <= 0b0111'1111'1111)
    {
        utf8[0] = (0b1100'0000 | ((codepoint >> 6) & 0b01'1111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 0) & 0b11'1111)); // y
        return 2;
    }

    if (codepoint <= 0b1111'1111'1111'1111)
    {
        utf8[0] = (0b1110'0000 | ((codepoint >> 12) & 0b00'1111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 6)  & 0b11'1111)); // y
        utf8[2] = (0b1000'0000 | ((codepoint >> 0)  & 0b11'1111)); // z
        return 3;
    }

    if (codepoint <= 0b1'1111'1111'1111'1111'1111)
    {
        utf8[0] = (0b1111'0000 | ((codepoint >> 18) & 0b00'0111)); // x
        utf8[1] = (0b1000'0000 | ((codepoint >> 12) & 0b11'1111)); // y
        utf8[2] = (0b1000'0000 | ((codepoint >> 6)  & 0b11'1111)); // z
        utf8[3] = (0b1000'0000 | ((codepoint >> 0)  & 0b11'1111)); // w
        return 4;
    }

    return 0;
}

DQN_API int Dqn_UTF16_EncodeCodepoint(uint16_t utf16[2], uint32_t codepoint)
{
    // NOTE: Table from https://www.reedbeta.com/blog/programmers-intro-to-unicode/
    // ----------------------------------------+------------------------------------+------------------+
    // UTF-16 (binary)                         | Code point (binary)                | Range            |
    // ----------------------------------------+------------------------------------+------------------+
    // xxxx'xxxx'xxxx'xxxx                     | xxxx'xxxx'xxxx'xxxx                | U+0000–U+FFFF    |
    // 1101'10xx'xxxx'xxxx 1101'11yy'yyyy'yyyy | xxxx'xxxx'xxyy'yyyy'yyyy + 0x10000 | U+10000–U+10FFFF |
    // ----------------------------------------+------------------------------------+------------------+

    if (codepoint <= 0b1111'1111'1111'1111)
    {
        utf16[0] = DQN_CAST(uint16_t)codepoint;
        return 1;
    }

    if (codepoint <= 0b1111'1111'1111'1111'1111)
    {
        uint32_t surrogate_codepoint = codepoint + 0x10000;
        utf16[0] = 0b1101'1000'0000'0000 | ((surrogate_codepoint >> 10) & 0b11'1111'1111); // x
        utf16[1] = 0b1101'1100'0000'0000 | ((surrogate_codepoint >>  0) & 0b11'1111'1111); // y
        return 2;
    }

    return 0;
}

#if !defined(DQN_NO_HEX)
// NOTE: [$BHEX] Dqn_Bin ===========================================================================
DQN_API char const *Dqn_Bin_HexBufferTrim0x(char const *hex, Dqn_usize size, Dqn_usize *real_size)
{
    Dqn_String8 result = Dqn_String8_TrimWhitespaceAround(Dqn_String8_Init(hex, size));
    result             = Dqn_String8_TrimPrefix(result, DQN_STRING8("0x"), Dqn_String8EqCase_Insensitive);
    if (real_size)
        *real_size = result.size;
    return result.data;
}

DQN_API Dqn_String8 Dqn_Bin_HexTrim0x(Dqn_String8 string)
{
    Dqn_usize trimmed_size = 0;
    char const *trimmed    = Dqn_Bin_HexBufferTrim0x(string.data, string.size, &trimmed_size);
    Dqn_String8 result     = Dqn_String8_Init(trimmed, trimmed_size);
    return result;
}

DQN_API Dqn_BinHexU64String Dqn_Bin_U64ToHexU64String(uint64_t number, uint32_t flags)
{
    Dqn_String8 prefix = {};
    if (!(flags & Dqn_BinHexU64StringFlags_No0xPrefix))
        prefix = DQN_STRING8("0x");

    Dqn_BinHexU64String result = {};
    DQN_MEMCPY(result.data, prefix.data, prefix.size);
    result.size += DQN_CAST(int8_t)prefix.size;

    char const *fmt = (flags & Dqn_BinHexU64StringFlags_UppercaseHex) ? "%I64X" : "%I64x";
    int size        = STB_SPRINTF_DECORATE(snprintf)(result.data + result.size, DQN_ARRAY_UCOUNT(result.data) - result.size, fmt, number);
    result.size    += DQN_CAST(uint8_t)size;
    DQN_ASSERT(result.size < DQN_ARRAY_UCOUNT(result.data));

    // NOTE: snprintf returns the required size of the format string
    // irrespective of if there's space or not, but, always null terminates so
    // the last byte is wasted.
    result.size = DQN_MIN(result.size, DQN_ARRAY_UCOUNT(result.data) - 1);
    return result;
}

DQN_API Dqn_String8 Dqn_Bin_U64ToHex(Dqn_Allocator allocator, uint64_t number, uint32_t flags)
{
    Dqn_String8 prefix = {};
    if (!(flags & Dqn_BinHexU64StringFlags_No0xPrefix))
        prefix = DQN_STRING8("0x");

    char const *fmt         = (flags & Dqn_BinHexU64StringFlags_UppercaseHex) ? "%I64X" : "%I64x";
    Dqn_usize required_size = Dqn_CString8_FSize(fmt, number) + prefix.size;
    Dqn_String8 result      = Dqn_String8_Allocate(allocator, required_size, Dqn_ZeroMem_No);

    if (Dqn_String8_IsValid(result)) {
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

    uint64_t   result      = 0;
    Dqn_usize bits_written = 0;
    Dqn_usize max_size     = DQN_MIN(size, 8 /*bytes*/ * 2 /*hex chars per byte*/);
    for (Dqn_usize hex_index = 0; hex_index < max_size; hex_index++, bits_written += 4) {
        char ch = trim_hex[hex_index];
        if (!Dqn_Char_IsHex(ch))
            break;
        uint8_t val = Dqn_Char_HexToU8(ch);
        Dqn_usize bit_shift = 60 - bits_written;
        result |= (DQN_CAST(uint64_t)val << bit_shift);
    }

    result >>= (64 - bits_written); // Shift the remainder digits to the end.
    return result;
}

DQN_API uint64_t Dqn_Bin_HexToU64(Dqn_String8 hex)
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

DQN_API Dqn_String8 Dqn_Bin_BytesToHexArena(Dqn_Arena *arena, void const *src, Dqn_usize size)
{
    Dqn_String8 result = {};
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

DQN_API Dqn_usize Dqn_Bin_HexToBytesUnchecked(Dqn_String8 hex, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result = Dqn_Bin_HexBufferToBytesUnchecked(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API Dqn_usize Dqn_Bin_HexToBytes(Dqn_String8 hex, void *dest, Dqn_usize dest_size)
{
    Dqn_usize result = Dqn_Bin_HexBufferToBytes(hex.data, hex.size, dest, dest_size);
    return result;
}

DQN_API char *Dqn_Bin_HexBufferToBytesArena(Dqn_Arena *arena, char const *hex, Dqn_usize size, Dqn_usize *real_size)
{
    char *result = nullptr;
    if (!arena || !hex || size <= 0)
        return result;

    Dqn_usize trim_size  = 0;
    char const *trim_hex = Dqn_Bin_HexBufferTrim0x(hex,
                                                                size,
                                                                &trim_size);

    Dqn_usize binary_size = trim_size / 2;
    result                = Dqn_Arena_NewArray(arena, char, binary_size, Dqn_ZeroMem_No);
    if (result) {
        Dqn_usize convert_size = Dqn_Bin_HexBufferToBytesUnchecked(trim_hex, trim_size, result, binary_size);
        if (real_size)
            *real_size = convert_size;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_Bin_HexToBytesArena(Dqn_Arena *arena, Dqn_String8 hex)
{
    Dqn_String8 result = {};
    result.data        = Dqn_Bin_HexBufferToBytesArena(arena, hex.data, hex.size, &result.size);
    return result;
}
#endif // !defined(DQN_NO_HEX)

// NOTE: Other =====================================================================================
DQN_API int Dqn_SNPrintFDotTruncate(char *buffer, int size, char const *fmt, ...)
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

DQN_API Dqn_U64String Dqn_U64ToString(uint64_t val, char separator)
{
    Dqn_U64String result = {};
    if (val == 0) {
        result.data[result.size++] = '0';
    } else {
        // NOTE: The number is written in reverse because we form the string by
        // dividing by 10, so we write it in, then reverse it out after all is
        // done.
        Dqn_U64String temp = {};
        for (size_t digit_count = 0; val > 0; digit_count++) {
            if (separator && (digit_count != 0) && (digit_count % 3 == 0)) {
                temp.data[temp.size++] = separator;
            }

            auto digit             = DQN_CAST(char)(val % 10);
            temp.data[temp.size++] = '0' + digit;
            val /= 10;
        }

        // NOTE: Reverse the string
        for (size_t temp_index = temp.size - 1; temp_index < temp.size; temp_index--) {
            char ch = temp.data[temp_index];
            result.data[result.size++] = ch;
        }
    }

    return result;
}

// NOTE: [$STBS] stb_sprintf =======================================================================
#if !defined(DQN_STB_SPRINTF_HEADER_ONLY)
#define STB_SPRINTF_IMPLEMENTATION
#ifdef STB_SPRINTF_IMPLEMENTATION

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
#if defined(__ppc64__) || defined(__powerpc64__) || defined(__aarch64__) || defined(_M_X64) || defined(__x86_64__) || defined(__x86_64) || defined(__s390x__)
#define stbsp__uintptr stbsp__uint64
#else
#define stbsp__uintptr stbsp__uint32
#endif
#endif

#ifndef STB_SPRINTF_MSVC_MODE // used for MSVC2013 and earlier (MSVC2015 matches GCC)
#if defined(_MSC_VER) && (_MSC_VER < 1900)
#define STB_SPRINTF_MSVC_MODE
#endif
#endif

#ifdef STB_SPRINTF_NOUNALIGNED // define this before inclusion to force stbsp_sprintf to always use aligned accesses
#define STBSP__UNALIGNED(code)
#else
#define STBSP__UNALIGNED(code) code
#endif

#ifndef STB_SPRINTF_NOFLOAT
// internal float utility functions
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits);
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value);
#define STBSP__SPECIAL 0x7000
#endif

static char stbsp__period = '.';
static char stbsp__comma = ',';
static struct
{
   short temp; // force next field to be 2-byte aligned
   char pair[201];
} stbsp__digitpair =
{
  0,
   "00010203040506070809101112131415161718192021222324"
   "25262728293031323334353637383940414243444546474849"
   "50515253545556575859606162636465666768697071727374"
   "75767778798081828384858687888990919293949596979899"
};

STBSP__PUBLICDEF void STB_SPRINTF_DECORATE(set_separators)(char pcomma, char pperiod)
{
   stbsp__period = pperiod;
   stbsp__comma = pcomma;
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
   if (fl & STBSP__NEGATIVE) {
      sign[0] = 1;
      sign[1] = '-';
   } else if (fl & STBSP__LEADINGSPACE) {
      sign[0] = 1;
      sign[1] = ' ';
   } else if (fl & STBSP__LEADINGPLUS) {
      sign[0] = 1;
      sign[1] = '+';
   }
}

static STBSP__ASAN stbsp__uint32 stbsp__strlen_limited(char const *s, stbsp__uint32 limit)
{
   char const * sn = s;

   // get up to 4-byte alignment
   for (;;) {
      if (((stbsp__uintptr)sn & 3) == 0)
         break;

      if (!limit || *sn == 0)
         return (stbsp__uint32)(sn - s);

      ++sn;
      --limit;
   }

   // scan over 4 bytes at a time to find terminating 0
   // this will intentionally scan up to 3 bytes past the end of buffers,
   // but becase it works 4B aligned, it will never cross page boundaries
   // (hence the STBSP__ASAN markup; the over-read here is intentional
   // and harmless)
   while (limit >= 4) {
      stbsp__uint32 v = *(stbsp__uint32 *)sn;
      // bit hack to find if there's a 0 byte in there
      if ((v - 0x01010101) & (~v) & 0x80808080UL)
         break;

      sn += 4;
      limit -= 4;
   }

   // handle the last few characters to find actual size
   while (limit && *sn) {
      ++sn;
      --limit;
   }

   return (stbsp__uint32)(sn - s);
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintfcb)(STBSP_SPRINTFCB *callback, void *user, char *buf, char const *fmt, va_list va)
{
   static char hex[] = "0123456789abcdefxp";
   static char hexu[] = "0123456789ABCDEFXP";
   char *bf;
   char const *f;
   int tlen = 0;

   bf = buf;
   f = fmt;
   for (;;) {
      stbsp__int32 fw, pr, tz;
      stbsp__uint32 fl;

      // macros for the callback buffer stuff
      #define stbsp__chk_cb_bufL(bytes)                        \
         {                                                     \
            int len = (int)(bf - buf);                         \
            if ((len + (bytes)) >= STB_SPRINTF_MIN) {          \
               tlen += len;                                    \
               if (0 == (bf = buf = callback(buf, user, len))) \
                  goto done;                                   \
            }                                                  \
         }
      #define stbsp__chk_cb_buf(bytes)    \
         {                                \
            if (callback) {               \
               stbsp__chk_cb_bufL(bytes); \
            }                             \
         }
      #define stbsp__flush_cb()                      \
         {                                           \
            stbsp__chk_cb_bufL(STB_SPRINTF_MIN - 1); \
         } // flush if there is even one byte in the buffer
      #define stbsp__cb_buf_clamp(cl, v)                \
         cl = v;                                        \
         if (callback) {                                \
            int lg = STB_SPRINTF_MIN - (int)(bf - buf); \
            if (cl > lg)                                \
               cl = lg;                                 \
         }

      // fast copy everything up to the next % (or end of string)
      for (;;) {
         while (((stbsp__uintptr)f) & 3) {
         schk1:
            if (f[0] == '%')
               goto scandd;
         schk2:
            if (f[0] == 0)
               goto endfmt;
            stbsp__chk_cb_buf(1);
            *bf++ = f[0];
            ++f;
         }
         for (;;) {
            // Check if the next 4 bytes contain %(0x25) or end of string.
            // Using the 'hasless' trick:
            // https://graphics.stanford.edu/~seander/bithacks.html#HasLessInWord
            stbsp__uint32 v, c;
            v = *(stbsp__uint32 *)f;
            c = (~v) & 0x80808080;
            if (((v ^ 0x25252525) - 0x01010101) & c)
               goto schk1;
            if ((v - 0x01010101) & c)
               goto schk2;
            if (callback)
               if ((STB_SPRINTF_MIN - (int)(bf - buf)) < 4)
                  goto schk1;
            #ifdef STB_SPRINTF_NOUNALIGNED
                if(((stbsp__uintptr)bf) & 3) {
                    bf[0] = f[0];
                    bf[1] = f[1];
                    bf[2] = f[2];
                    bf[3] = f[3];
                } else
            #endif
            {
                *(stbsp__uint32 *)bf = v;
            }
            bf += 4;
            f += 4;
         }
      }
   scandd:

      ++f;

      // ok, we have a percent, read the modifiers first
      fw = 0;
      pr = -1;
      fl = 0;
      tz = 0;

      // flags
      for (;;) {
         switch (f[0]) {
         // if we have left justify
         case '-':
            fl |= STBSP__LEFTJUST;
            ++f;
            continue;
         // if we have leading plus
         case '+':
            fl |= STBSP__LEADINGPLUS;
            ++f;
            continue;
         // if we have leading space
         case ' ':
            fl |= STBSP__LEADINGSPACE;
            ++f;
            continue;
         // if we have leading 0x
         case '#':
            fl |= STBSP__LEADING_0X;
            ++f;
            continue;
         // if we have thousand commas
         case '\'':
            fl |= STBSP__TRIPLET_COMMA;
            ++f;
            continue;
         // if we have kilo marker (none->kilo->kibi->jedec)
         case '$':
            if (fl & STBSP__METRIC_SUFFIX) {
               if (fl & STBSP__METRIC_1024) {
                  fl |= STBSP__METRIC_JEDEC;
               } else {
                  fl |= STBSP__METRIC_1024;
               }
            } else {
               fl |= STBSP__METRIC_SUFFIX;
            }
            ++f;
            continue;
         // if we don't want space between metric suffix and number
         case '_':
            fl |= STBSP__METRIC_NOSPACE;
            ++f;
            continue;
         // if we have leading zero
         case '0':
            fl |= STBSP__LEADINGZERO;
            ++f;
            goto flags_done;
         default: goto flags_done;
         }
      }
   flags_done:

      // get the field width
      if (f[0] == '*') {
         fw = va_arg(va, stbsp__uint32);
         ++f;
      } else {
         while ((f[0] >= '0') && (f[0] <= '9')) {
            fw = fw * 10 + f[0] - '0';
            f++;
         }
      }
      // get the precision
      if (f[0] == '.') {
         ++f;
         if (f[0] == '*') {
            pr = va_arg(va, stbsp__uint32);
            ++f;
         } else {
            pr = 0;
            while ((f[0] >= '0') && (f[0] <= '9')) {
               pr = pr * 10 + f[0] - '0';
               f++;
            }
         }
      }

      // handle integer size overrides
      switch (f[0]) {
      // are we halfwidth?
      case 'h':
         fl |= STBSP__HALFWIDTH;
         ++f;
         if (f[0] == 'h')
            ++f;  // QUARTERWIDTH
         break;
      // are we 64-bit (unix style)
      case 'l':
         fl |= ((sizeof(long) == 8) ? STBSP__INTMAX : 0);
         ++f;
         if (f[0] == 'l') {
            fl |= STBSP__INTMAX;
            ++f;
         }
         break;
      // are we 64-bit on intmax? (c99)
      case 'j':
         fl |= (sizeof(size_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit on size_t or ptrdiff_t? (c99)
      case 'z':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      case 't':
         fl |= (sizeof(ptrdiff_t) == 8) ? STBSP__INTMAX : 0;
         ++f;
         break;
      // are we 64-bit (msft style)
      case 'I':
         if ((f[1] == '6') && (f[2] == '4')) {
            fl |= STBSP__INTMAX;
            f += 3;
         } else if ((f[1] == '3') && (f[2] == '2')) {
            f += 3;
         } else {
            fl |= ((sizeof(void *) == 8) ? STBSP__INTMAX : 0);
            ++f;
         }
         break;
      default: break;
      }

      // handle each replacement
      switch (f[0]) {
         #define STBSP__NUMSZ 512 // big enough for e308 (with commas) or e-307
         char num[STBSP__NUMSZ];
         char lead[8];
         char tail[8];
         char *s;
         char const *h;
         stbsp__uint32 l, n, cs;
         stbsp__uint64 n64;
#ifndef STB_SPRINTF_NOFLOAT
         double fv;
#endif
         stbsp__int32 dp;
         char const *sn;

      case 's':
         // get the string
         s = va_arg(va, char *);
         if (s == 0)
            s = (char *)"null";
         // get the length, limited to desired precision
         // always limit to ~0u chars since our counts are 32b
         l = stbsp__strlen_limited(s, (pr >= 0) ? pr : ~0u);
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         // copy the string in
         goto scopy;

      case 'c': // char
         // get the character
         s = num + STBSP__NUMSZ - 1;
         *s = (char)va_arg(va, int);
         l = 1;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
         goto scopy;

      case 'n': // weird write-bytes specifier
      {
         int *d = va_arg(va, int *);
         *d = tlen + (int)(bf - buf);
      } break;

#ifdef STB_SPRINTF_NOFLOAT
      case 'A':              // float
      case 'a':              // hex float
      case 'G':              // float
      case 'g':              // float
      case 'E':              // float
      case 'e':              // float
      case 'f':              // float
         va_arg(va, double); // eat it
         s = (char *)"No float";
         l = 8;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         cs = 0;
         STBSP__NOTUSED(dp);
         goto scopy;
#else
      case 'A': // hex float
      case 'a': // hex float
         h = (f[0] == 'A') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_parts((stbsp__int64 *)&n64, &dp, fv))
            fl |= STBSP__NEGATIVE;

         s = num + 64;

         stbsp__lead_sign(fl, lead);

         if (dp == -1023)
            dp = (n64) ? -1022 : 0;
         else
            n64 |= (((stbsp__uint64)1) << 52);
         n64 <<= (64 - 56);
         if (pr < 15)
            n64 += ((((stbsp__uint64)8) << 56) >> (pr * 4));
// add leading chars

#ifdef STB_SPRINTF_MSVC_MODE
         *s++ = '0';
         *s++ = 'x';
#else
         lead[1 + lead[0]] = '0';
         lead[2 + lead[0]] = 'x';
         lead[0] += 2;
#endif
         *s++ = h[(n64 >> 60) & 15];
         n64 <<= 4;
         if (pr)
            *s++ = stbsp__period;
         sn = s;

         // print the bits
         n = pr;
         if (n > 13)
            n = 13;
         if (pr > (stbsp__int32)n)
            tz = pr - n;
         pr = 0;
         while (n--) {
            *s++ = h[(n64 >> 60) & 15];
            n64 <<= 4;
         }

         // print the expo
         tail[1] = h[17];
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
         n = (dp >= 1000) ? 6 : ((dp >= 100) ? 5 : ((dp >= 10) ? 4 : 3));
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }

         dp = (int)(s - sn);
         l = (int)(s - (num + 64));
         s = num + 64;
         cs = 1 + (3 << 24);
         goto scopy;

      case 'G': // float
      case 'g': // float
         h = (f[0] == 'G') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6;
         else if (pr == 0)
            pr = 1; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, (pr - 1) | 0x80000000))
            fl |= STBSP__NEGATIVE;

         // clamp the precision and delete extra zeros after clamp
         n = pr;
         if (l > (stbsp__uint32)pr)
            l = pr;
         while ((l > 1) && (pr) && (sn[l - 1] == '0')) {
            --pr;
            --l;
         }

         // should we use %e
         if ((dp <= -4) || (dp > (stbsp__int32)n)) {
            if (pr > (stbsp__int32)l)
               pr = l - 1;
            else if (pr)
               --pr; // when using %e, there is one digit before the decimal
            goto doexpfromg;
         }
         // this is the insane action to get the pr to match %g semantics for %f
         if (dp > 0) {
            pr = (dp < (stbsp__int32)l) ? l - dp : 0;
         } else {
            pr = -dp + ((pr > (stbsp__int32)l) ? (stbsp__int32) l : pr);
         }
         goto dofloatfromg;

      case 'E': // float
      case 'e': // float
         h = (f[0] == 'E') ? hexu : hex;
         fv = va_arg(va, double);
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr | 0x80000000))
            fl |= STBSP__NEGATIVE;
      doexpfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;
         // handle leading chars
         *s++ = sn[0];

         if (pr)
            *s++ = stbsp__period;

         // handle after decimal
         if ((l - 1) > (stbsp__uint32)pr)
            l = pr + 1;
         for (n = 1; n < l; n++)
            *s++ = sn[n];
         // trailing zeros
         tz = pr - (l - 1);
         pr = 0;
         // dump expo
         tail[1] = h[0xe];
         dp -= 1;
         if (dp < 0) {
            tail[2] = '-';
            dp = -dp;
         } else
            tail[2] = '+';
#ifdef STB_SPRINTF_MSVC_MODE
         n = 5;
#else
         n = (dp >= 100) ? 5 : 4;
#endif
         tail[0] = (char)n;
         for (;;) {
            tail[n] = '0' + dp % 10;
            if (n <= 3)
               break;
            --n;
            dp /= 10;
         }
         cs = 1 + (3 << 24); // how many tens
         goto flt_lead;

      case 'f': // float
         fv = va_arg(va, double);
      doafloat:
         // do kilos
         if (fl & STBSP__METRIC_SUFFIX) {
            double divisor;
            divisor = 1000.0f;
            if (fl & STBSP__METRIC_1024)
               divisor = 1024.0;
            while (fl < 0x4000000) {
               if ((fv < divisor) && (fv > -divisor))
                  break;
               fv /= divisor;
               fl += 0x1000000;
            }
         }
         if (pr == -1)
            pr = 6; // default is 6
         // read the double into a string
         if (stbsp__real_to_str(&sn, &l, num, &dp, fv, pr))
            fl |= STBSP__NEGATIVE;
      dofloatfromg:
         tail[0] = 0;
         stbsp__lead_sign(fl, lead);
         if (dp == STBSP__SPECIAL) {
            s = (char *)sn;
            cs = 0;
            pr = 0;
            goto scopy;
         }
         s = num + 64;

         // handle the three decimal varieties
         if (dp <= 0) {
            stbsp__int32 i;
            // handle 0.000*000xxxx
            *s++ = '0';
            if (pr)
               *s++ = stbsp__period;
            n = -dp;
            if ((stbsp__int32)n > pr)
               n = pr;
            i = n;
            while (i) {
               if ((((stbsp__uintptr)s) & 3) == 0)
                  break;
               *s++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)s = 0x30303030;
               s += 4;
               i -= 4;
            }
            while (i) {
               *s++ = '0';
               --i;
            }
            if ((stbsp__int32)(l + n) > pr)
               l = pr - n;
            i = l;
            while (i) {
               *s++ = *sn++;
               --i;
            }
            tz = pr - (n + l);
            cs = 1 + (3 << 24); // how many tens did we write (for commas below)
         } else {
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((600 - (stbsp__uint32)dp) % 3) : 0;
            if ((stbsp__uint32)dp >= l) {
               // handle xxxx000*000.0
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= l)
                        break;
                  }
               }
               if (n < (stbsp__uint32)dp) {
                  n = dp - n;
                  if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                     while (n) {
                        if ((((stbsp__uintptr)s) & 3) == 0)
                           break;
                        *s++ = '0';
                        --n;
                     }
                     while (n >= 4) {
                        *(stbsp__uint32 *)s = 0x30303030;
                        s += 4;
                        n -= 4;
                     }
                  }
                  while (n) {
                     if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                        cs = 0;
                        *s++ = stbsp__comma;
                     } else {
                        *s++ = '0';
                        --n;
                     }
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr) {
                  *s++ = stbsp__period;
                  tz = pr;
               }
            } else {
               // handle xxxxx.xxxx000*000
               n = 0;
               for (;;) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (++cs == 4)) {
                     cs = 0;
                     *s++ = stbsp__comma;
                  } else {
                     *s++ = sn[n];
                     ++n;
                     if (n >= (stbsp__uint32)dp)
                        break;
                  }
               }
               cs = (int)(s - (num + 64)) + (3 << 24); // cs is how many tens
               if (pr)
                  *s++ = stbsp__period;
               if ((l - dp) > (stbsp__uint32)pr)
                  l = pr + dp;
               while (n < l) {
                  *s++ = sn[n];
                  ++n;
               }
               tz = pr - (l - dp);
            }
         }
         pr = 0;

         // handle k,m,g,t
         if (fl & STBSP__METRIC_SUFFIX) {
            char idx;
            idx = 1;
            if (fl & STBSP__METRIC_NOSPACE)
               idx = 0;
            tail[0] = idx;
            tail[1] = ' ';
            {
               if (fl >> 24) { // SI kilo is 'k', JEDEC and SI kibits are 'K'.
                  if (fl & STBSP__METRIC_1024)
                     tail[idx + 1] = "_KMGT"[fl >> 24];
                  else
                     tail[idx + 1] = "_kMGT"[fl >> 24];
                  idx++;
                  // If printing kibits and not in jedec, add the 'i'.
                  if (fl & STBSP__METRIC_1024 && !(fl & STBSP__METRIC_JEDEC)) {
                     tail[idx + 1] = 'i';
                     idx++;
                  }
                  tail[0] = idx;
               }
            }
         };

      flt_lead:
         // get the length that we copied
         l = (stbsp__uint32)(s - (num + 64));
         s = num + 64;
         goto scopy;
#endif

      case 'B': // upper binary
      case 'b': // lower binary
         h = (f[0] == 'B') ? hexu : hex;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[0xb];
         }
         l = (8 << 4) | (1 << 8);
         goto radixnum;

      case 'o': // octal
         h = hexu;
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 1;
            lead[1] = '0';
         }
         l = (3 << 4) | (3 << 8);
         goto radixnum;

      case 'p': // pointer
         fl |= (sizeof(void *) == 8) ? STBSP__INTMAX : 0;
         pr = sizeof(void *) * 2;
         fl &= ~STBSP__LEADINGZERO; // 'p' only prints the pointer with zeros
                                    // fall through - to X

      case 'X': // upper hex
      case 'x': // lower hex
         h = (f[0] == 'X') ? hexu : hex;
         l = (4 << 4) | (4 << 8);
         lead[0] = 0;
         if (fl & STBSP__LEADING_0X) {
            lead[0] = 2;
            lead[1] = '0';
            lead[2] = h[16];
         }
      radixnum:
         // get the number
         if (fl & STBSP__INTMAX)
            n64 = va_arg(va, stbsp__uint64);
         else
            n64 = va_arg(va, stbsp__uint32);

         s = num + STBSP__NUMSZ;
         dp = 0;
         // clear tail, and clear leading if value is zero
         tail[0] = 0;
         if (n64 == 0) {
            lead[0] = 0;
            if (pr == 0) {
               l = 0;
               cs = 0;
               goto scopy;
            }
         }
         // convert to string
         for (;;) {
            *--s = h[n64 & ((1 << (l >> 8)) - 1)];
            n64 >>= (l >> 8);
            if (!((n64) || ((stbsp__int32)((num + STBSP__NUMSZ) - s) < pr)))
               break;
            if (fl & STBSP__TRIPLET_COMMA) {
               ++l;
               if ((l & 15) == ((l >> 4) & 15)) {
                  l &= ~15;
                  *--s = stbsp__comma;
               }
            }
         };
         // get the tens and the comma pos
         cs = (stbsp__uint32)((num + STBSP__NUMSZ) - s) + ((((l >> 4) & 15)) << 24);
         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         // copy it
         goto scopy;

      case 'u': // unsigned
      case 'i':
      case 'd': // integer
         // get the integer and abs it
         if (fl & STBSP__INTMAX) {
            stbsp__int64 i64 = va_arg(va, stbsp__int64);
            n64 = (stbsp__uint64)i64;
            if ((f[0] != 'u') && (i64 < 0)) {
               n64 = (stbsp__uint64)-i64;
               fl |= STBSP__NEGATIVE;
            }
         } else {
            stbsp__int32 i = va_arg(va, stbsp__int32);
            n64 = (stbsp__uint32)i;
            if ((f[0] != 'u') && (i < 0)) {
               n64 = (stbsp__uint32)-i;
               fl |= STBSP__NEGATIVE;
            }
         }

#ifndef STB_SPRINTF_NOFLOAT
         if (fl & STBSP__METRIC_SUFFIX) {
            if (n64 < 1024)
               pr = 0;
            else if (pr == -1)
               pr = 1;
            fv = (double)(stbsp__int64)n64;
            goto doafloat;
         }
#endif

         // convert to string
         s = num + STBSP__NUMSZ;
         l = 0;

         for (;;) {
            // do in 32-bit chunks (avoid lots of 64-bit divides even with constant denominators)
            char *o = s - 8;
            if (n64 >= 100000000) {
               n = (stbsp__uint32)(n64 % 100000000);
               n64 /= 100000000;
            } else {
               n = (stbsp__uint32)n64;
               n64 = 0;
            }
            if ((fl & STBSP__TRIPLET_COMMA) == 0) {
               do {
                  s -= 2;
                  *(stbsp__uint16 *)s = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
                  n /= 100;
               } while (n);
            }
            while (n) {
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = (char)(n % 10) + '0';
                  n /= 10;
               }
            }
            if (n64 == 0) {
               if ((s[0] == '0') && (s != (num + STBSP__NUMSZ)))
                  ++s;
               break;
            }
            while (s != o)
               if ((fl & STBSP__TRIPLET_COMMA) && (l++ == 3)) {
                  l = 0;
                  *--s = stbsp__comma;
                  --o;
               } else {
                  *--s = '0';
               }
         }

         tail[0] = 0;
         stbsp__lead_sign(fl, lead);

         // get the length that we copied
         l = (stbsp__uint32)((num + STBSP__NUMSZ) - s);
         if (l == 0) {
            *--s = '0';
            l = 1;
         }
         cs = l + (3 << 24);
         if (pr < 0)
            pr = 0;

      scopy:
         // get fw=leading/trailing space, pr=leading zeros
         if (pr < (stbsp__int32)l)
            pr = l;
         n = pr + lead[0] + tail[0] + tz;
         if (fw < (stbsp__int32)n)
            fw = n;
         fw -= n;
         pr -= l;

         // handle right justify and leading zeros
         if ((fl & STBSP__LEFTJUST) == 0) {
            if (fl & STBSP__LEADINGZERO) // if leading zeros, everything is in pr
            {
               pr = (fw > pr) ? fw : pr;
               fw = 0;
            } else {
               fl &= ~STBSP__TRIPLET_COMMA; // if no leading zeros, then no commas
            }
         }

         // copy the spaces and/or zeros
         if (fw + pr) {
            stbsp__int32 i;
            stbsp__uint32 c;

            // copy leading spaces (or when doing %8.4d stuff)
            if ((fl & STBSP__LEFTJUST) == 0)
               while (fw > 0) {
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i) {
                     *bf++ = ' ';
                     --i;
                  }
                  stbsp__chk_cb_buf(1);
               }

            // copy leader
            sn = lead + 1;
            while (lead[0]) {
               stbsp__cb_buf_clamp(i, lead[0]);
               lead[0] -= (char)i;
               while (i) {
                  *bf++ = *sn++;
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }

            // copy leading zeros
            c = cs >> 24;
            cs &= 0xffffff;
            cs = (fl & STBSP__TRIPLET_COMMA) ? ((stbsp__uint32)(c - ((pr + cs) % (c + 1)))) : 0;
            while (pr > 0) {
               stbsp__cb_buf_clamp(i, pr);
               pr -= i;
               if ((fl & STBSP__TRIPLET_COMMA) == 0) {
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = '0';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x30303030;
                     bf += 4;
                     i -= 4;
                  }
               }
               while (i) {
                  if ((fl & STBSP__TRIPLET_COMMA) && (cs++ == c)) {
                     cs = 0;
                     *bf++ = stbsp__comma;
                  } else
                     *bf++ = '0';
                  --i;
               }
               stbsp__chk_cb_buf(1);
            }
         }

         // copy leader if there is still one
         sn = lead + 1;
         while (lead[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, lead[0]);
            lead[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy the string
         n = l;
         while (n) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, n);
            n -= i;
            STBSP__UNALIGNED(while (i >= 4) {
               *(stbsp__uint32 volatile *)bf = *(stbsp__uint32 volatile *)s;
               bf += 4;
               s += 4;
               i -= 4;
            })
            while (i) {
               *bf++ = *s++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy trailing zeros
         while (tz) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tz);
            tz -= i;
            while (i) {
               if ((((stbsp__uintptr)bf) & 3) == 0)
                  break;
               *bf++ = '0';
               --i;
            }
            while (i >= 4) {
               *(stbsp__uint32 *)bf = 0x30303030;
               bf += 4;
               i -= 4;
            }
            while (i) {
               *bf++ = '0';
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // copy tail if there is one
         sn = tail + 1;
         while (tail[0]) {
            stbsp__int32 i;
            stbsp__cb_buf_clamp(i, tail[0]);
            tail[0] -= (char)i;
            while (i) {
               *bf++ = *sn++;
               --i;
            }
            stbsp__chk_cb_buf(1);
         }

         // handle the left justify
         if (fl & STBSP__LEFTJUST)
            if (fw > 0) {
               while (fw) {
                  stbsp__int32 i;
                  stbsp__cb_buf_clamp(i, fw);
                  fw -= i;
                  while (i) {
                     if ((((stbsp__uintptr)bf) & 3) == 0)
                        break;
                     *bf++ = ' ';
                     --i;
                  }
                  while (i >= 4) {
                     *(stbsp__uint32 *)bf = 0x20202020;
                     bf += 4;
                     i -= 4;
                  }
                  while (i--)
                     *bf++ = ' ';
                  stbsp__chk_cb_buf(1);
               }
            }
         break;

      default: // unknown, just copy code
         s = num + STBSP__NUMSZ - 1;
         *s = f[0];
         l = 1;
         fw = fl = 0;
         lead[0] = 0;
         tail[0] = 0;
         pr = 0;
         dp = 0;
         cs = 0;
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
   return tlen + (int)(bf - buf);
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

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(sprintf)(char *buf, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);
   result = STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
   va_end(va);
   return result;
}

typedef struct stbsp__context {
   char *buf;
   int count;
   int length;
   char tmp[STB_SPRINTF_MIN];
} stbsp__context;

static char *stbsp__clamp_callback(const char *buf, void *user, int len)
{
   stbsp__context *c = (stbsp__context *)user;
   c->length += len;

   if (len > c->count)
      len = c->count;

   if (len) {
      if (buf != c->buf) {
         const char *s, *se;
         char *d;
         d = c->buf;
         s = buf;
         se = buf + len;
         do {
            *d++ = *s++;
         } while (s < se);
      }
      c->buf += len;
      c->count -= len;
   }

   if (c->count <= 0)
      return c->tmp;
   return (c->count >= STB_SPRINTF_MIN) ? c->buf : c->tmp; // go direct into buffer if you can
}

static char * stbsp__count_clamp_callback( const char * buf, void * user, int len )
{
   stbsp__context * c = (stbsp__context*)user;
   (void) sizeof(buf);

   c->length += len;
   return c->tmp; // go direct into buffer if you can
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE( vsnprintf )( char * buf, int count, char const * fmt, va_list va )
{
   stbsp__context c;

   if ( (count == 0) && !buf )
   {
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__count_clamp_callback, &c, c.tmp, fmt, va );
   }
   else
   {
      int l;

      c.buf = buf;
      c.count = count;
      c.length = 0;

      STB_SPRINTF_DECORATE( vsprintfcb )( stbsp__clamp_callback, &c, stbsp__clamp_callback(0,&c,0), fmt, va );

      // zero-terminate
      l = (int)( c.buf - buf );
      if ( l >= count ) // should never be greater, only equal (or less) than count
         l = count - 1;
      buf[l] = 0;
   }

   return c.length;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(snprintf)(char *buf, int count, char const *fmt, ...)
{
   int result;
   va_list va;
   va_start(va, fmt);

   result = STB_SPRINTF_DECORATE(vsnprintf)(buf, count, fmt, va);
   va_end(va);

   return result;
}

STBSP__PUBLICDEF int STB_SPRINTF_DECORATE(vsprintf)(char *buf, char const *fmt, va_list va)
{
   return STB_SPRINTF_DECORATE(vsprintfcb)(0, 0, buf, fmt, va);
}

// =======================================================================
//   low level float utility functions

#ifndef STB_SPRINTF_NOFLOAT

// copies d to bits w/ strict aliasing (this compiles to nothing on /Ox)
#define STBSP__COPYFP(dest, src)                   \
   {                                               \
      int cn;                                      \
      for (cn = 0; cn < 8; cn++)                   \
         ((char *)&dest)[cn] = ((char *)&src)[cn]; \
   }

// get float info
static stbsp__int32 stbsp__real_to_parts(stbsp__int64 *bits, stbsp__int32 *expo, double value)
{
   double d;
   stbsp__int64 b = 0;

   // load value and round at the frac_digits
   d = value;

   STBSP__COPYFP(b, d);

   *bits = b & ((((stbsp__uint64)1) << 52) - 1);
   *expo = (stbsp__int32)(((b >> 52) & 2047) - 1023);

   return (stbsp__int32)((stbsp__uint64) b >> 63);
}

static double const stbsp__bot[23] = {
   1e+000, 1e+001, 1e+002, 1e+003, 1e+004, 1e+005, 1e+006, 1e+007, 1e+008, 1e+009, 1e+010, 1e+011,
   1e+012, 1e+013, 1e+014, 1e+015, 1e+016, 1e+017, 1e+018, 1e+019, 1e+020, 1e+021, 1e+022
};
static double const stbsp__negbot[22] = {
   1e-001, 1e-002, 1e-003, 1e-004, 1e-005, 1e-006, 1e-007, 1e-008, 1e-009, 1e-010, 1e-011,
   1e-012, 1e-013, 1e-014, 1e-015, 1e-016, 1e-017, 1e-018, 1e-019, 1e-020, 1e-021, 1e-022
};
static double const stbsp__negboterr[22] = {
   -5.551115123125783e-018,  -2.0816681711721684e-019, -2.0816681711721686e-020, -4.7921736023859299e-021, -8.1803053914031305e-022, 4.5251888174113741e-023,
   4.5251888174113739e-024,  -2.0922560830128471e-025, -6.2281591457779853e-026, -3.6432197315497743e-027, 6.0503030718060191e-028,  2.0113352370744385e-029,
   -3.0373745563400371e-030, 1.1806906454401013e-032,  -7.7705399876661076e-032, 2.0902213275965398e-033,  -7.1542424054621921e-034, -7.1542424054621926e-035,
   2.4754073164739869e-036,  5.4846728545790429e-037,  9.2462547772103625e-038,  -4.8596774326570872e-039
};
static double const stbsp__top[13] = {
   1e+023, 1e+046, 1e+069, 1e+092, 1e+115, 1e+138, 1e+161, 1e+184, 1e+207, 1e+230, 1e+253, 1e+276, 1e+299
};
static double const stbsp__negtop[13] = {
   1e-023, 1e-046, 1e-069, 1e-092, 1e-115, 1e-138, 1e-161, 1e-184, 1e-207, 1e-230, 1e-253, 1e-276, 1e-299
};
static double const stbsp__toperr[13] = {
   8388608,
   6.8601809640529717e+028,
   -7.253143638152921e+052,
   -4.3377296974619174e+075,
   -1.5559416129466825e+098,
   -3.2841562489204913e+121,
   -3.7745893248228135e+144,
   -1.7356668416969134e+167,
   -3.8893577551088374e+190,
   -9.9566444326005119e+213,
   6.3641293062232429e+236,
   -5.2069140800249813e+259,
   -5.2504760255204387e+282
};
static double const stbsp__negtoperr[13] = {
   3.9565301985100693e-040,  -2.299904345391321e-063,  3.6506201437945798e-086,  1.1875228833981544e-109,
   -5.0644902316928607e-132, -6.7156837247865426e-155, -2.812077463003139e-178,  -5.7778912386589953e-201,
   7.4997100559334532e-224,  -4.6439668915134491e-247, -6.3691100762962136e-270, -9.436808465446358e-293,
   8.0970921678014997e-317
};

#if defined(_MSC_VER) && (_MSC_VER <= 1200)
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000,
   100000000000,
   1000000000000,
   10000000000000,
   100000000000000,
   1000000000000000,
   10000000000000000,
   100000000000000000,
   1000000000000000000,
   10000000000000000000U
};
#define stbsp__tento19th ((stbsp__uint64)1000000000000000000)
#else
static stbsp__uint64 const stbsp__powten[20] = {
   1,
   10,
   100,
   1000,
   10000,
   100000,
   1000000,
   10000000,
   100000000,
   1000000000,
   10000000000ULL,
   100000000000ULL,
   1000000000000ULL,
   10000000000000ULL,
   100000000000000ULL,
   1000000000000000ULL,
   10000000000000000ULL,
   100000000000000000ULL,
   1000000000000000000ULL,
   10000000000000000000ULL
};
#define stbsp__tento19th (1000000000000000000ULL)
#endif

#define stbsp__ddmulthi(oh, ol, xh, yh)                            \
   {                                                               \
      double ahi = 0, alo, bhi = 0, blo;                           \
      stbsp__int64 bt;                                             \
      oh = xh * yh;                                                \
      STBSP__COPYFP(bt, xh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(ahi, bt);                                      \
      alo = xh - ahi;                                              \
      STBSP__COPYFP(bt, yh);                                       \
      bt &= ((~(stbsp__uint64)0) << 27);                           \
      STBSP__COPYFP(bhi, bt);                                      \
      blo = yh - bhi;                                              \
      ol = ((ahi * bhi - oh) + ahi * blo + alo * bhi) + alo * blo; \
   }

#define stbsp__ddtoS64(ob, xh, xl)          \
   {                                        \
      double ahi = 0, alo, vh, t;           \
      ob = (stbsp__int64)xh;                \
      vh = (double)ob;                      \
      ahi = (xh - vh);                      \
      t = (ahi - xh);                       \
      alo = (xh - (ahi - t)) - (vh + t);    \
      ob += (stbsp__int64)(ahi + alo + xl); \
   }

#define stbsp__ddrenorm(oh, ol) \
   {                            \
      double s;                 \
      s = oh + ol;              \
      ol = ol - (s - oh);       \
      oh = s;                   \
   }

#define stbsp__ddmultlo(oh, ol, xh, xl, yh, yl) ol = ol + (xh * yl + xl * yh);

#define stbsp__ddmultlos(oh, ol, xh, yl) ol = ol + (xh * yl);

static void stbsp__raise_to_power10(double *ohi, double *olo, double d, stbsp__int32 power) // power can be -323 to +350
{
   double ph, pl;
   if ((power >= 0) && (power <= 22)) {
      stbsp__ddmulthi(ph, pl, d, stbsp__bot[power]);
   } else {
      stbsp__int32 e, et, eb;
      double p2h, p2l;

      e = power;
      if (power < 0)
         e = -e;
      et = (e * 0x2c9) >> 14; /* %23 */
      if (et > 13)
         et = 13;
      eb = e - (et * 23);

      ph = d;
      pl = 0.0;
      if (power < 0) {
         if (eb) {
            --eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__negbot[eb]);
            stbsp__ddmultlos(ph, pl, d, stbsp__negboterr[eb]);
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__negtop[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__negtop[et], stbsp__negtoperr[et]);
            ph = p2h;
            pl = p2l;
         }
      } else {
         if (eb) {
            e = eb;
            if (eb > 22)
               eb = 22;
            e -= eb;
            stbsp__ddmulthi(ph, pl, d, stbsp__bot[eb]);
            if (e) {
               stbsp__ddrenorm(ph, pl);
               stbsp__ddmulthi(p2h, p2l, ph, stbsp__bot[e]);
               stbsp__ddmultlos(p2h, p2l, stbsp__bot[e], pl);
               ph = p2h;
               pl = p2l;
            }
         }
         if (et) {
            stbsp__ddrenorm(ph, pl);
            --et;
            stbsp__ddmulthi(p2h, p2l, ph, stbsp__top[et]);
            stbsp__ddmultlo(p2h, p2l, ph, pl, stbsp__top[et], stbsp__toperr[et]);
            ph = p2h;
            pl = p2l;
         }
      }
   }
   stbsp__ddrenorm(ph, pl);
   *ohi = ph;
   *olo = pl;
}

// given a float value, returns the significant bits in bits, and the position of the
//   decimal point in decimal_pos.  +/-INF and NAN are specified by special values
//   returned in the decimal_pos parameter.
// frac_digits is absolute normally, but if you want from first significant digits (got %g and %e), or in 0x80000000
static stbsp__int32 stbsp__real_to_str(char const **start, stbsp__uint32 *len, char *out, stbsp__int32 *decimal_pos, double value, stbsp__uint32 frac_digits)
{
   double d;
   stbsp__int64 bits = 0;
   stbsp__int32 expo, e, ng, tens;

   d = value;
   STBSP__COPYFP(bits, d);
   expo = (stbsp__int32)((bits >> 52) & 2047);
   ng = (stbsp__int32)((stbsp__uint64) bits >> 63);
   if (ng)
      d = -d;

   if (expo == 2047) // is nan or inf?
   {
      *start = (bits & ((((stbsp__uint64)1) << 52) - 1)) ? "NaN" : "Inf";
      *decimal_pos = STBSP__SPECIAL;
      *len = 3;
      return ng;
   }

   if (expo == 0) // is zero or denormal
   {
      if (((stbsp__uint64) bits << 1) == 0) // do zero
      {
         *decimal_pos = 1;
         *start = out;
         out[0] = '0';
         *len = 1;
         return ng;
      }
      // find the right expo for denormals
      {
         stbsp__int64 v = ((stbsp__uint64)1) << 51;
         while ((bits & v) == 0) {
            --expo;
            v >>= 1;
         }
      }
   }

   // find the decimal exponent as well as the decimal bits of the value
   {
      double ph, pl;

      // log10 estimate - very specifically tweaked to hit or undershoot by no more than 1 of log10 of all expos 1..2046
      tens = expo - 1023;
      tens = (tens < 0) ? ((tens * 617) / 2048) : (((tens * 1233) / 4096) + 1);

      // move the significant bits into position and stick them into an int
      stbsp__raise_to_power10(&ph, &pl, d, 18 - tens);

      // get full as much precision from double-double as possible
      stbsp__ddtoS64(bits, ph, pl);

      // check if we undershot
      if (((stbsp__uint64)bits) >= stbsp__tento19th)
         ++tens;
   }

   // now do the rounding in integer land
   frac_digits = (frac_digits & 0x80000000) ? ((frac_digits & 0x7ffffff) + 1) : (tens + frac_digits);
   if ((frac_digits < 24)) {
      stbsp__uint32 dg = 1;
      if ((stbsp__uint64)bits >= stbsp__powten[9])
         dg = 10;
      while ((stbsp__uint64)bits >= stbsp__powten[dg]) {
         ++dg;
         if (dg == 20)
            goto noround;
      }
      if (frac_digits < dg) {
         stbsp__uint64 r;
         // add 0.5 at the right position and round
         e = dg - frac_digits;
         if ((stbsp__uint32)e >= 24)
            goto noround;
         r = stbsp__powten[e];
         bits = bits + (r / 2);
         if ((stbsp__uint64)bits >= stbsp__powten[dg])
            ++tens;
         bits /= r;
      }
   noround:;
   }

   // kill long trailing runs of zeros
   if (bits) {
      stbsp__uint32 n;
      for (;;) {
         if (bits <= 0xffffffff)
            break;
         if (bits % 1000)
            goto donez;
         bits /= 1000;
      }
      n = (stbsp__uint32)bits;
      while ((n % 1000) == 0)
         n /= 1000;
      bits = n;
   donez:;
   }

   // convert to string
   out += 64;
   e = 0;
   for (;;) {
      stbsp__uint32 n;
      char *o = out - 8;
      // do the conversion in chunks of U32s (avoid most 64-bit divides, worth it, constant denomiators be damned)
      if (bits >= 100000000) {
         n = (stbsp__uint32)(bits % 100000000);
         bits /= 100000000;
      } else {
         n = (stbsp__uint32)bits;
         bits = 0;
      }
      while (n) {
         out -= 2;
         *(stbsp__uint16 *)out = *(stbsp__uint16 *)&stbsp__digitpair.pair[(n % 100) * 2];
         n /= 100;
         e += 2;
      }
      if (bits == 0) {
         if ((e) && (out[0] == '0')) {
            ++out;
            --e;
         }
         break;
      }
      while (out != o) {
         *--out = '0';
         ++e;
      }
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
#endif // DQN_STB_SPRINTF_HEADER_ONLY
