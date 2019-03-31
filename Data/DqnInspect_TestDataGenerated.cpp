// This is an auto generated file using Dqn_Inspect

//
// ..\Data\DqnInspect_TestData.h
//

#ifndef DQN_INSPECT_DQNINSPECT_TESTDATA_H
#define DQN_INSPECT_DQNINSPECT_TESTDATA_H

 // NOTE: These macros are undefined at the end of the file so to not pollute namespace
#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])
#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)
#define STR_AND_LEN(str) str, CHAR_COUNT(str)

DqnInspect_StructMember const DqnInspect_SampleStruct_StructMembers[] =
{
    {
        STR_AND_LEN("int const *c"), STR_AND_LEN("b"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("c"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int const *const b, c, *"), STR_AND_LEN("d"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int const *const b, c, *d, *"), STR_AND_LEN("e"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("Array"), STR_AND_LEN("lights"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("Array"), STR_AND_LEN("camera_matrixes"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("char          **"), STR_AND_LEN("bitmaps"),
        nullptr, 0, // template_expr and template_expr_len
        2 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("shaders"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("void           *"), STR_AND_LEN("win32_handle"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("ebo"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("vao"), STR_AND_LEN("DQN_INSPECT_META"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("V4"), STR_AND_LEN("draw_color"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("V3"), STR_AND_LEN("lighting_ambient_coeff"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("draw_call_count"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("int"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("a"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("const"), STR_AND_LEN("int"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("g"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int *c"), STR_AND_LEN("h"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int const*"), STR_AND_LEN("i"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
    {
        STR_AND_LEN("int *********c"), STR_AND_LEN("j"),
        nullptr, 0, // template_expr and template_expr_len
        9 // array_dimensions
    },
    {
        STR_AND_LEN("int"), STR_AND_LEN("k"),
        nullptr, 0, // template_expr and template_expr_len
        0 // array_dimensions
    },
    {
        STR_AND_LEN("int *********const j, k, ******"), STR_AND_LEN("l"),
        nullptr, 0, // template_expr and template_expr_len
        6 // array_dimensions
    },
    {
        STR_AND_LEN("int *********const j, k, ******l, *"), STR_AND_LEN("m"),
        nullptr, 0, // template_expr and template_expr_len
        1 // array_dimensions
    },
};

DqnInspect_Struct const DqnInspect_SampleStruct_Struct =
{
    STR_AND_LEN("SampleStruct"),
    DqnInspect_SampleStruct_StructMembers, // members
    ARRAY_COUNT(DqnInspect_SampleStruct_StructMembers) // members_len
};

DqnInspect_Struct const *DqnInspect_GetStruct(SampleStruct const *val)
{
    (void)val;
    DqnInspect_Struct const *result = &DqnInspect_SampleStruct_Struct;
    return result;
}

char const *DqnInspect_EnumWithMetadata_Strings[] = {"Rect", "Square", "Count", };

char const *DqnInspectEnum_Stringify(EnumWithMetadata val, int *len = nullptr)
{
    if (val == EnumWithMetadata::Rect)   { if (len) *len = CHAR_COUNT("Rect"); return DqnInspect_EnumWithMetadata_Strings[0]; }
    if (val == EnumWithMetadata::Square) { if (len) *len = CHAR_COUNT("Square"); return DqnInspect_EnumWithMetadata_Strings[1]; }
    if (val == EnumWithMetadata::Count)  { if (len) *len = CHAR_COUNT("Count"); return DqnInspect_EnumWithMetadata_Strings[2]; }
    return nullptr;
}

char const * DqnInspectMetadata_FilePath(EnumWithMetadata val)
{
    if (val == EnumWithMetadata::Rect) { return "Rect.vert"; }
    if (val == EnumWithMetadata::Square) { return "Square.vert"; }
    return nullptr;
}

bool DqnInspectMetadata_Coords(EnumWithMetadata val, V3 *value)
{
    if (val == EnumWithMetadata::Rect) { *value = V3(1, 2, 3); return true; }
    return false;
}

void                               Function1(int a, float b = {}, char const * c = nullptr, bool e = false, int f = 1, char * g = "Hello world");
void *                             Function2(V3 foo = V3(10, 20, 50), V3 bar = {120, 150, 20}, ...);
Array<int const *, 3> const *const Function3(Array<int, 32> const * foobar);

#undef ARRAY_COUNT
#undef CHAR_COUNT
#undef STR_AND_LEN
#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H

