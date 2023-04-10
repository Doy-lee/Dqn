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
bool Dqn_JSON_IteratorPushObjElement  (Dqn_JSONIterator *it, json_object_element_s *element);
bool Dqn_JSON_IteratorPushObj         (Dqn_JSONIterator *it, json_object_s *obj);
bool Dqn_JSON_IteratorPushArrayElement(Dqn_JSONIterator *it, json_array_element_s *element);
bool Dqn_JSON_IteratorPushArray       (Dqn_JSONIterator *it, json_array_s *array);
bool Dqn_JSON_IteratorPushValue       (Dqn_JSONIterator *it, json_value_s *value);
void Dqn_JSON_IteratorPop             (Dqn_JSONIterator *it);

// NOTE: Dqn_JSON_Iterator tree navigation
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorPushCurrValue(Dqn_JSONIterator *it);
bool Dqn_JSON_IteratorNext(Dqn_JSONIterator *it);

#define Dqn_JSON_IteratorErrorUnrecognisedKey(it) Dqn_JSON_IteratorErrorUnrecognisedKey_(it, DQN_STRING8(__FILE__), DQN_STRING8(__func__), __LINE__)
void Dqn_JSON_IteratorErrorUnrecognisedKey_(Dqn_JSONIterator *it, Dqn_String8 file, Dqn_String8 func, Dqn_uint line);

#define Dqn_JSON_IteratorPushCurrValueIterateThenPop(it) \
    for(void *DQN_UNIQUE_NAME(ptr) = Dqn_JSON_IteratorPushCurrValue(it); DQN_UNIQUE_NAME(ptr); Dqn_JSON_IteratorPop(it), DQN_UNIQUE_NAME(ptr) = nullptr) \
        while (Dqn_JSON_IteratorNext(it))

// NOTE: Dqn_JSON_IteratorCurr
// -----------------------------------------------------------------------------
Dqn_JSONIteratorEntry *Dqn_JSON_IteratorCurr(Dqn_JSONIterator *it);
json_value_s          *Dqn_JSON_IteratorCurrValue(Dqn_JSONIterator *it);
json_object_element_s *Dqn_JSON_IteratorCurrObjElement(Dqn_JSONIterator *it);

// NOTE: Dqn_JSON_IteratorValueIs
// -----------------------------------------------------------------------------
json_value_s  *Dqn_JSON_IteratorValueIs(Dqn_JSONIterator *it, json_type_e type);
json_object_s *Dqn_JSON_IteratorValueIsObj(Dqn_JSONIterator *it);
json_array_s  *Dqn_JSON_IteratorValueIsArray(Dqn_JSONIterator *it);
json_string_s *Dqn_JSON_IteratorValueIsString(Dqn_JSONIterator *it);
json_number_s *Dqn_JSON_IteratorValueIsNumber(Dqn_JSONIterator *it);
json_value_s  *Dqn_JSON_IteratorValueIsBool(Dqn_JSONIterator *it);

size_t Dqn_JSON_IteratorValueArraySize(Dqn_JSONIterator *it);

// NOTE: Dqn_JSON_IteratorKeyValueIs
// -----------------------------------------------------------------------------
Dqn_String8    Dqn_JSON_IteratorKey(Dqn_JSONIterator *it);
bool           Dqn_JSON_IteratorKeyIs(Dqn_JSONIterator *it, Dqn_String8 key);
json_object_s *Dqn_JSON_IteratorKeyValueIsObj(Dqn_JSONIterator *it, Dqn_String8 key);
json_array_s  *Dqn_JSON_IteratorKeyValueIsArray(Dqn_JSONIterator *it, Dqn_String8 key);
json_string_s *Dqn_JSON_IteratorKeyValueIsString(Dqn_JSONIterator *it, Dqn_String8 key);
json_number_s *Dqn_JSON_IteratorKeyValueIsNumber(Dqn_JSONIterator *it, Dqn_String8 key);
json_value_s  *Dqn_JSON_IteratorKeyValueIsBool(Dqn_JSONIterator *it, Dqn_String8 key);

// NOTE: Dqn_JSON_IteratorValueTo
// -----------------------------------------------------------------------------
Dqn_String8 Dqn_JSON_IteratorValueToString(Dqn_JSONIterator *it);
int64_t     Dqn_JSON_IteratorValueToI64(Dqn_JSONIterator *it);
uint64_t    Dqn_JSON_IteratorValueToU64(Dqn_JSONIterator *it);
bool        Dqn_JSON_IteratorValueToBool(Dqn_JSONIterator *it);

#define Dqn_JSON_IteratorErrorUnknownKeyValue(it) \
    Dqn_JSON_IteratorErrorUnknownKeyValue_(it, DQN_CALL_SITE)

void Dqn_JSON_IteratorErrorUnknownKeyValue_(Dqn_JSONIterator *it, Dqn_String8 file, Dqn_String8 func, int line);

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

// NOTE: Dqn_JSON_Iterator_push/pop
// -----------------------------------------------------------------------------
bool Dqn_JSON_IteratorPushObjElement(Dqn_JSONIterator *it, json_object_element_s *element)
{
    if (!it || !element)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_IteratorEntryTypeObjElement, element};
    return true;
}

bool Dqn_JSON_IteratorPushObj(Dqn_JSONIterator *it, json_object_s *obj)
{
    if (!it || !obj)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_IteratorEntryTypeObj, obj};
    return true;
}

bool Dqn_JSON_IteratorPushArrayElement(Dqn_JSONIterator *it, json_array_element_s *element)
{
    if (!it || !element)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_IteratorEntryTypeArrayElement, element};
    return true;
}

bool Dqn_JSON_IteratorPushArray(Dqn_JSONIterator *it, json_array_s *array)
{
    if (!it || !array)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_IteratorEntryTypeArray, array};
    return true;
}

bool Dqn_JSON_IteratorPushValue(Dqn_JSONIterator *it, json_value_s *value)
{
    bool result = false;
    if (!it || !value)
        return result;

    if (value->type == json_type_object) {
        result = Dqn_JSON_IteratorPushObj(it, json_value_as_object(value));
    } else if (value->type == json_type_array) {
        result = Dqn_JSON_IteratorPushArray(it, json_value_as_array(value));
    }

    return result;
}

void Dqn_JSON_IteratorPop(Dqn_JSONIterator *it)
{
    if (!it)
        return;
    DQN_ASSERT(it->stack_count > 0);
    if (it->stack_count > 0)
        it->stack_count--;
}

// NOTE: Dqn_JSON_Iterator json tree navigation
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorPushCurrValue(Dqn_JSONIterator *it)
{
    json_value_s          *result = nullptr;
    Dqn_JSONIteratorEntry *curr   = Dqn_JSON_IteratorCurr(it);
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
        Dqn_JSON_IteratorPushArray(it, array);
    } else if (result->type == json_type_object) {
        json_object_s *obj = json_value_as_object(result);
        DQN_ASSERT(obj);
        Dqn_JSON_IteratorPushObj(it, obj);
    }

    return result;
}

bool Dqn_JSON_IteratorNext(Dqn_JSONIterator *it)
{
    Dqn_JSONIteratorEntry *curr = Dqn_JSON_IteratorCurr(it);
    if (!curr)
        return false;

    json_object_element_s *obj_element   = nullptr;
    json_array_element_s  *array_element = nullptr;
    if (curr->type == Dqn_JSON_IteratorEntryTypeObj) {
        auto *obj   = DQN_CAST(json_object_s *) curr->value;
        obj_element = obj->start;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeObjElement) {
        auto *element = DQN_CAST(json_object_element_s *) curr->value;
        obj_element   = element->next;
        Dqn_JSON_IteratorPop(it);
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArray) {
        auto *array   = DQN_CAST(json_array_s *) curr->value;
        array_element = array->start;
    } else if (curr->type == Dqn_JSON_IteratorEntryTypeArrayElement) {
        auto *element = DQN_CAST(json_array_element_s *) curr->value;
        array_element = element->next;
        Dqn_JSON_IteratorPop(it);
    } else {
        Dqn_JSON_IteratorPop(it);
    }

    if (obj_element)
        Dqn_JSON_IteratorPushObjElement(it, obj_element);
    else if (array_element)
        Dqn_JSON_IteratorPushArrayElement(it, array_element);

    bool result = obj_element || array_element;
    return result;
}

// NOTE: Dqn_JSON_IteratorCurr
// -----------------------------------------------------------------------------
Dqn_JSONIteratorEntry *Dqn_JSON_IteratorCurr(Dqn_JSONIterator *it)
{
    Dqn_JSONIteratorEntry *result = nullptr;
    if (!it || it->stack_count <= 0)
        return result;

    result = &it->stack[it->stack_count - 1];
    return result;
}

json_value_s *Dqn_JSON_IteratorCurrValue(Dqn_JSONIterator *it)
{
    json_value_s *result = nullptr;
    Dqn_JSONIteratorEntry *curr = Dqn_JSON_IteratorCurr(it);
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

json_object_element_s *Dqn_JSON_IteratorCurrObjElement(Dqn_JSONIterator *it)
{
    Dqn_JSONIteratorEntry *curr   = Dqn_JSON_IteratorCurr(it);
    auto                  *result = (curr && curr->type == Dqn_JSON_IteratorEntryTypeObjElement)
                                        ? DQN_CAST(json_object_element_s *) curr->value
                                        : nullptr;
    return result;
}

// NOTE: Dqn_JSON_IteratorValueIs
// -----------------------------------------------------------------------------
json_value_s *Dqn_JSON_IteratorValueIs(Dqn_JSONIterator *it, json_type_e type)
{
    json_value_s  *curr  = Dqn_JSON_IteratorCurrValue(it);
    json_value_s *result = (curr && type == curr->type) ? curr : nullptr;
    return result;
}

json_object_s *Dqn_JSON_IteratorValueIsObj(Dqn_JSONIterator *it)
{
    json_value_s *curr    = Dqn_JSON_IteratorCurrValue(it);
    json_object_s *result = curr ? json_value_as_object(curr) : nullptr;
    return result;
}

json_array_s *Dqn_JSON_IteratorValueIsArray(Dqn_JSONIterator *it)
{
    json_value_s *curr   = Dqn_JSON_IteratorCurrValue(it);
    json_array_s *result = curr ? json_value_as_array(curr) : nullptr;
    return result;
}

json_string_s *Dqn_JSON_IteratorValueIsString(Dqn_JSONIterator *it)
{
    json_value_s *curr    = Dqn_JSON_IteratorCurrValue(it);
    json_string_s *result = curr ? json_value_as_string(curr) : nullptr;
    return result;
}

json_number_s *Dqn_JSON_IteratorValueIsNumber(Dqn_JSONIterator *it)
{
    json_value_s *curr    = Dqn_JSON_IteratorCurrValue(it);
    json_number_s *result = curr ? json_value_as_number(curr) : nullptr;
    return result;
}

json_value_s *Dqn_JSON_IteratorValueIsBool(Dqn_JSONIterator *it)
{
    json_value_s *curr   = Dqn_JSON_IteratorCurrValue(it);
    json_value_s *result = (curr && (curr->type == json_type_true || curr->type == json_type_false)) ? curr : nullptr;
    return result;
}

size_t Dqn_JSON_IteratorValueArraySize(Dqn_JSONIterator *it)
{
    size_t result = 0;
    if (json_array_s *curr = Dqn_JSON_IteratorValueIsArray(it))
        result = curr->length;
    return result;
}


// NOTE: Dqn_JSON_IteratorKeyValueIs
// -----------------------------------------------------------------------------
Dqn_String8 Dqn_JSON_IteratorKey(Dqn_JSONIterator *it)
{
    json_object_element_s *curr = Dqn_JSON_IteratorCurrObjElement(it);
    Dqn_String8 result = {};
    if (curr) {
        result.data = DQN_CAST(char *)curr->name->string;
        result.size = curr->name->string_size;
    }
    return result;
}

bool Dqn_JSON_IteratorKeyIs(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_object_element_s *curr = Dqn_JSON_IteratorCurrObjElement(it);
    bool result                 = Dqn_JSON_String8Cmp(curr->name, key);
    return result;
}

json_object_s *Dqn_JSON_IteratorKeyValueIsObj(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_object_s         *result = nullptr;
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(it);
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_object(curr->value);
    return result;
}

json_array_s *Dqn_JSON_IteratorKeyValueIsArray(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_array_s          *result = nullptr;
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(it);
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_array(curr->value);
    return result;
}

json_string_s *Dqn_JSON_IteratorKeyValueIsString(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(it);
    json_string_s         *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_string(curr->value);
    return result;
}

json_number_s *Dqn_JSON_IteratorKeyValueIsNumber(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(it);
    json_number_s         *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_number(curr->value);
    return result;
}

json_value_s *Dqn_JSON_IteratorKeyValueIsBool(Dqn_JSONIterator *it, Dqn_String8 key)
{
    json_object_element_s *curr   = Dqn_JSON_IteratorCurrObjElement(it);
    json_value_s          *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = curr->value->type == json_type_true || curr->value->type == json_type_false ? curr->value : nullptr;
    return result;
}

// NOTE: Dqn_JSON_IteratorValueTo
// -----------------------------------------------------------------------------
Dqn_String8 Dqn_JSON_IteratorValueToString(Dqn_JSONIterator *it)
{
    Dqn_String8 result = {};
    if (json_string_s *curr = Dqn_JSON_IteratorValueIsString(it))
        result = Dqn_String8_Init(curr->string, curr->string_size);
    return result;
}

int64_t Dqn_JSON_IteratorValueToI64(Dqn_JSONIterator *it)
{
    int64_t result = {};
    if (json_number_s *curr = Dqn_JSON_IteratorValueIsNumber(it))
        result = Dqn_String8_ToI64(Dqn_String8_Init(curr->number, curr->number_size), 0 /*separator*/);
    return result;
}

uint64_t Dqn_JSON_IteratorValueToU64(Dqn_JSONIterator *it)
{
    uint64_t result = {};
    if (json_number_s *curr = Dqn_JSON_IteratorValueIsNumber(it))
        result = Dqn_String8_ToU64(Dqn_String8_Init(curr->number, curr->number_size), 0 /*separator*/);
    return result;
}

bool Dqn_JSON_IteratorValueToBool(Dqn_JSONIterator *it)
{
    bool result = {};
    if (json_value_s *curr = Dqn_JSON_IteratorValueIsBool(it))
        result = curr->type == json_type_true;
    return result;
}

void Dqn_JSON_IteratorErrorUnknownKeyValue_(Dqn_JSONIterator *it, Dqn_CallSite call_site)
{
    if (!it)
        return;

    json_object_element_s const *curr = Dqn_JSON_IteratorCurrObjElement(it);
    if (!curr)
        return;

    size_t value_type_size = 0;
    char const *value_type = Dqn_JSON_TypeEnumCString(DQN_CAST(json_type_e)curr->value->type, &value_type_size);

    json_string_s const *key = curr->name;
    if (it->flags & json_parse_flags_allow_location_information) {
        json_string_ex_s const *info = DQN_CAST(json_string_ex_s const *)key;
        Dqn_Log_TypeFCallSite(Dqn_LogType_Warning,
                              call_site,
                              "Unknown key-value pair in object [loc=%zu:%zu, key=%.*s, value=%.*s]",
                              info->line_no,
                              info->row_no,
                              key->string_size,
                              key->string,
                              value_type_size,
                              value_type);
    } else {
        Dqn_Log_TypeFCallSite(Dqn_LogType_Warning,
                              call_site,
                              "Unknown key-value pair in object [key=%.*s, value=%.*s]",
                              key->string_size,
                              key->string,
                              value_type_size,
                              value_type);
    }
}

#endif // defined(DQN_JSON_IMPLEMENTATION)
