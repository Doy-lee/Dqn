#if !defined(SHEREDOM_JSON_H_INCLUDED)
    #error Sheredom's json.h (github.com/sheredom/json.h) must be included before this file
#endif

#if !defined(DQN_JSON_H)
#define DQN_JSON_H
// NOTE: Dqn_JSON //////////////////////////////////////////////////////////////////////////////////

void       *Dqn_JSON_ArenaAllocFunc (void *user_data, size_t count);
char const *Dqn_JSON_TypeEnumCString(json_type_e type, size_t *size);
bool        Dqn_JSON_String8Cmp     (json_string_s const *lhs, Dqn_Str8 rhs);

// NOTE: Dqn_JSON_It /////////////////////////////////////////////////////////////////////////
enum Dqn_JSONItEntryType
{
    Dqn_JSON_ItEntryTypeObjElement,
    Dqn_JSON_ItEntryTypeObj,
    Dqn_JSON_ItEntryTypeArrayElement,
    Dqn_JSON_ItEntryTypeArray,
    Dqn_JSON_ItEntryTypeString,
    Dqn_JSON_ItEntryTypeNumber,
};

struct Dqn_JSONItEntry
{
    Dqn_JSONItEntryType type;
    void               *value;
};

struct Dqn_JSONIt
{
    Dqn_JSONItEntry stack[128];
    int             stack_count;
    size_t          flags;
};

Dqn_JSONIt Dqn_JSON_LoadFileToIt(Dqn_Arena *arena, Dqn_Str8 json);

// NOTE: Dqn_JSON_ItPush/Pop /////////////////////////////////////////////////////////////////
bool Dqn_JSON_ItPushObjElement  (Dqn_JSONIt *it, json_object_element_s *element);
bool Dqn_JSON_ItPushObj         (Dqn_JSONIt *it, json_object_s *obj);
bool Dqn_JSON_ItPushArrayElement(Dqn_JSONIt *it, json_array_element_s *element);
bool Dqn_JSON_ItPushArray       (Dqn_JSONIt *it, json_value_s *value);
bool Dqn_JSON_ItPushValue       (Dqn_JSONIt *it, json_value_s *value);
void Dqn_JSON_ItPop             (Dqn_JSONIt *it);

// NOTE: Dqn_JSON_It tree navigation /////////////////////////////////////////////////////////
json_value_s *Dqn_JSON_ItPushCurrValue(Dqn_JSONIt *it);
bool          Dqn_JSON_ItNext(Dqn_JSONIt *it);

#define Dqn_JSON_ItPushCurrValueIterateThenPop(it) \
    for(void *DQN_UNIQUE_NAME(ptr) = Dqn_JSON_ItPushCurrValue(it); DQN_UNIQUE_NAME(ptr); Dqn_JSON_ItPop(it), DQN_UNIQUE_NAME(ptr) = nullptr) \
        while (Dqn_JSON_ItNext(it))

// NOTE: Dqn_JSON_ItCurr /////////////////////////////////////////////////////////////////////
Dqn_JSONItEntry       *Dqn_JSON_ItCurr(Dqn_JSONIt *it);
json_value_s          *Dqn_JSON_ItCurrValue(Dqn_JSONIt *it);
json_object_element_s *Dqn_JSON_ItCurrObjElement(Dqn_JSONIt *it);

// NOTE: Dqn_JSON_ItValueIs //////////////////////////////////////////////////////////////////
json_value_s  *Dqn_JSON_ItValueIs(Dqn_JSONIt *it, json_type_e type);
json_object_s *Dqn_JSON_ItValueIsObj(Dqn_JSONIt *it);
json_array_s  *Dqn_JSON_ItValueIsArray(Dqn_JSONIt *it);
json_string_s *Dqn_JSON_ItValueIsString(Dqn_JSONIt *it);
json_number_s *Dqn_JSON_ItValueIsNumber(Dqn_JSONIt *it);
json_value_s  *Dqn_JSON_ItValueIsBool(Dqn_JSONIt *it);
json_value_s  *Dqn_JSON_ItValueIsNull(Dqn_JSONIt *it);

size_t         Dqn_JSON_ItValueArraySize(Dqn_JSONIt *it);

// NOTE: Dqn_JSON_ItKeyValueIs ///////////////////////////////////////////////////////////////
Dqn_Str8       Dqn_JSON_ItKey(Dqn_JSONIt *it);
bool           Dqn_JSON_ItKeyIs(Dqn_JSONIt *it, Dqn_Str8 key);
json_object_s *Dqn_JSON_ItKeyValueIsObj(Dqn_JSONIt *it, Dqn_Str8 key);
json_array_s  *Dqn_JSON_ItKeyValueIsArray(Dqn_JSONIt *it, Dqn_Str8 key);
json_string_s *Dqn_JSON_ItKeyValueIsString(Dqn_JSONIt *it, Dqn_Str8 key);
json_number_s *Dqn_JSON_ItKeyValueIsNumber(Dqn_JSONIt *it, Dqn_Str8 key);
json_value_s  *Dqn_JSON_ItKeyValueIsBool(Dqn_JSONIt *it, Dqn_Str8 key);
json_value_s  *Dqn_JSON_ItKeyValueIsNull(Dqn_JSONIt *it, Dqn_Str8 key);

// NOTE: Dqn_JSON_ItValueTo //////////////////////////////////////////////////////////////////
Dqn_Str8       Dqn_JSON_ItValueToString(Dqn_JSONIt *it);
int64_t        Dqn_JSON_ItValueToI64(Dqn_JSONIt *it);
uint64_t       Dqn_JSON_ItValueToU64(Dqn_JSONIt *it);
bool           Dqn_JSON_ItValueToBool(Dqn_JSONIt *it);

#define Dqn_JSON_ItErrorUnknownKeyValue(it) Dqn_JSON_ItErrorUnknownKeyValue_(it, DQN_CALL_SITE)
void Dqn_JSON_ItErrorUnknownKeyValue_(Dqn_JSONIt *it, Dqn_CallSite call_site);

#endif // DQN_JSON_H

#if defined(DQN_JSON_IMPLEMENTATION)
// NOTE: Dqn_JSON //////////////////////////////////////////////////////////////////////////////////
void *Dqn_JSON_ArenaAllocFunc(void *user_data, size_t count)
{
    void *result = NULL;
    if (!user_data)
        return result;

    Dqn_Arena *arena = DQN_CAST(Dqn_Arena*)user_data;
    result = Dqn_Arena_Alloc(arena, count, alignof(json_value_s), Dqn_ZeroMem_No);
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

bool Dqn_JSON_String8Cmp(json_string_s const *lhs, Dqn_Str8 key)
{
    bool result = false;
    if (lhs && Dqn_Str8_HasData(key)) {
        Dqn_Str8 lhs_string = Dqn_Str8_Init(lhs->string, lhs->string_size);
        result              = Dqn_Str8_Eq(lhs_string, key);
    }
    return result;
}

// NOTE: Dqn_JSON_It ///////////////////////////////////////////////////////////////////////////////
Dqn_JSONIt Dqn_JSON_LoadFileToIt(Dqn_Arena *arena, Dqn_Str8 json)
{
    json_parse_result_s parse_result = {};
    json_value_ex_s    *ex_value     =
        DQN_CAST(json_value_ex_s *) json_parse_ex(json.data,
                                                  json.size,
                                                  json_parse_flags_allow_location_information,
                                                  Dqn_JSON_ArenaAllocFunc,
                                                  arena,
                                                  &parse_result);

    Dqn_JSONIt result = {};
    Dqn_JSON_ItPushValue(&result, &ex_value->value);
    return result;
}

// NOTE: Dqn_JSON_ItPush/Pop ///////////////////////////////////////////////////////////////////////
bool Dqn_JSON_ItPushObjElement(Dqn_JSONIt *it, json_object_element_s *element)
{
    if (!it || !element)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_ItEntryTypeObjElement, element};
    return true;
}

bool Dqn_JSON_ItPushObj(Dqn_JSONIt *it, json_object_s *obj)
{
    if (!it || !obj)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_ItEntryTypeObj, obj};
    return true;
}

bool Dqn_JSON_ItPushArrayElement(Dqn_JSONIt *it, json_array_element_s *element)
{
    if (!it || !element)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_ItEntryTypeArrayElement, element};
    return true;
}

bool Dqn_JSON_ItPushArray(Dqn_JSONIt *it, json_value_s *value)
{
    if (!it || !value || json_value_as_array(value) == nullptr)
        return false;
    DQN_ASSERT(it->stack_count < DQN_ARRAY_ICOUNT(it->stack));
    it->stack[it->stack_count++] = {Dqn_JSON_ItEntryTypeArray, value};
    return true;
}

bool Dqn_JSON_ItPushValue(Dqn_JSONIt *it, json_value_s *value)
{
    bool result = false;
    if (!it || !value)
        return result;

    if (value->type == json_type_object) {
        result = Dqn_JSON_ItPushObj(it, json_value_as_object(value));
    } else if (value->type == json_type_array) {
        result = Dqn_JSON_ItPushArray(it, value);
    }

    return result;
}

void Dqn_JSON_ItPop(Dqn_JSONIt *it)
{
    if (!it)
        return;
    DQN_ASSERT(it->stack_count > 0);
    if (it->stack_count > 0)
        it->stack_count--;
}

// NOTE: Dqn_JSON_It JSON tree navigation //////////////////////////////////////////////////////////
json_value_s *Dqn_JSON_ItPushCurrValue(Dqn_JSONIt *it)
{
    json_value_s          *result = nullptr;
    Dqn_JSONItEntry *curr   = Dqn_JSON_ItCurr(it);
    if (!curr)
        return result;

    if (curr->type == Dqn_JSON_ItEntryTypeObjElement) {
        json_object_element_s *element = DQN_CAST(json_object_element_s *) curr->value;
        result                         = element->value;
    } else if (curr->type == Dqn_JSON_ItEntryTypeArrayElement) {
        json_array_element_s *element = DQN_CAST(json_array_element_s *) curr->value;
        result                        = element->value;
    } else {
        result = DQN_CAST(json_value_s *) curr->value;
    }

    if (result->type == json_type_array) {
        json_array_s *array = json_value_as_array(result);
        DQN_ASSERT(array);
        Dqn_JSON_ItPushArray(it, result);
    } else if (result->type == json_type_object) {
        json_object_s *obj = json_value_as_object(result);
        DQN_ASSERT(obj);
        Dqn_JSON_ItPushObj(it, obj);
    }

    return result;
}

bool Dqn_JSON_ItNext(Dqn_JSONIt *it)
{
    Dqn_JSONItEntry *curr = Dqn_JSON_ItCurr(it);
    if (!curr)
        return false;

    json_object_element_s *obj_element   = nullptr;
    json_array_element_s  *array_element = nullptr;
    if (curr->type == Dqn_JSON_ItEntryTypeObj) {
        auto *obj   = DQN_CAST(json_object_s *) curr->value;
        obj_element = obj->start;
    } else if (curr->type == Dqn_JSON_ItEntryTypeObjElement) {
        auto *element = DQN_CAST(json_object_element_s *) curr->value;
        obj_element   = element->next;
        Dqn_JSON_ItPop(it);
    } else if (curr->type == Dqn_JSON_ItEntryTypeArray) {
        auto *value   = DQN_CAST(json_value_s *) curr->value;
        auto *array   = json_value_as_array(value);
        array_element = array->start;
    } else if (curr->type == Dqn_JSON_ItEntryTypeArrayElement) {
        auto *element = DQN_CAST(json_array_element_s *) curr->value;
        array_element = element->next;
        Dqn_JSON_ItPop(it);
    } else {
        Dqn_JSON_ItPop(it);
    }

    if (obj_element)
        Dqn_JSON_ItPushObjElement(it, obj_element);
    else if (array_element)
        Dqn_JSON_ItPushArrayElement(it, array_element);

    bool result = obj_element || array_element;
    return result;
}

// NOTE: Dqn_JSON_ItCurr ///////////////////////////////////////////////////////////////////////////
Dqn_JSONItEntry *Dqn_JSON_ItCurr(Dqn_JSONIt *it)
{
    Dqn_JSONItEntry *result = nullptr;
    if (!it || it->stack_count <= 0)
        return result;

    result = &it->stack[it->stack_count - 1];
    return result;
}

json_value_s *Dqn_JSON_ItCurrValue(Dqn_JSONIt *it)
{
    json_value_s *result = nullptr;
    Dqn_JSONItEntry *curr = Dqn_JSON_ItCurr(it);
    if (!curr)
        return result;

    if (curr->type == Dqn_JSON_ItEntryTypeObjElement) {
        auto *element = DQN_CAST(json_object_element_s *)curr->value;
        result = element->value;
    } else if (curr->type == Dqn_JSON_ItEntryTypeArrayElement) {
        auto *element = DQN_CAST(json_array_element_s *)curr->value;
        result = element->value;
    } else if (curr->type == Dqn_JSON_ItEntryTypeString ||
               curr->type == Dqn_JSON_ItEntryTypeNumber ||
               curr->type == Dqn_JSON_ItEntryTypeObj    ||
               curr->type == Dqn_JSON_ItEntryTypeArray)
    {
        result = DQN_CAST(json_value_s *)curr->value;
    }

    return result;
}

json_object_element_s *Dqn_JSON_ItCurrObjElement(Dqn_JSONIt *it)
{
    Dqn_JSONItEntry *curr   = Dqn_JSON_ItCurr(it);
    auto                  *result = (curr && curr->type == Dqn_JSON_ItEntryTypeObjElement)
                                        ? DQN_CAST(json_object_element_s *) curr->value
                                        : nullptr;
    return result;
}

// NOTE: Dqn_JSON_ItValueIs ////////////////////////////////////////////////////////////////////////
json_value_s *Dqn_JSON_ItValueIs(Dqn_JSONIt *it, json_type_e type)
{
    json_value_s  *curr  = Dqn_JSON_ItCurrValue(it);
    json_value_s *result = (curr && type == curr->type) ? curr : nullptr;
    return result;
}

json_object_s *Dqn_JSON_ItValueIsObj(Dqn_JSONIt *it)
{
    json_value_s *curr    = Dqn_JSON_ItCurrValue(it);
    json_object_s *result = curr ? json_value_as_object(curr) : nullptr;
    return result;
}

json_array_s *Dqn_JSON_ItValueIsArray(Dqn_JSONIt *it)
{
    json_value_s *curr   = Dqn_JSON_ItCurrValue(it);
    json_array_s *result = curr ? json_value_as_array(curr) : nullptr;
    return result;
}

json_string_s *Dqn_JSON_ItValueIsString(Dqn_JSONIt *it)
{
    json_value_s *curr    = Dqn_JSON_ItCurrValue(it);
    json_string_s *result = curr ? json_value_as_string(curr) : nullptr;
    return result;
}

json_number_s *Dqn_JSON_ItValueIsNumber(Dqn_JSONIt *it)
{
    json_value_s *curr    = Dqn_JSON_ItCurrValue(it);
    json_number_s *result = curr ? json_value_as_number(curr) : nullptr;
    return result;
}

json_value_s *Dqn_JSON_ItValueIsBool(Dqn_JSONIt *it)
{
    json_value_s *curr   = Dqn_JSON_ItCurrValue(it);
    json_value_s *result = (curr && (curr->type == json_type_true || curr->type == json_type_false)) ? curr : nullptr;
    return result;
}

json_value_s *Dqn_JSON_ItValueIsNull(Dqn_JSONIt *it)
{
    json_value_s *curr   = Dqn_JSON_ItCurrValue(it);
    json_value_s *result = (curr && (curr->type == json_type_null)) ? curr : nullptr;
    return result;
}

size_t Dqn_JSON_ItValueArraySize(Dqn_JSONIt *it)
{
    size_t result = 0;
    if (json_array_s *curr = Dqn_JSON_ItValueIsArray(it))
        result = curr->length;
    return result;
}


// NOTE: Dqn_JSON_ItKeyValueIs /////////////////////////////////////////////////////////////////////
Dqn_Str8 Dqn_JSON_ItKey(Dqn_JSONIt *it)
{
    json_object_element_s *curr = Dqn_JSON_ItCurrObjElement(it);
    Dqn_Str8 result = {};
    if (curr) {
        result.data = DQN_CAST(char *)curr->name->string;
        result.size = curr->name->string_size;
    }
    return result;
}

bool Dqn_JSON_ItKeyIs(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_element_s *curr = Dqn_JSON_ItCurrObjElement(it);
    bool result                 = Dqn_JSON_String8Cmp(curr->name, key);
    return result;
}

json_object_s *Dqn_JSON_ItKeyValueIsObj(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_s         *result = nullptr;
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_object(curr->value);
    return result;
}

json_array_s *Dqn_JSON_ItKeyValueIsArray(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_array_s          *result = nullptr;
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_array(curr->value);
    return result;
}

json_string_s *Dqn_JSON_ItKeyValueIsString(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    json_string_s         *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_string(curr->value);
    return result;
}

json_number_s *Dqn_JSON_ItKeyValueIsNumber(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    json_number_s         *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = json_value_as_number(curr->value);
    return result;
}

json_value_s *Dqn_JSON_ItKeyValueIsBool(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    json_value_s          *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = curr->value->type == json_type_true || curr->value->type == json_type_false ? curr->value : nullptr;
    return result;
}

json_value_s *Dqn_JSON_ItKeyValueIsNull(Dqn_JSONIt *it, Dqn_Str8 key)
{
    json_object_element_s *curr   = Dqn_JSON_ItCurrObjElement(it);
    json_value_s          *result = nullptr;
    if (curr && Dqn_JSON_String8Cmp(curr->name, key))
        result = curr->value->type == json_type_null ? curr->value : nullptr;
    return result;
}


// NOTE: Dqn_JSON_ItValueTo ////////////////////////////////////////////////////////////////////////
Dqn_Str8 Dqn_JSON_ItValueToString(Dqn_JSONIt *it)
{
    Dqn_Str8 result = {};
    if (json_string_s *curr = Dqn_JSON_ItValueIsString(it))
        result = Dqn_Str8_Init(curr->string, curr->string_size);
    return result;
}

int64_t Dqn_JSON_ItValueToI64(Dqn_JSONIt *it)
{
    int64_t result = {};
    if (json_number_s *curr = Dqn_JSON_ItValueIsNumber(it))
        result = Dqn_Str8_ToI64(Dqn_Str8_Init(curr->number, curr->number_size), 0 /*separator*/).value;
    return result;
}

uint64_t Dqn_JSON_ItValueToU64(Dqn_JSONIt *it)
{
    uint64_t result = {};
    if (json_number_s *curr = Dqn_JSON_ItValueIsNumber(it))
        result = Dqn_Str8_ToU64(Dqn_Str8_Init(curr->number, curr->number_size), 0 /*separator*/).value;
    return result;
}

bool Dqn_JSON_ItValueToBool(Dqn_JSONIt *it)
{
    bool result = {};
    if (json_value_s *curr = Dqn_JSON_ItValueIsBool(it))
        result = curr->type == json_type_true;
    return result;
}

void Dqn_JSON_ItErrorUnknownKeyValue_(Dqn_JSONIt *it, Dqn_CallSite call_site)
{
    if (!it)
        return;

    json_object_element_s const *curr = Dqn_JSON_ItCurrObjElement(it);
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
                              DQN_CAST(int)key->string_size,
                              key->string,
                              DQN_CAST(int)value_type_size,
                              value_type);
    } else {
        Dqn_Log_TypeFCallSite(Dqn_LogType_Warning,
                              call_site,
                              "Unknown key-value pair in object [key=%.*s, value=%.*s]",
                              DQN_CAST(int)key->string_size,
                              key->string,
                              DQN_CAST(int)value_type_size,
                              value_type);
    }
}
#endif // defined(DQN_JSON_IMPLEMENTATION)
