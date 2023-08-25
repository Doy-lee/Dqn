// NOTE: [$CSTR] Dqn_CString8 ======================================================================
DQN_API Dqn_usize Dqn_CString8_FSize(DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Dqn_usize result = STB_SPRINTF_DECORATE(vsnprintf)(nullptr, 0, fmt, args);
    va_end(args);
    return result;
}

DQN_API Dqn_usize Dqn_CString8_FVSize(DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
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

DQN_API Dqn_String8 Dqn_String8_Advance(Dqn_String8 string, Dqn_usize amount)
{
    Dqn_String8 result = Dqn_String8_Slice(string, amount, UINT64_MAX);
    return result;
}

DQN_API Dqn_String8BinarySplitResult Dqn_String8_BinarySplitArray(Dqn_String8 string, Dqn_String8 const *find, Dqn_usize find_size)
{
    Dqn_String8BinarySplitResult result = {};
    if (!Dqn_String8_IsValid(string) || !find || find_size == 0)
        return result;

    result.lhs = string;
    for (size_t index = 0; !result.rhs.data && index <= string.size; index++) {
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

DQN_API Dqn_String8SplitAllocResult Dqn_String8_SplitAlloc(Dqn_Allocator allocator,
                                                           Dqn_String8   string,
                                                           Dqn_String8   delimiter)
{
    Dqn_String8SplitAllocResult result = {};
    Dqn_usize splits_required          = Dqn_String8_Split(string, delimiter, /*splits*/ nullptr, /*count*/ 0);
    result.data                        = Dqn_Allocator_NewArray(allocator, Dqn_String8, splits_required, Dqn_ZeroMem_No);
    if (result.data) {
        result.size = Dqn_String8_Split(string, delimiter, result.data, splits_required);
        DQN_ASSERT(splits_required == result.size);
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
                result.start_to_before_match  = Dqn_String8_Init(string.data, index);
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

    DQN_MSVC_WARNING_PUSH
    DQN_MSVC_WARNING_DISABLE(6293) // Ill-defined for-loop.
    for (Dqn_usize index = result.size - 1; index < result.size; --index) {
        if (result.data[index] == '\\' || result.data[index] == '/') {
            char const *end = result.data + result.size;
            result.data     = result.data + (index + 1);
            result.size     = end - result.data;
            break;
        }
    }
    DQN_MSVC_WARNING_POP
    return result;
}

DQN_API Dqn_String8ToU64Result Dqn_String8_ToU64(Dqn_String8 string, char separator)
{
    // NOTE: Argument check
    Dqn_String8ToU64Result result = {};
    if (!Dqn_String8_IsValid(string))
        return result;

    // NOTE: Sanitize input/output
    Dqn_String8 trim_string = Dqn_String8_TrimWhitespaceAround(string);
    if (trim_string.size == 0) {
        result.success = false;
        return result;
    }

    // NOTE: Handle prefix '+'
    Dqn_usize start_index = 0;
    if (!Dqn_Char_IsDigit(trim_string.data[0])) {
        if (trim_string.data[0] != '+')
            return result;
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
            return result;

        result.value   = Dqn_Safe_MulU64(result.value, 10);
        uint64_t digit = ch - '0';
        result.value   = Dqn_Safe_AddU64(result.value, digit);
    }

    result.success = true;
    return result;
}

DQN_API Dqn_String8ToI64Result Dqn_String8_ToI64(Dqn_String8 string, char separator)
{
    // NOTE: Argument check
    Dqn_String8ToI64Result result = {};
    if (!Dqn_String8_IsValid(string))
        return result;

    // NOTE: Sanitize input/output
    Dqn_String8 trim_string = Dqn_String8_TrimWhitespaceAround(string);
    if (trim_string.size == 0) {
        result.success = false;
        return result;
    }

    bool negative         = false;
    Dqn_usize start_index = 0;
    if (!Dqn_Char_IsDigit(trim_string.data[0])) {
        negative = (trim_string.data[start_index] == '-');
        if (!negative && trim_string.data[0] != '+')
            return result;
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
            return result;

        result.value   = Dqn_Safe_MulU64(result.value, 10);
        uint64_t digit = ch - '0';
        result.value   = Dqn_Safe_AddU64(result.value, digit);
    }

    if (negative)
        result.value *= -1;

    result.success = true;
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

DQN_API Dqn_String8 Dqn_String8_InitF(Dqn_Allocator allocator, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
{
    va_list va;
    va_start(va, fmt);
    Dqn_String8 result = Dqn_String8_InitFV(allocator, fmt, va);
    va_end(va);
    return result;
}

DQN_API Dqn_String8 Dqn_String8_InitFV(Dqn_Allocator allocator, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
{
    Dqn_String8 result = {};
    if (!fmt)
        return result;

    Dqn_usize size = Dqn_CString8_FVSize(fmt, args);
    if (size) {
        result = Dqn_String8_Allocate(allocator, size, Dqn_ZeroMem_No);
        if (Dqn_String8_IsValid(result))
            STB_SPRINTF_DECORATE(vsnprintf)(result.data, Dqn_Safe_SaturateCastISizeToInt(size + 1 /*null-terminator*/), fmt, args);
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Allocate(Dqn_Allocator allocator, Dqn_usize size, Dqn_ZeroMem zero_mem)
{
    Dqn_String8 result = {};
    result.data        = (char *)Dqn_Allocator_Alloc(allocator, size + 1, alignof(char), zero_mem);
    if (result.data)
        result.size = size;
    return result;
}

DQN_API Dqn_String8 Dqn_String8_CopyCString(Dqn_Allocator allocator, char const *string, Dqn_usize size)
{
    Dqn_String8 result = {};
    if (!string)
        return result;

    result = Dqn_String8_Allocate(allocator, size, Dqn_ZeroMem_No);
    if (Dqn_String8_IsValid(result)) {
        DQN_MEMCPY(result.data, string, size);
        result.data[size] = 0;
    }
    return result;
}

DQN_API Dqn_String8 Dqn_String8_Copy(Dqn_Allocator allocator, Dqn_String8 string)
{
    Dqn_String8 result = Dqn_String8_CopyCString(allocator, string.data, string.size);
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

DQN_API bool Dqn_String8Builder_AppendFV(Dqn_String8Builder *builder, DQN_FMT_STRING_ANNOTATE char const *fmt, va_list args)
{
    Dqn_String8 string = Dqn_String8_InitFV(builder->allocator, fmt, args);
    if (string.size == 0)
        return true;

    bool result = Dqn_String8Builder_AppendRef(builder, string);
    if (!result)
        Dqn_Allocator_Dealloc(builder->allocator, string.data, string.size + 1);
    return result;
}

DQN_API bool Dqn_String8Builder_AppendF(Dqn_String8Builder *builder, DQN_FMT_STRING_ANNOTATE char const *fmt, ...)
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

static char constexpr DQN_HEX_LUT[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};
DQN_API char Dqn_Char_ToHex(char ch)
{
    char result = DQN_CAST(char)-1;
    if (ch < 16)
        result = DQN_HEX_LUT[ch];
    return result;
}

DQN_API char Dqn_Char_ToHexUnchecked(char ch)
{
    char result = DQN_HEX_LUT[ch];
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

