#if !defined(SHEREDOM_JSON_H_INCLUDED)
    #error Sheredom's json.h (github.com/sheredom/json.h) must be included before this file
#endif

#if !defined(DQN_JSON_H)
#define DQN_JSON_H

// NOTE: Dqn_JSON
// -----------------------------------------------------------------------------
void *Dqn_JSON_ArenaAllocFunc(void *user_data, size_t count);
char const *Dqn_JSON_TypeEnumCString(json_type_e type, size_t *size);
bool Dqn_JSON_String8Cmp(json_string_s const *lhs, Dqn_String8 rhs);

#define Dqn_JSON_RecordIgnoredObjElement(element, flags) Dqn_JSON_RecordIgnoredObjElement_(element, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
void    Dqn_JSON_RecordIgnoredObjElement_(json_object_element_s const *element, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define Dqn_JSON_RecordIgnoredArrayElement(element, flags) Dqn_JSON_RecordIgnoredArrayElement_(element, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
void    Dqn_JSON_RecordIgnoredArrayElement_(json_array_element_s const *element, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define Dqn_JSON_RecordIgnoredValue(value, flags) Dqn_JSON_RecordIgnoredValue_(value, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
void    Dqn_JSON_RecordIgnoredValue_(json_value_s const *value, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

// NOTE: Dqn_JSON_ValueCheck
// -----------------------------------------------------------------------------
#define        Dqn_JSON_ValueCheck(val, type, flags, require) Dqn_JSON_ValueCheck(val, type, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_ValueCheck_(json_value_s *val, json_type_e type, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ValueCheckObj(val, flags, require) Dqn_JSON_ValueCheckObj_(val, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_object_s *Dqn_JSON_ValueCheckObj_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ValueCheckArray(val, flags, require) Dqn_JSON_ValueCheckArray_(val, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_array_s  *Dqn_JSON_ValueCheckArray_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ValueCheckString(val, flags, require) Dqn_JSON_ValueCheckString_(val, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_string_s *Dqn_JSON_ValueCheckString_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ValueCheckNumber(val, flags, require) Dqn_JSON_ValueCheckNumber_(val, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_number_s *Dqn_JSON_ValueCheckNumber_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define       Dqn_JSON_ValueCheckBool(val, flags, require) Dqn_JSON_ValueCheckBool_(val, flags, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s *Dqn_JSON_ValueCheckBool_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);


// NOTE: Dqn_JSON_ValueRequire
// -----------------------------------------------------------------------------
#define Dqn_JSON_ValueRequire(val, type, flags) Dqn_JSON_ValueCheck(val, type,    flags, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ValueRequireObj(val, flags) Dqn_JSON_ValueCheckObj_(val,       flags, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ValueRequireArray(val, flags) Dqn_JSON_ValueCheckArray_(val,   flags, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ValueRequireString(val, flags) Dqn_JSON_ValueCheckString_(val, flags, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ValueRequireNumber(val, flags) Dqn_JSON_ValueCheckNumber_(val, flags, true /*require*/ DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ValueRequireBool(val, flags) Dqn_JSON_ValueCheckBool_(val,     flags, true /*require*/ DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)

// NOTE: Dqn_JSON_ObjElementCheck
// -----------------------------------------------------------------------------
#define        Dqn_JSON_ObjElementCheckNamed(element, type, key, flags) Dqn_JSON_ObjElementCheckNamed_(element, type, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_ObjElementCheckNamed_(json_object_element_s *element, json_type_e type, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ObjElementCheckNamedObj(element, key, flags) Dqn_JSON_ObjElementCheckNamedObj_(element, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_object_s *Dqn_JSON_ObjElementCheckNamedObj_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ObjElementCheckNamedArray(element, key, flags) Dqn_JSON_ObjElementCheckNamedArray_(element, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_array_s  *Dqn_JSON_ObjElementCheckNamedArray_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ObjElementCheckNamedString(element, key, flags) Dqn_JSON_ObjElementCheckNamedString_(element, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_string_s *Dqn_JSON_ObjElementCheckNamedString_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ObjElementCheckNamedNumber(element, key, flags) Dqn_JSON_ObjElementCheckNamedNumber_(element, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_number_s *Dqn_JSON_ObjElementCheckNamedNumber_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_ObjElementCheckNamedBool(element, key, flags) Dqn_JSON_ObjElementCheckNamedBool_(element, key, flags, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_ObjElementCheckNamedBool_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

// NOTE: Dqn_JSON_ObjElementIs
// -----------------------------------------------------------------------------
#define Dqn_JSON_ObjElementIsNamed(element, type, key) Dqn_JSON_ObjElementCheckNamed_(element, type,  key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementIsNamedObj(element, key)    Dqn_JSON_ObjElementCheckNamedObj_(element,    key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementIsNamedArray(element, key)  Dqn_JSON_ObjElementCheckNamedArray_(element,  key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementIsNamedString(element, key) Dqn_JSON_ObjElementCheckNamedString_(element, key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementIsNamedNumber(element, key) Dqn_JSON_ObjElementCheckNamedNumber_(element, key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementIsNamedBool(element, key)   Dqn_JSON_ObjElementCheckNamedBool_(element,   key, 0 /*flags*/, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)

// NOTE: Dqn_JSON_ObjElementRequire
// -----------------------------------------------------------------------------
#define Dqn_JSON_ObjElementRequireNamed(element, type, key)  Dqn_JSON_ObjElementCheckNamed_(element, type,  key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementRequireNamedObj(element, key)    Dqn_JSON_ObjElementCheckNamedObj_(element,    key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementRequireNamedArray(element, key)  Dqn_JSON_ObjElementCheckNamedArray_(element,  key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementRequireNamedString(element, key) Dqn_JSON_ObjElementCheckNamedString_(element, key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementRequireNamedNumber(element, key) Dqn_JSON_ObjElementCheckNamedNumber_(element, key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_ObjElementRequireNamedBool(element, key)   Dqn_JSON_ObjElementCheckNamedBool_(element,   key, 0 /*flags*/, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)

// NOTE: Dqn_JSON_Iterator
// -----------------------------------------------------------------------------
enum Dqn_JSONIteratorEntryType
{
    Dqn_JSON_IteratorEntryTypeObjElement,
    Dqn_JSON_IteratorEntryTypeObj,
    Dqn_JSON_IteratorEntryTypeArrayElement,
    Dqn_JSON_IteratorEntryTypeArray,
    Dqn_JSON_IteratorEntryTypeString,
    Dqn_JSON_IteratorEntryTypeNumber,
};

struct Dqn_JSONIteratorEntry
{
    Dqn_JSONIteratorEntryType type;
    void *value;
};

struct Dqn_JSONIterator
{
    Dqn_JSONIteratorEntry stack[128];
    int                   stack_count;
    size_t                flags;
};

// NOTE: Dqn_JSON_IteratorPush/Pop
// -----------------------------------------------------------------------------
bool Dqn_JSON_IteratorPushObjElement(Dqn_JSONIterator *state, json_object_element_s *element);
bool Dqn_JSON_IteratorPushObj(Dqn_JSONIterator *state, json_object_s *obj);
bool Dqn_JSON_IteratorPushArrayElement(Dqn_JSONIterator *state, json_array_element_s *element);
bool Dqn_JSON_IteratorPushArray(Dqn_JSONIterator *state, json_array_s *array);
void Dqn_JSON_IteratorPop(Dqn_JSONIterator *state);

// NOTE: Dqn_JSON_Iterator tree navigation
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorPushKey(Dqn_JSONIterator *state);
bool Dqn_JSON_IteratorNextField(Dqn_JSONIterator *state);

#define Dqn_JSON_IteratorErrorUnrecognisedKey(state) Dqn_JSON_IteratorErrorUnrecognisedKey_(state, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
void Dqn_JSON_IteratorErrorUnrecognisedKey_(Dqn_JSONIterator *state, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define Dqn_JSON_IteratorPushKeyIterateThenPop(state) \
    for(void *DQN_UNIQUE_NAME(ptr) = Dqn_JSON_IteratorPushKey(state); DQN_UNIQUE_NAME(ptr); Dqn_JSON_IteratorPop(state), DQN_UNIQUE_NAME(ptr) = nullptr) \
        while (Dqn_JSON_IteratorNextField(state))

// NOTE: Dqn_JSON_IteratorCurr
// -----------------------------------------------------------------------------
Dqn_JSONIteratorEntry *Dqn_JSON_IteratorCurr(Dqn_JSONIterator *state);
json_value_s          *Dqn_JSON_IteratorCurrValue(Dqn_JSONIterator *state);
json_object_element_s *Dqn_JSON_IteratorCurrObjElement(Dqn_JSONIterator *state);

// NOTE: Dqn_JSON_IteratorKeyCheck
// -----------------------------------------------------------------------------

#define        Dqn_JSON_IteratorKeyCheck(state, type, require) Dqn_JSON_IteratorKeyCheck(state, type, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_IteratorKeyCheck_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyCheckObj(state, require) Dqn_JSON_IteratorKeyCheckObj_(state, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_object_s *Dqn_JSON_IteratorKeyCheckObj_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyCheckArray(state, require) Dqn_JSON_IteratorKeyCheckArray_(state, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_array_s  *Dqn_JSON_IteratorKeyCheckArray_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyCheckString(state, require) Dqn_JSON_IteratorKeyCheckString_(state, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_string_s *Dqn_JSON_IteratorKeyCheckString_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyCheckNumber(state, require) Dqn_JSON_IteratorKeyCheckNumber_(state, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_number_s *Dqn_JSON_IteratorKeyCheckNumber_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyCheckBool(state, require) Dqn_JSON_IteratorKeyCheckBool_(state, require, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_IteratorKeyCheckBool_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

// NOTE: Dqn_JSON_Iterator_KeyIs
// -----------------------------------------------------------------------------
#define Dqn_JSON_IteratorKeyIs(state, type) Dqn_JSON_IteratorKeyCheck(state, type,    false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyIsObj(state) Dqn_JSON_IteratorKeyCheckObj_(state,       false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyIsArray(state) Dqn_JSON_IteratorKeyCheckArray_(state,   false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyIsString(state) Dqn_JSON_IteratorKeyCheckString_(state, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyIsNumber(state) Dqn_JSON_IteratorKeyCheckNumber_(state, false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyIsBool(state) Dqn_JSON_IteratorKeyCheckBool_(state,     false /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)

// NOTE: Dqn_JSON_Iterator_KeyRequire
// -----------------------------------------------------------------------------
#define Dqn_JSON_IteratorKeyRequire(state, type, require) Dqn_JSON_IteratorKeyCheck(state, type,  true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyRequireObj(state, require) Dqn_JSON_IteratorKeyCheckObj_(state,       true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyRequireArray(state, require) Dqn_JSON_IteratorKeyCheckArray_(state,   true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyRequireString(state, require) Dqn_JSON_IteratorKeyCheckString_(state, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyRequireNumber(state, require) Dqn_JSON_IteratorKeyCheckNumber_(state, true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
#define Dqn_JSON_IteratorKeyRequireBool(state, require) Dqn_JSON_IteratorKeyCheckBool_(state,     true /*require*/, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)

// NOTE: Dqn_JSON_IteratorKeyIsNamed
// -----------------------------------------------------------------------------
#define        Dqn_JSON_IteratorKeyIsNamedObj(state, key) Dqn_JSON_IteratorKeyIsNamedObj_(state, key, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_object_s *Dqn_JSON_IteratorKeyIsNamedObj_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyIsNamedArray(state, key) Dqn_JSON_IteratorKeyIsNamedArray_(state, key, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_array_s  *Dqn_JSON_IteratorKeyIsNamedArray_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyIsNamedString(state, key) Dqn_JSON_IteratorKeyIsNamedString_(state, key, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_string_s *Dqn_JSON_IteratorKeyIsNamedString_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyIsNamedNumber(state, key) Dqn_JSON_IteratorKeyIsNamedNumber_(state, key, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_number_s *Dqn_JSON_IteratorKeyIsNamedNumber_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define        Dqn_JSON_IteratorKeyIsNamedBool(state, key) Dqn_JSON_IteratorKeyIsNamedBool_(state, key, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
json_value_s  *Dqn_JSON_IteratorKeyIsNamedBool_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

// NOTE: Dqn_JSON_IteratorKeyTo
// -----------------------------------------------------------------------------
Dqn_String8 Dqn_JSON_IteratorKeyToString(Dqn_JSONIterator *state);
int64_t     Dqn_JSON_IteratorKeyToI64(Dqn_JSONIterator *state);
bool        Dqn_JSON_IteratorKeyToBool(Dqn_JSONIterator *state);

#endif // DQN_JSON_H

#if defined(DQN_JSON_IMPLEMENTATION)
// NOTE: Dqn_JSON
// -----------------------------------------------------------------------------
void *Dqn_JSON_ArenaAllocFunc(void *user_data, size_t count)
{
    void *result = NULL;
    if (!user_data)
        return result;

    Dqn_Arena *arena = DQN_CAST(Dqn_Arena*)user_data;
    result = Dqn_Arena_Allocate(arena, count, alignof(json_value_s), Dqn_ZeroMem_No);
    return result;
}

char const *Dqn_JSON_TypeEnumCString(json_type_e type, size_t *size)
{
    switch (type) {
        case json_type_string: { if (size) { *size = sizeof("string") - 1; } return "string"; }
        case json_type_number: { if (size) { *size = sizeof("number") - 1; } return "number"; }
        case json_type_object: { if (size) { *size = sizeof("object") - 1; } return "object"; }
        case json_type_array:  { if (size) { *size = sizeof("array") - 1; } return "array"; }
        case json_type_true:   { if (size) { *size = sizeof("true (boolean)") - 1; } return "true (boolean)"; }
        case json_type_false:  { if (size) { *size = sizeof("false (boolean)") - 1; } return "false (boolean)"; }

        default: /*FALLTHRU*/
        case json_type_null: { if (size) { *size = sizeof("(null)") - 1; } return "(null)"; }
    }
}

bool Dqn_JSON_String8Cmp(json_string_s const *lhs, Dqn_String8 key)
{
    bool result = false;
    if (lhs && Dqn_String8_IsValid(key)) {
        Dqn_String8 lhs_string = Dqn_String8_Init(lhs->string, lhs->string_size);
        result                 = Dqn_String8_Eq(lhs_string, key);
    }
    return result;
}

void Dqn_JSON_RecordIgnoredObjElement_(json_object_element_s const *element, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_string_s const *key = element->name;
    size_t value_type_size = 0;
    char const *value_type = Dqn_JSON_TypeEnumCString(DQN_CAST(json_type_e)element->value->type, &value_type_size);
    if (flags == json_parse_flags_allow_location_information) {
        auto const *info = DQN_CAST(json_string_ex_s const *)key;
        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored key-value pair in object [line=%zu, col=%zu, key=%.*s, type=%.*s]",
                info->line_no,
                info->row_no,
                key->string_size,
                key->string,
                value_type_size,
                value_type);
    } else {
        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored key-value pair in object [key=%.*s, type=%.*s]",
                key->string_size,
                key->string,
                value_type_size,
                value_type);
    }
}

void Dqn_JSON_RecordIgnoredArrayElement_(json_array_element_s const *element, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s const *value = element->value;
    size_t              size  = 0;
    char const         *type  = Dqn_JSON_TypeEnumCString((json_type_e)value->type, &size);
    if (flags == json_parse_flags_allow_location_information) {
        size_t line_no = 0;
        size_t row_no = 0;
        if (value->type == json_type_string) {
            auto *info = DQN_CAST(json_string_ex_s const *) value;
            line_no    = info->line_no;
            row_no     = info->row_no;
        } else {
            auto *info = DQN_CAST(json_value_ex_s const *) value;
            line_no    = info->line_no;
            row_no     = info->row_no;
        }

        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored value in array [line=%zu, col=%zu, value_type=%.*s]",
                line_no,
                row_no,
                size,
                type);
    } else {
        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored value in array [value_type=%.*s]",
                size,
                type);
    }
}

void Dqn_JSON_RecordIgnoredValue_(json_value_s const *value, size_t flags, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    size_t              size  = 0;
    char const         *type  = Dqn_JSON_TypeEnumCString((json_type_e)value->type, &size);
    if (flags == json_parse_flags_allow_location_information) {
        size_t line_no = 0;
        size_t row_no = 0;
        if (value->type == json_type_string) {
            auto *info = DQN_CAST(json_string_ex_s const *) value;
            line_no    = info->line_no;
            row_no     = info->row_no;
        } else {
            auto *info = DQN_CAST(json_value_ex_s const *) value;
            line_no    = info->line_no;
            row_no     = info->row_no;
        }

        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored value in json [line=%zu, col=%zu, value_type=%.*s]",
                line_no,
                row_no,
                size,
                type);
    } else {
        Dqn_Log(Dqn_LogType::Warning,
                nullptr,
                file,
                func,
                line,
                "Ignored value in json [value_type=%.*s]",
                size,
                type);
    }
}

// NOTE: Dqn_JSON_ValueCheck
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_ValueCheck_(json_value_s *val, json_type_e type, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *result = (val && val->type == type) ? val : nullptr;
    if (!result && require) {
        size_t      expect_size = 0;
        char const *expect_type = Dqn_JSON_TypeEnumCString(type, &expect_size);
        size_t      val_size    = 0;
        char const *val_type    = Dqn_JSON_TypeEnumCString((json_type_e)val->type, &val_size);
        if (flags == json_parse_flags_allow_location_information) {
            auto const *info = DQN_CAST(json_value_ex_s const *) val;
            Dqn_Log(Dqn_LogType::Error,
                    nullptr /*user_data*/,
                    file,
                    func,
                    line,
                    "Expected json '%.*s' [line=%zu, col=%zu, value_type=%.*s]",
                    expect_size,
                    expect_type,
                    info->line_no,
                    info->row_no,
                    val_size,
                    val_type);
        } else {
            Dqn_Log(Dqn_LogType::Error,
                    nullptr /*userdata*/,
                    file,
                    func,
                    line,
                    "Expected json '%.*s' [value_type=%.*s]",
                    expect_size,
                    expect_type,
                    val_size,
                    val_type);
        }
    }

    return result;
}

json_object_s *Dqn_JSON_ValueCheckObj_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_s *result = nullptr;
    if (Dqn_JSON_ValueCheck_(val, json_type_object, flags, require, file, func, line))
        result = json_value_as_object(val);
    return result;
}

json_array_s *Dqn_JSON_ValueCheckArray_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_array_s *result = nullptr;
    if (Dqn_JSON_ValueCheck_(val, json_type_array, flags, require, file, func, line))
        result = json_value_as_array(val);
    return result;
}

json_string_s *Dqn_JSON_ValueCheckString_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_string_s *result = nullptr;
    if (Dqn_JSON_ValueCheck_(val, json_type_string, flags, require, file, func, line))
        result = json_value_as_string(val);
    return result;
}

json_number_s *Dqn_JSON_ValueCheckNumber_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_number_s *result = nullptr;
    if (Dqn_JSON_ValueCheck_(val, json_type_number, flags, require, file, func, line))
        result = json_value_as_number(val);
    return result;
}

json_value_s *Dqn_JSON_ValueCheckBool_(json_value_s *val, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *result = nullptr;
    if (val->type == json_type_true || val->type == json_type_false) {
        result = val;
    } else {
        if (require) {
            size_t      val_size = 0;
            char const *val_type = Dqn_JSON_TypeEnumCString((json_type_e)val->type, &val_size);
            if (flags == json_parse_flags_allow_location_information) {
                auto const *info = DQN_CAST(json_value_ex_s const *) val;
                Dqn_Log(Dqn_LogType::Error,
                        nullptr /*userdata*/,
                        file,
                        func,
                        line,
                        "Expected json bool [line=%zu, col=%zu, value_type=%.*s]",
                        info->line_no,
                        info->row_no,
                        val_size,
                        val_type);
            } else {
                Dqn_Log(Dqn_LogType::Error,
                        nullptr /*userdata*/,
                        file,
                        func,
                        line,
                        "Expected json bool [value_type=%.*s]",
                        val_size,
                        val_type);
            }
        }
    }

    return result;
}

// NOTE: Dqn_JSON_ObjElementCheck
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_ObjElementCheckNamed_(json_object_element_s *element, json_type_e type, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *result = nullptr;
    if (element && Dqn_JSON_String8Cmp(element->name, key))
        result = Dqn_JSON_ValueCheck_(element->value, type, flags, require, file, func, line);
    return result;
}

json_object_s *Dqn_JSON_ObjElementCheckNamedObj_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_s *result = DQN_CAST(json_object_s *)Dqn_JSON_ObjElementCheckNamed_(element, json_type_object, key, flags, require, file, func, line);
    return result;
}

json_array_s *Dqn_JSON_ObjElementCheckNamedArray_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_array_s *result = DQN_CAST(json_array_s *)Dqn_JSON_ObjElementCheckNamed_(element, json_type_array, key, flags, require, file, func, line);
    return result;
}

json_string_s *Dqn_JSON_ObjElementCheckNamedString_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_string_s *result = DQN_CAST(json_string_s *)Dqn_JSON_ObjElementCheckNamed_(element, json_type_string, key, flags, require, file, func, line);
    return result;
}

json_number_s *Dqn_JSON_ObjElementCheckNamedNumber_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_number_s *result = DQN_CAST(json_number_s *)Dqn_JSON_ObjElementCheckNamed_(element, json_type_number, key, flags, require, file, func, line);
    return result;
}

json_value_s *Dqn_JSON_ObjElementCheckNamedBool_(json_object_element_s *element, Dqn_String8 key, size_t flags, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *result = nullptr;
    if (element && Dqn_JSON_String8Cmp(element->name, key))
        result = Dqn_JSON_ValueCheckBool_(element->value, flags, require, file, func, line);
    return result;
}

// NOTE: Dqn_JSON_Iterator_push/pop
// -----------------------------------------------------------------------------
bool Dqn_JSON_IteratorPushObjElement(Dqn_JSONIterator *state, json_object_element_s *element)
{
    if (!state || !element)
        return false;
    DQN_ASSERT(state->stack_count < DQN_ARRAY_ICOUNT(state->stack));
    state->stack[state->stack_count++] = {Dqn_JSON_IteratorEntryTypeObjElement, element};
    return true;
}

bool Dqn_JSON_IteratorPushObj(Dqn_JSONIterator *state, json_object_s *obj)
{
    if (!state || !obj)
        return false;
    DQN_ASSERT(state->stack_count < DQN_ARRAY_ICOUNT(state->stack));
    state->stack[state->stack_count++] = {Dqn_JSON_IteratorEntryTypeObj, obj};
    return true;
}

bool Dqn_JSON_IteratorPushArrayElement(Dqn_JSONIterator *state, json_array_element_s *element)
{
    if (!state || !element)
        return false;
    DQN_ASSERT(state->stack_count < DQN_ARRAY_ICOUNT(state->stack));
    state->stack[state->stack_count++] = {Dqn_JSON_IteratorEntryTypeArrayElement, element};
    return true;
}

bool Dqn_JSON_IteratorPushArray(Dqn_JSONIterator *state, json_array_s *array)
{
    if (!state || !array)
        return false;
    DQN_ASSERT(state->stack_count < DQN_ARRAY_ICOUNT(state->stack));
    state->stack[state->stack_count++] = {Dqn_JSON_IteratorEntryTypeArray, array};
    return true;
}

void Dqn_JSON_IteratorPop(Dqn_JSONIterator *state)
{
    if (!state)
        return;
    DQN_ASSERT(state->stack_count > 0);
    if (state->stack_count > 0)
        state->stack_count--;
}

// NOTE: Dqn_JSON_Iterator json tree navigation
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorPushKey(Dqn_JSONIterator *state)
{
    json_value_s          *result = nullptr;
    Dqn_JSONIteratorEntry *curr   = Dqn_JSON_IteratorCurr(state);
    if (!curr)
        return result;

    if (curr->type == Dqn_JSON_IteratorEntryTypeObjElement) {
        json_object_element_s *element = DQN_CAST(json_object_element_s *) curr->value;
        result                         = element->value;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArrayElement) {
        json_array_element_s *element = DQN_CAST(json_array_element_s *) curr->value;
        result                        = element->value;
    } else {
        result = DQN_CAST(json_value_s *) curr->value;
    }

    if (result->type == json_type_array) {
        json_array_s *array = json_value_as_array(result);
        DQN_ASSERT(array);
        Dqn_JSON_IteratorPushArray(state, array);
    } else if (result->type == json_type_object) {
        json_object_s *obj = json_value_as_object(result);
        DQN_ASSERT(obj);
        Dqn_JSON_IteratorPushObj(state, obj);
    }

    return result;
}

bool Dqn_JSON_IteratorNextField(Dqn_JSONIterator *state)
{
    Dqn_JSONIteratorEntry *curr          = Dqn_JSON_IteratorCurr(state);
    json_object_element_s *obj_element   = nullptr;
    json_array_element_s  *array_element = nullptr;
    if (curr->type == Dqn_JSON_IteratorEntryTypeObj) {
        auto *obj   = DQN_CAST(json_object_s *) curr->value;
        obj_element = obj->start;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeObjElement) {
        auto *element = DQN_CAST(json_object_element_s *) curr->value;
        obj_element   = element->next;
        Dqn_JSON_IteratorPop(state);
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArray) {
        auto *array   = DQN_CAST(json_array_s *) curr->value;
        array_element = array->start;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArrayElement) {
        auto *element = DQN_CAST(json_array_element_s *) curr->value;
        array_element = element->next;
        Dqn_JSON_IteratorPop(state);
    } else {
        Dqn_JSON_IteratorPop(state);
    }

    if (obj_element)
        Dqn_JSON_IteratorPushObjElement(state, obj_element);
    else if (array_element)
        Dqn_JSON_IteratorPushArrayElement(state, array_element);

    bool result = obj_element || array_element;
    return result;
}

void Dqn_JSON_IteratorErrorUnrecognisedKey_(Dqn_JSONIterator *state, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    Dqn_JSONIteratorEntry *curr = Dqn_JSON_IteratorCurr(state);
    if (!curr)
        return;

    if (curr->type == Dqn_JSON_IteratorEntryTypeObjElement) {
        json_object_element_s *element = DQN_CAST(json_object_element_s *) curr->value;
        Dqn_JSON_RecordIgnoredObjElement_(element, state->flags, file, func, line);
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArrayElement) {
        json_array_element_s *element = DQN_CAST(json_array_element_s *) curr->value;
        Dqn_JSON_RecordIgnoredArrayElement_(element, state->flags, file, func, line);
    } else {
        json_value_s *value = DQN_CAST(json_value_s *) curr->value;
        Dqn_JSON_RecordIgnoredValue_(value, state->flags, file, func, line);
    }
}

// NOTE: Dqn_JSON_IteratorCurr
// -----------------------------------------------------------------------------
Dqn_JSONIteratorEntry *Dqn_JSON_IteratorCurr(Dqn_JSONIterator *state)
{
    Dqn_JSONIteratorEntry *result = nullptr;
    if (!state || state->stack_count <= 0)
        return result;

    result = &state->stack[state->stack_count - 1];
    return result;
}

json_value_s *Dqn_JSON_IteratorCurrValue(Dqn_JSONIterator *state)
{
    json_value_s *result = nullptr;
    Dqn_JSONIteratorEntry *curr = Dqn_JSON_IteratorCurr(state);
    if (!curr)
        return result;

    if (curr->type == Dqn_JSON_IteratorEntryTypeObjElement) {
        auto *element = DQN_CAST(json_object_element_s *)curr->value;
        result = element->value;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArrayElement) {
        auto *element = DQN_CAST(json_array_element_s *)curr->value;
        result = element->value;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeString ||
               curr->type == Dqn_JSON_IteratorEntryTypeNumber ||
               curr->type == Dqn_JSON_IteratorEntryTypeObj    ||
               curr->type == Dqn_JSON_IteratorEntryTypeArray)
    {
        result = DQN_CAST(json_value_s *)curr->value;
    }

    return result;
}

json_object_element_s *Dqn_JSON_IteratorCurrObjElement(Dqn_JSONIterator *state)
{
    Dqn_JSONIteratorEntry *curr   = Dqn_JSON_IteratorCurr(state);
    auto           *result = (curr && curr->type == Dqn_JSON_IteratorEntryTypeObjElement)
                                 ? DQN_CAST(json_object_element_s *) curr->value
                                 : nullptr;
    return result;
}

// NOTE: Dqn_JSON_IteratorKeyCheck
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorKeyCheck_(Dqn_JSONIterator *state, json_type_e type, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s  *curr  = Dqn_JSON_IteratorCurrValue(state);
    json_value_s *result = Dqn_JSON_ValueCheck_(curr, type, state->flags, require, file, func, line);
    return result;
}

json_object_s *Dqn_JSON_IteratorKeyCheckObj_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s  *curr   = Dqn_JSON_IteratorCurrValue(state);
    json_object_s *result = Dqn_JSON_ValueCheckObj_(curr, state->flags, require, file, func, line);
    return result;
}

json_array_s *Dqn_JSON_IteratorKeyCheckArray_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *curr   = Dqn_JSON_IteratorCurrValue(state);
    json_array_s *result = Dqn_JSON_ValueCheckArray_(curr, state->flags, require, file, func, line);
    return result;
}

json_string_s *Dqn_JSON_IteratorKeyCheckString_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s  *curr   = Dqn_JSON_IteratorCurrValue(state);
    json_string_s *result = Dqn_JSON_ValueCheckString_(curr, state->flags, require, file, func, line);
    return result;
}

json_number_s *Dqn_JSON_IteratorKeyCheckNumber_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s  *curr   = Dqn_JSON_IteratorCurrValue(state);
    json_number_s *result = Dqn_JSON_ValueCheckNumber_(curr, state->flags, require, file, func, line);
    return result;
}

json_value_s *Dqn_JSON_IteratorKeyCheckBool_(Dqn_JSONIterator *state, bool require, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_value_s *curr   = Dqn_JSON_IteratorCurrValue(state);
    json_value_s *result = Dqn_JSON_ValueCheckBool_(curr, state->flags, require, file, func, line);
    return result;
}

// NOTE: Dqn_JSON_IteratorKeyIsNamed
// -----------------------------------------------------------------------------
json_object_s *Dqn_JSON_IteratorKeyIsNamedObj_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(state);
    json_object_s         *result = state ? Dqn_JSON_ObjElementCheckNamedObj_(curr, key, state->flags, false /*require*/, file, func, line) : nullptr;
    return result;
}

json_array_s *Dqn_JSON_IteratorKeyIsNamedArray_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(state);
    json_array_s          *result = state ? Dqn_JSON_ObjElementCheckNamedArray_(curr, key, state->flags, false /*require*/, file, func, line) : nullptr;
    return result;
}

json_string_s *Dqn_JSON_IteratorKeyIsNamedString_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(state);
    json_string_s         *result = state ? Dqn_JSON_ObjElementCheckNamedString_(curr, key, state->flags, false /*require*/, file, func, line) : nullptr;
    return result;
}

json_number_s *Dqn_JSON_IteratorKeyIsNamedNumber_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(state);
    json_number_s         *result = state ? Dqn_JSON_ObjElementCheckNamedNumber_(curr, key, state->flags, false /*require*/, file, func, line) : nullptr;
    return result;
}

json_value_s *Dqn_JSON_IteratorKeyIsNamedBool_(Dqn_JSONIterator *state, Dqn_String8 key, Dqn_String8 file, Dqn_String8 func, Dqn_uint line)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(state);
    json_value_s          *result = state ? Dqn_JSON_ObjElementCheckNamedBool_(curr, key, state->flags, false /*require*/, file, func, line) : nullptr;
    return result;
}


// NOTE: Dqn_JSON_IteratorKeyTo
// -----------------------------------------------------------------------------
Dqn_String8 Dqn_JSON_IteratorKeyToString(Dqn_JSONIterator *state)
{
    Dqn_String8 result = {};
    if (json_string_s *curr = Dqn_JSON_IteratorKeyIsString(state))
        result = Dqn_String8_Init(curr->string, curr->string_size);
    return result;
}

int64_t Dqn_JSON_IteratorKeyToI64(Dqn_JSONIterator *state)
{
    int64_t result = {};
    if (json_number_s *curr = Dqn_JSON_IteratorKeyIsNumber(state))
        result = Dqn_String8_ToI64(Dqn_String8_Init(curr->number, curr->number_size), 0 /*separator*/);
    return result;
}

bool Dqn_JSON_IteratorKeyToBool(Dqn_JSONIterator *state)
{
    bool result = {};
    if (json_value_s *curr = Dqn_JSON_IteratorKeyIsBool(state))
        result = curr->type == json_type_true;
    return result;
}
#endif // defined(DQN_JSON_IMPLEMENTATION)
