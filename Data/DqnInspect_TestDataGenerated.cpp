// This is an auto generated file using DqnInspect

 // NOTE: These macros are undefined at the end of the file so to not pollute namespace
#define ARRAY_COUNT(array) sizeof(array)/sizeof((array)[0])
#define CHAR_COUNT(str) (ARRAY_COUNT(str) - 1)
#define STR_AND_LEN(str) str, CHAR_COUNT(str)

enum struct DqnInspectMemberType
{
    SampleStruct,
    SampleStruct_a,
    SampleStruct_b,
    SampleStruct_c,
    SampleStruct_d,
    SampleStruct_e,
    SampleStruct_f,
    SampleStruct_g,
    SampleStruct_h,
    SampleStruct_i,
    SampleStruct_j,
    SampleStruct_k,
    SampleStruct_l,
    SampleStruct_m,
    EnumWithMetadata,
    SampleStruct_ebo,
    SampleStruct_vao,
    SampleStruct_vbo,
    SampleStruct_lights,
    SampleStruct_bitmaps,
    SampleStruct_shaders,
    EnumWithMetadata_Rect,
    EnumWithMetadata_Count,
    EnumWithMetadata_Square,
    SampleStruct_draw_color,
    SampleStruct_win32_handle,
    SampleStruct_camera_matrixes,
    SampleStruct_draw_call_count,
    SampleStruct_lighting_ambient_coeff,
};

enum struct DqnInspectDeclType
{
    V3_,
    V4_,
    int_,
    Array_,
    int_Ptr_,
    void_Ptr_,
    char_Ptr_Ptr_,
    int_const_,
    int_Ptr_Ptr_Ptr_Ptr_Ptr_Ptr_,
    int_Ptr_const_,
    int_const_Ptr_,
    SampleStruct_,
    char_const_Ptr_,
    EnumWithMetadata_,
    int_const_Ptr_const_,
    int_Ptr_Ptr_Ptr_Ptr_Ptr_Ptr_Ptr_Ptr_Ptr_const_,
};

enum struct DqnInspectMetaType
{
    DisplayName,
    OpenGLVersion,
};

//
// ..\Data\DqnInspect_TestData.h
//

#ifndef DQN_INSPECT_DQNINSPECT_TESTDATA_H
#define DQN_INSPECT_DQNINSPECT_TESTDATA_H

char const * DqnInspectMetadata_SampleStruct_ebo_DisplayName = "Element Buffer Object";
char const * DqnInspectMetadata_SampleStruct_vao_DisplayName = "Vertex Array Object";
int DqnInspectMetadata_SampleStruct_vao_OpenGLVersion = 330;
char const * DqnInspectMetadata_SampleStruct_draw_color_DisplayName = "HelloWorld";

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_ebo[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_ebo_DisplayName},
};

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_vao[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_vao_DisplayName},
    { DqnInspectDeclType::int_, DqnInspectMetaType::OpenGLVersion, &DqnInspectMetadata_SampleStruct_vao_OpenGLVersion},
};

DqnInspectMetadata const DqnInspectMetadata_SampleStruct_draw_color[] =
{
    { DqnInspectDeclType::char_const_Ptr_, DqnInspectMetaType::DisplayName, &DqnInspectMetadata_SampleStruct_draw_color_DisplayName},
};

DqnInspectMember const DqnInspect_SampleStruct_Members[] =
{
    {
        DqnInspectMemberType::SampleStruct_ebo,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("ebo"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        DqnInspectMetadata_SampleStruct_ebo, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_ebo), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_vbo,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("vbo"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_vao,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("vao"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        DqnInspectMetadata_SampleStruct_vao, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_vao), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_a,
        DqnInspectDeclType::int *const_,
        STR_AND_LEN("int *const"), STR_AND_LEN("a"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_b,
        DqnInspectDeclType::int const *const_,
        STR_AND_LEN("int const *const"), STR_AND_LEN("b"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_c,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("c"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_d,
        DqnInspectDeclType::int *_,
        STR_AND_LEN("int *"), STR_AND_LEN("d"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_e,
        DqnInspectDeclType::int *_,
        STR_AND_LEN("int *"), STR_AND_LEN("e"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_draw_color,
        DqnInspectDeclType::V4_,
        STR_AND_LEN("V4"), STR_AND_LEN("draw_color"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        DqnInspectMetadata_SampleStruct_draw_color, ARRAY_COUNT(DqnInspectMetadata_SampleStruct_draw_color), // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_lights,
        DqnInspectDeclType::Array_,
        STR_AND_LEN("Array"), STR_AND_LEN("lights"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_camera_matrixes,
        DqnInspectDeclType::Array_,
        STR_AND_LEN("Array"), STR_AND_LEN("camera_matrixes"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_bitmaps,
        DqnInspectDeclType::char **_,
        STR_AND_LEN("char **"), STR_AND_LEN("bitmaps"),
        nullptr, 0, // template_expr and template_expr_len
        2, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_shaders,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("shaders"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_win32_handle,
        DqnInspectDeclType::void *_,
        STR_AND_LEN("void *"), STR_AND_LEN("win32_handle"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_lighting_ambient_coeff,
        DqnInspectDeclType::V3_,
        STR_AND_LEN("V3"), STR_AND_LEN("lighting_ambient_coeff"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_draw_call_count,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("draw_call_count"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_f,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("f"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_g,
        DqnInspectDeclType::int const_,
        STR_AND_LEN("int const"), STR_AND_LEN("g"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_h,
        DqnInspectDeclType::int *const_,
        STR_AND_LEN("int *const"), STR_AND_LEN("h"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_i,
        DqnInspectDeclType::int const*_,
        STR_AND_LEN("int const*"), STR_AND_LEN("i"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_j,
        DqnInspectDeclType::int *********const_,
        STR_AND_LEN("int *********const"), STR_AND_LEN("j"),
        nullptr, 0, // template_expr and template_expr_len
        9, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_k,
        DqnInspectDeclType::int_,
        STR_AND_LEN("int"), STR_AND_LEN("k"),
        nullptr, 0, // template_expr and template_expr_len
        0, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_l,
        DqnInspectDeclType::int ******_,
        STR_AND_LEN("int ******"), STR_AND_LEN("l"),
        nullptr, 0, // template_expr and template_expr_len
        6, // array_dimensions
        nullptr, 0, // metadata array
    },
    {
        DqnInspectMemberType::SampleStruct_m,
        DqnInspectDeclType::int *_,
        STR_AND_LEN("int *"), STR_AND_LEN("m"),
        nullptr, 0, // template_expr and template_expr_len
        1, // array_dimensions
        nullptr, 0, // metadata array
    },
};

DqnInspectStruct const DqnInspect_SampleStruct_Struct =
{
    STR_AND_LEN("SampleStruct"),
    DqnInspect_SampleStruct_Members, // members
    ARRAY_COUNT(DqnInspect_SampleStruct_Members) // members_len
};

DqnInspectStruct const *DqnInspect_Struct(SampleStruct const *)
{
    DqnInspectStruct const *result = &DqnInspect_SampleStruct_Struct;
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

int DqnInspectEnum_Count(EnumWithMetadata) { return 3; }

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
#endif // DQN_INSPECT_DQNINSPECT_TESTDATA_H


#undef ARRAY_COUNT
#undef CHAR_COUNT
#undef STR_AND_LEN
